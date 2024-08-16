import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import JUCETest 1.0

Window {
    id: root
    width: 640
    height: 480
    visible: true
    title: qsTr("QJUCE")

    property alias playing: controller.playing

    signal play(string file)
    signal stop()

    Controller {
        id: controller
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#4d4d4d"
        anchors.centerIn: parent
    }

    ColumnLayout {
        id: mainLayout
        anchors.centerIn: parent
        spacing: 10

        Label {
            id: fileLabel
            Layout.alignment: Qt.AlignCenter
            text: "No file selected"
        }

        Button {
            id: fileButton
            Layout.alignment: Qt.AlignCenter
            text: "Select File"
            onClicked: () => fileDialog.open()
        }

        Button {
            id: playButton
            Layout.alignment: Qt.AlignCenter
            text: root.playing ? "Stop" : "Play"
            enabled: fileLabel.text !== "No file selected"
            onClicked: root.playing ? root.stop() : root.play(fileLabel.text)
        }
    }

    FileDialog {
        id: fileDialog
        title: "Select a file"
        currentFolder: StandardPaths.writableLocation(StandardPaths.MusicLocation)
        onAccepted: {
            fileLabel.text = fileDialog.selectedFile.toString().replace("file://", "")
        }
    }

    Connections {
        target: root
        function onPlay(file) {
            controller.onPlay(file)
            playButton.text = "Stop"
        }
    }

    Connections {
        target: root
        function onStop() {
            controller.onStop()
        }
    }

    Connections {
        target: controller
        function onStopped() {
            playButton.text = "Play"
        }
    }
}
