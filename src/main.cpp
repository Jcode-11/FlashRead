#include "DocumentController.h"
#include "WorkspaceController.h"
#include "diagram/DiagramImageProvider.h"
#include "webview/MarkdownWebViewItem.h"

#include <QFile>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>

#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>

namespace {
void messageLogger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QFile logFile(QCoreApplication::applicationDirPath() + QStringLiteral("/flashread_debug.log"));
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    QTextStream out(&logFile);
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    const char *typeStr = "INFO";
    switch (type) {
    case QtDebugMsg: typeStr = "DEBUG"; break;
    case QtInfoMsg: typeStr = "INFO"; break;
    case QtWarningMsg: typeStr = "WARN"; break;
    case QtCriticalMsg: typeStr = "CRIT"; break;
    case QtFatalMsg: typeStr = "FATAL"; break;
    }
    out << "[" << timeStr << "] [" << typeStr << "] " << msg << "\n";
    out.flush();
}

class FileOpenHandler final : public QObject
{
public:
    explicit FileOpenHandler(WorkspaceController *workspace)
        : m_workspace(workspace)
    {
    }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if (event->type() == QEvent::FileOpen) {
            const QUrl url = static_cast<QFileOpenEvent *>(event)->url();
            if (url.isLocalFile()) {
                QTimer::singleShot(0, m_workspace, [workspace = m_workspace, url]() {
                    workspace->handleUrl(url);
                });
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }

private:
    WorkspaceController *m_workspace;
};
} // namespace

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageLogger);
    QGuiApplication app(argc, argv);
    app.setOrganizationName("FlashRead");
    app.setApplicationName("FlashRead");
    app.setWindowIcon(QIcon(":/FlashRead/resources/FlashRead.png"));

    qDebug() << "=== FlashRead Starting ===";

    qmlRegisterType<MarkdownWebViewItem>("FlashRead.WebView", 1, 0, "MarkdownWebView");

    DocumentController documentController;
    WorkspaceController workspaceController(&documentController);
    FileOpenHandler fileOpenHandler(&workspaceController);
    app.installEventFilter(&fileOpenHandler);

    QQmlApplicationEngine engine;
    engine.addImageProvider("diagram", new Diagram::DiagramImageProvider());
    engine.rootContext()->setContextProperty("systemFontFamily",
        QFontDatabase::systemFont(QFontDatabase::GeneralFont).family());
    engine.rootContext()->setContextProperty("monospaceFontFamily",
        QFontDatabase::systemFont(QFontDatabase::FixedFont).family());
    engine.rootContext()->setContextProperty("documentController", &documentController);
    engine.rootContext()->setContextProperty("workspaceController", &workspaceController);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("FlashRead", "Main");

    const QStringList arguments = app.arguments();
    if (arguments.size() > 1 && !arguments.at(1).startsWith('-')) {
        const QString initialPath = arguments.at(1);
        QTimer::singleShot(0, &workspaceController, [&workspaceController, initialPath]() {
            workspaceController.handleUrl(QUrl::fromLocalFile(initialPath));
        });
    } else {
        QTimer::singleShot(0, &workspaceController, [&workspaceController]() {
            QString fileToOpen;
            for (const QVariant &entry : workspaceController.recentItems()) {
                QVariantMap map = entry.toMap();
                if (map.value(QStringLiteral("type")).toString() == QStringLiteral("file")) {
                    QString path = map.value(QStringLiteral("path")).toString();
                    if (QFile::exists(path)) {
                        fileToOpen = path;
                        break;
                    }
                }
            }
            if (fileToOpen.isEmpty()) {
                QString samplePath = QCoreApplication::applicationDirPath() + QStringLiteral("/sample.md");
                if (QFile::exists(samplePath)) {
                    fileToOpen = samplePath;
                } else {
                    samplePath = QCoreApplication::applicationDirPath() + QStringLiteral("/../sample.md");
                    if (QFile::exists(samplePath)) {
                        fileToOpen = samplePath;
                    }
                }
            }
            if (!fileToOpen.isEmpty()) {
                workspaceController.openPath(fileToOpen);
            }
        });
    }

    return app.exec();
}
