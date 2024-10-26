import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia 5.15

ApplicationWindow {
    visible: true
    width: 400
    height: 600
    title: "Audio Settings"

    Column {
        spacing: 10
        anchors.centerIn: parent

        Button {
            id: modeCallButton1
            text: "Call"
            onClicked: inputTest.chooseCallMode1()
        }

        Button {
            id: modeCallButton2
            text: "Wait for Call"
            onClicked: inputTest.chooseCallMode2()
        }

        Rectangle {
            id: canvas
            width: 200
            height: 30
            border.color: "black"
            color: "#ffffff"

            Rectangle {
                id: levelBar
                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                }
                width: canvas.width * inputTest.level
                color: "red"
            }
        }

        TextField {
            id: inputIp
            placeholderText: "IP"
        }

        ComboBox {
            id: deviceBox
            model: inputTest.audioInputDevices
            onActivated: inputTest.deviceChanged(index)
        }

        ComboBox {
            id: deviceBox2
            model: inputTest.audioOutputDevices
            onActivated: inputTest.deviceOutputChanged(index)
        }

        Slider {
            id: volumeSlider
            from: 0
            to: 100
            value: 100
            onValueChanged: inputTest.sliderChanged(value)
        }

        Button {
            id: modeButton
            text: "Unmute Mic"
            onClicked: inputTest.toggleMode()
        }

        Button {
            id: suspendResumeButton
            text: "Suspend recording"
            onClicked: inputTest.toggleSuspend()
        }

        Button {
            id: muteSpeakerButton
            text: "Mute Speaker"
            onClicked: inputTest.toggleSpeaker()
        }
    }
}
