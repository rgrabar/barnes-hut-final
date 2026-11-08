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
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 12
        spacing: 8

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

        Text {
            text: "θ = " + bh.theta.toFixed(2)
            color: "white"
            font.pointSize: 14
            Layout.leftMargin: 8
        }

        Slider {
            id: thetaSlider
            from: 0.0
            to: 2.0
            value: bh.theta
            onMoved: bh.theta = value
            Layout.preferredWidth: 160
        }
    }
}
