#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>
#include <QQmlEngine>

#include "Controller.h"
#include "PointsItem.h"

int main(int argc, char *argv[])
{
    //    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<PointsItem>("Points", 1, 0, "PointsItem");
    QQmlApplicationEngine engine;

    // Maybe I made this a singleton because I didn't know if this would
    // be first or the QML one. But, obviously, QML isn't calling GetInstance,
    // so we don't need a singleton.
    Controller* controller = Controller::GetInstance();
    controller->SetContext(engine.rootContext());
    engine.load(QUrl(QStringLiteral("qrc:/Liq/qml/liq.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

