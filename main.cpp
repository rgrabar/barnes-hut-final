#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>
#include <QQuickStyle>

int main(int argc, char *argv[]) {
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    QGuiApplication app(argc, argv);

    // The native Windows style ignores custom contentItem/background,
    // which ColorButton.qml relies on.
    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///scenegraph/openglunderqml/main.qml")));

    return app.exec();
}
