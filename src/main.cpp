#include "DocumentController.h"
#include "WorkspaceController.h"
#include "diagram/DiagramImageProvider.h"

#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>

namespace {
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
    QGuiApplication app(argc, argv);
    app.setOrganizationName("FlashRead");
    app.setApplicationName("FlashRead");
    app.setWindowIcon(QIcon(":/FlashRead/resources/FlashRead.png"));

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
    }

    return app.exec();
}
