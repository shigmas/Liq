QT += quickcontrols2 qml quick widgets

TEMPLATE = app

CONFIG += c++14

SOURCES += \
    src/main.cpp

HEADERS += \
    src/Controller.h             \
    src/PointsItem.h             \


SOURCES += \
    src/Controller.cpp           \
    src/PointsItem.cpp           \

RESOURCES += \
    qml.qrc

INCLUDEPATH += $$OUT_PWD/include
LIBS += -L$$OUT_PWD/libs -lrplidar_sdk
