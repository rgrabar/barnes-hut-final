import QtQuick 6.0
import QtQuick.Controls 2.15

Button {

    id: control

    property color backgroundColor: "white"

    width: t_metrics.tightBoundingRect.width ? t_metrics.tightBoundingRect.width : 100
    height:t_metrics.tightBoundingRect.height ? t_metrics.tightBoundingRect.height : 40

    contentItem: Text {
        id: a_text
        text: control.text
        font.family: "Geometria Medium"
        font.pointSize: 18

        /*
        Component.onCompleted: {
            console.log(text)
            console.log(t_metrics.tightBoundingRect.width)
        }
        */
    }

    TextMetrics {
        id: t_metrics
        font: a_text.font
        text: a_text.text
    }

    background: Rectangle {
        width: t_metrics.tightBoundingRect.width ? t_metrics.tightBoundingRect.width : 100
        height: t_metrics.tightBoundingRect.height ? t_metrics.tightBoundingRect.height : 40
        border.color: "#26282a"
        border.width: 1
        radius: 4
        color: backgroundColor

        /*
        Component.onCompleted: {
            width: t_metrics.tightBoundingRect.width ? t_metrics.tightBoundingRect.width : 150
            height: t_metrics.tightBoundingRect.height ? t_metrics.tightBoundingRect.height : 150

             console.log(t_metrics.tightBoundingRect.width)
             console.log(width)
            //console.log(t_metrics.tightBoundingRect.width)
        }
        */
    }
}
