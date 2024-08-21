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

        RowLayout {
            id: rowLayout
            spacing: 10
            Layout.alignment: Qt.AlignCenter

            ColumnLayout {
                id: volumeLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: volumeLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Gain"
                }

                Slider {
                    id: volumeSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 4
                    value: controller.volume
                    orientation: Qt.Vertical
                    onValueChanged: controller.volume !== value ? controller.volume = value : null
                }
            }

            ColumnLayout {
                id: reverbLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: reverbLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Room Size"
                }

                Slider {
                    id: roomSizeSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 1
                    value: controller.roomSize
                    onValueChanged: controller.roomSize !== value ? controller.roomSize = value : null
                    orientation: Qt.Vertical
                }
            }

            ColumnLayout {
                id: dampingLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: dampingLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Damping"
                }

                Slider {
                    id: dampingSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 1
                    value: controller.damping
                    onValueChanged: controller.damping !== value ? controller.damping = value : null
                    orientation: Qt.Vertical
                }
            }

            ColumnLayout {
                id: wetLevelLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: wetLevelLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Wet Level"
                }

                Slider {
                    id: wetLevelSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 1
                    value: controller.wetLevel
                    onValueChanged: controller.wetLevel !== value ? controller.wetLevel = value : null
                    orientation: Qt.Vertical
                }
            }

            ColumnLayout {
                id: dryLevelLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: dryLevelLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Dry Level"
                }

                Slider {
                    id: dryLevelSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 1
                    value: controller.dryLevel
                    onValueChanged: controller.dryLevel !== value ? controller.dryLevel = value : null
                    orientation: Qt.Vertical
                }
            }

            ColumnLayout {
                id: widthLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: widthLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Width"
                }

                Slider {
                    id: widthSlider
                    Layout.alignment: Qt.AlignCenter
                    from: 0
                    to: 1
                    value: controller.width
                    onValueChanged: controller.width !== value ? controller.width = value : null
                    orientation: Qt.Vertical
                }
            }

            ColumnLayout {
                id: freezeLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: freezeLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Freeze"
                }

                CheckBox {
                    id: freezeCheckBox
                    Layout.alignment: Qt.AlignCenter
                    checked: controller.freeze >= 0.5
                    onCheckedChanged: () => checked ? controller.freeze = 1.0 : controller.freeze = 0.0
                }
            }

            ColumnLayout {
                id: panLayout
                Layout.alignment: Qt.AlignCenter
                spacing: 10

                Label {
                    id: panLabel
                    Layout.alignment: Qt.AlignCenter
                    text: "Pan"
                }

                Slider {
                    id: panSlider
                    Layout.alignment: Qt.AlignCenter
                    from: -1.0
                    to: 1.0
                    orientation: Qt.Horizontal
                    value: controller.pan
                    onValueChanged: controller.pan !== value ? controller.pan = value : null
                }
            }
        }

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
