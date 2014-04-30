import QtQuick 2.0

Rectangle {
    width: 360
    height: 360
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }

        Text {
            id: text1
            x: 13
            y: -85
            text: qsTr("Local Nodes")
            font.pixelSize: 22
        }

        Text {
            id: text2
            x: 218
            y: -40
            text: qsTr("Pool Nodes")
            font.pixelSize: 22
        }
    }
}
