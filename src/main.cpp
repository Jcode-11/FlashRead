#include "DocumentController.h"
#include "WorkspaceController.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("FlashRead");
    app.setApplicationName("FlashRead");

    DocumentController documentController;
    WorkspaceController workspaceController(&documentController);
    QQmlApplicationEngine engine;
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
            workspaceController.openPath(initialPath);
        });
    }

    return app.exec();
}
