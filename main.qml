import QtQuick 6.0
import BarnesHut 1.0
import QtQuick.Controls
import QtQuick.Layouts

Window {
    title: "Barnes-Hut"
    id: page
    visible: true
    width: 800
    height: 800

    BarnesHut {
        id: bh
        anchors.fill: parent
        focus: true
    }

    RowLayout {
        id: rowLayout
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        spacing:2

        ColorButton {
            text: qsTr("Draw quad tree")
            onClicked: {
                bh.drawTree()
            }
        }

        ColorButton {
            text: qsTr("Pause")
            backgroundColor: "Red"
            onClicked: {
                text = text === "Pause" ? "Play" : "Pause";
                backgroundColor = text === "Pause" ? "Red" : "Green";
                bh.togglePause()
            }
        }
    }
}
