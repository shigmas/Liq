import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Imagine 2.12

import Points 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    id: mainWindow
    //color: "steelblue"

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Quit"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "USB"
            
            Repeater {
                //model: ['foo','bar']//usbDevices
                model: usbDevices
                MenuItem {
                    text: modelData
                    checkable: true
                    onToggled: {
                        pointsItem.update()
                        controller.SetUsbDevice(model.index, toggled)
                    }
                }
            }
        }
    }

    PointsItem {
        id: pointsItem
        anchors.fill: parent
        // width: parent.width
        // Layout.alignment: Qt.AlignTop | Qt.AlignBottom
        // Layout.fillWidth: true
        // Layout.fillHeight: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("click")
                update()
            }
        }
    }
}
