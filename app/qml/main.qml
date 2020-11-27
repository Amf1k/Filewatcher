import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Dialogs 1.2
import QtQuick.Controls 2.0
import amf1k.models 1.0

import "qrc:/common.js" as JS

Window {
    width: 1024
    height: 768
    visible: true

    FileWatcherModel {
        id: fileWatcherModel
    }

    Item {
        id: currentDirContainer
        height: 50
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 10
        }
        Text {
            id: currentDirLabel
            text: "Current Dir:"
            anchors {
                top: parent.top
                left: parent.left
                right: fileDialogBtn.right
                rightMargin: 10
            }
            wrapMode: Text.WordWrap
            font {
                pixelSize: 16
                weight: Font.Bold
            }
        }
        Text {
            id: currentDirValue
            text: fileWatcherModel.currentWatchPath.length
                  === 0 ? "Not chosen" : fileWatcherModel.currentWatchPath
            anchors {
                top: currentDirLabel.bottom
                left: currentDirLabel.left
                right: fileDialogBtn.right
                rightMargin: 10
                topMargin: 10
            }
            wrapMode: Text.WordWrap
            font {
                pixelSize: 14
            }
        }
        Button {
            id: fileDialogBtn
            text: "Choose dir"
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
            }
            onClicked: {
                fileDialog.open()
            }
        }
        FileDialog {
            id: fileDialog
            title: "Please choose a dir"
            folder: shortcuts.home
            selectFolder: true
            selectMultiple: false
            onAccepted: {
                console.log("You chose: " + fileDialog.fileUrl)
                fileWatcherModel.currentWatchPath = fileDialog.fileUrl
            }
            onRejected: {
                console.log("Canceled")
            }
        }
    }

    Item {
        id: tableContainer
        anchors {
            top: currentDirContainer.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: 10
        }

        ListView {
            id: fileList
            anchors.fill: parent
            model: fileWatcherModel
            clip: true
            footer: Item {
                height: 30
                width: fileList.width
                Rectangle {
                    color: "gray"
                    anchors.fill: parent
                    opacity: 0.6
                }
                Text {
                    id: coutTextLabel
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    anchors {
                        left: parent.left
                        leftMargin: 5
                    }
                    text: "Count files: "
                }
                Text {
                    id: coutTextValue
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                    anchors {
                        left: coutTextLabel.right
                        right: parent.right
                        rightMargin: 5
                        leftMargin: 5
                    }
                    text: fileList.count
                }
            }
            header: TableRowDelegate {
                width: fileList.width
                fileNameValue: "File Name"
                fileModified: "File Modified Date"
                fileSize: "File Size"
                isHead: true
            }
            delegate: TableRowDelegate {
                width: fileList.width
                fileNameValue: model.fileName
                fileModified: model.fileModified
                fileSize: model.fileSizeByte
                onOpenRename: {
                    console.log(fileName)
                    renameWindow.open(fileName)
                }
            }
        }
    }
    RenameWindow {
        id: renameWindow
        onRenamed: {
            fileWatcherModel.rename(oldName, newName)
        }
    }
}
