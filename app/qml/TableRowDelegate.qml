import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    height: 30
    property string fileNameValue: ""
    property string fileModified: ""
    property string fileSize: ""
    property bool isHead: false
    property color _innerRectangleColor: root.isHead ? "transparent" : "white"
    property var _fontWeight: root.isHead ? Font.Bold : Font.Normal

    signal openRename(string fileName)

    Rectangle {
        visible: root.isHead
        color: "gray"
        anchors.fill: parent
        opacity: 0.6
    }

    Row {
        anchors.fill: parent
        Rectangle {
            height: parent.height
            width: parent.width * 0.4
            color: root._innerRectangleColor
            border.color: "black"
            Text {
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 5
                    rightMargin: 5
                }
                text: root.fileNameValue
                font.weight: root._fontWeight
            }
        }
        Rectangle {
            height: parent.height
            width: parent.width * 0.2
            color: root._innerRectangleColor
            border.color: "black"
            Text {
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 5
                    rightMargin: 5
                }
                text: root.fileModified
                font.weight: root._fontWeight
            }
        }
        Rectangle {
            height: parent.height
            width: parent.width * 0.2
            color: root._innerRectangleColor
            border.color: "black"
            Text {
                height: parent.height
                verticalAlignment: Text.AlignVCenter
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 5
                    rightMargin: 5
                }
                text: root.fileSize
                font.weight: root._fontWeight
            }
        }
        Rectangle {
            height: parent.height
            width: parent.width * 0.2
            color: root._innerRectangleColor
            border.color: "black"
            Button {
                visible: !root.isHead
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                    margins: 5
                }
                text: "Rename"
                onClicked: {
                    root.openRename(root.fileNameValue)
                }
            }
        }
    }
}
