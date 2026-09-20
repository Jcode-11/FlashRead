#include "DocumentController.h"
#include "MarkdownRenderer.h"

#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QThreadPool>
#include <QVariantMap>

namespace {
constexpr qint64 MaxDirectReadBytes = 512 * 1024;

struct ParseResult {
    quint64 requestId = 0;
    QString filePath;
    QString title;
    QString content;
    QString renderedContent;
    QString statusMessage;
    bool truncated = false;
    QVariantList outline;
    bool success = false;
};
} // namespace

DocumentController::DocumentController(QObject *parent)
    : QObject(parent)
{
}

QString DocumentController::content() const
{
    return m_content;
}

QString DocumentController::renderedContent() const
{
    return m_renderedContent;
}

QString DocumentController::filePath() const
{
    return m_filePath;
}

QString DocumentController::title() const
{
    return m_title;
}

QString DocumentController::statusMessage() const
{
    return m_statusMessage;
}

bool DocumentController::truncated() const
{
    return m_truncated;
}

bool DocumentController::isLoading() const
{
    return m_isLoading;
}

QVariantList DocumentController::outline() const
{
    return m_outline;
}

void DocumentController::openUrl(const QUrl &url)
{
    if (!url.isLocalFile()) {
        m_statusMessage = tr("Only local files can be opened.");
        emit documentChanged();
        return;
    }

    openPath(url.toLocalFile());
}

void DocumentController::openPath(const QString &path)
{
    const quint64 reqId = ++m_currentRequestId;

    if (path.isEmpty()) {
        m_content.clear();
        m_renderedContent.clear();
        m_filePath.clear();
        m_title.clear();
        m_statusMessage.clear();
        m_truncated = false;
        m_isLoading = false;
        m_outline.clear();
        emit documentChanged();
        return;
    }

    const QFileInfo fileInfo(path);
    const QString absolutePath = fileInfo.absoluteFilePath();
    m_filePath = absolutePath;
    m_title = fileInfo.fileName();
    m_isLoading = true;
    m_statusMessage = tr("Loading...");
    emit documentChanged();

    QThreadPool::globalInstance()->start([this, absolutePath, reqId]() {
        QFile file(absolutePath);
        ParseResult result;
        result.requestId = reqId;
        result.filePath = absolutePath;
        result.title = QFileInfo(absolutePath).fileName();

        if (!file.open(QIODevice::ReadOnly)) {
            result.statusMessage = tr("Unable to open this file.");
            result.success = false;
        } else {
            QByteArray bytes = file.read(MaxDirectReadBytes);
            const bool reachedReadLimit = bytes.size() == MaxDirectReadBytes;
            if (reachedReadLimit) {
                const qsizetype lastNewline = bytes.lastIndexOf('\n');
                if (lastNewline > 0) {
                    bytes.truncate(lastNewline + 1);
                }
            }

            result.content = QString::fromUtf8(bytes);
            result.renderedContent = MarkdownRenderer::render(bytes);
            result.truncated = file.size() > bytes.size();
            result.statusMessage = result.truncated
                ? tr("Showing the first 512 KB. The rest will be parsed in the background.")
                : tr("Ready");
            result.success = true;

            const QRegularExpression headingExpression(
                QStringLiteral(R"(^(#{1,6})\s+(.+?)\s*#*\s*$)"),
                QRegularExpression::MultilineOption
            );
            QRegularExpressionMatchIterator it = headingExpression.globalMatch(result.content);
            while (it.hasNext()) {
                const auto match = it.next();
                result.outline.append(QVariantMap {
                    {"title", match.captured(2).trimmed()},
                    {"level", match.captured(1).size()},
                    {"progress", result.content.isEmpty() ? 0.0 : static_cast<double>(match.capturedStart()) / result.content.size()}
                });
            }
        }

        QMetaObject::invokeMethod(this, [this, res = std::move(result)]() mutable {
            if (res.requestId != m_currentRequestId.load()) {
                return; // User has switched to another file, drop stale result
            }

            m_content = std::move(res.content);
            m_renderedContent = std::move(res.renderedContent);
            m_filePath = std::move(res.filePath);
            m_title = std::move(res.title);
            m_statusMessage = std::move(res.statusMessage);
            m_truncated = res.truncated;
            m_outline = std::move(res.outline);
            m_isLoading = false;
            emit documentChanged();
        }, Qt::QueuedConnection);
    });
}
