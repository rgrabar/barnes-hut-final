import QtQuick 6.0
import QtQuick.Controls 2.15

Button {

    id: control

    property color backgroundColor: "white"

    // Let the label drive the size: implicitWidth/Height come from the
    // contentItem plus this padding, so the text is never clipped.
    padding: 12

    contentItem: Text {
        id: a_text
        text: control.text
        font.family: "Geometria Medium"
        font.pointSize: 18
        color: "#26282a"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        border.color: "#26282a"
        border.width: 1
        radius: 4
        color: control.backgroundColor
    }
}
