import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: renameFileWindow
    property string oldName: ""
    signal renamed(string oldName, string newName);
    modality: Qt.ApplicationModal
    title: "Rename file - " + oldName;
    width: 600
    height: 300
    function open(fileName) {
        renameFileWindow.oldName = fileName;
        renameFileWindow.visible = true;
    }
    TextField {
        id: renameInput
        width: 400
        anchors.centerIn: parent
        text: renameFileWindow.oldName
        color: "black"
    }
    Button {
        id: remameSave
        anchors {
            top: renameInput.bottom
            topMargin: 20
            horizontalCenter: renameInput.horizontalCenter
        }
        text: "SAVE"
        onClicked: {
            renameFileWindow.renamed(renameFileWindow.oldName, renameInput.text)
            renameFileWindow.close();
        }
    }
}
