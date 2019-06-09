#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QDebug>
#include <QQmlEngine>
#include <QQuickStyle>

#include "Controller.h"
#include "PointsItem.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<PointsItem>("Points", 1, 0, "PointsItem");
    QQmlApplicationEngine engine;
    
    Controller* controller = Controller::GetInstance();
    controller->SetContext(engine.rootContext());
    engine.load(QUrl(QStringLiteral("qrc:/qml/liq.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}

