#include "DocumentController.h"
#include "MarkdownRenderer.h"

#include <QFile>
#include <QFileInfo>

namespace {
constexpr qint64 InitialReadBytes = 256 * 1024;
}

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
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        m_content.clear();
        m_renderedContent.clear();
        m_filePath = path;
        m_title = QFileInfo(path).fileName();
        m_statusMessage = tr("Unable to open this file.");
        m_truncated = false;
        emit documentChanged();
        return;
    }

    QByteArray bytes = file.read(InitialReadBytes);
    const bool reachedReadLimit = bytes.size() == InitialReadBytes;
    if (reachedReadLimit) {
        const qsizetype lastNewline = bytes.lastIndexOf('\n');
        if (lastNewline > 0) {
            bytes.truncate(lastNewline + 1);
        }
    }

    m_content = QString::fromUtf8(bytes);
    m_renderedContent = MarkdownRenderer::render(bytes);
    m_filePath = QFileInfo(path).absoluteFilePath();
    m_title = QFileInfo(path).fileName();
    m_truncated = file.size() > bytes.size();
    m_statusMessage = m_truncated
        ? tr("Showing the first 256 KB. The rest will be parsed in the background.")
        : tr("Ready");
    emit documentChanged();
}
