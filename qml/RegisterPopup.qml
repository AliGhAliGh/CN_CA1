import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15

Popup {
    id: namePopup
    x: 0
    y: 0
    width: parent.width
    modal: true

    ColumnLayout {
        spacing: 10
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            text: "Enter Your Name"
            font.pixelSize: 24
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: nameInput
            placeholderText: "Enter your name"
            Layout.alignment: Qt.AlignHCenter
            Layout.leftMargin: 10
            Layout.rightMargin: 10
            Layout.fillWidth: true

            onAccepted: {
                registerName()
            }
            Component.onCompleted: {
                forceActiveFocus()
                selectAll()
            }
        }

        Button {
            text: "Submit"
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                registerName()
            }
        }
    }

    function registerName() {
        if (nameInput.text.trim() !== "") {
            username = nameInput.text.trim()
            backend.registerName(username)
            namePopup.close()
        } else {
            console.log("Please enter a valid name.")
        }
    }

    background: Rectangle {
        color: "lightblue"
        border.color: "gray"
        radius: 10
    }
}
