import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Popup {
    id: incomingCallPopup
    width: parent.width * 0.8
    height: parent.height * 0.3
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose

    property string callerName: "Unknown"

    ColumnLayout {
        anchors.fill: parent
        // anchors.margins: 10
        // spacing: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            text: "Incoming Call from"
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: callerName
            font.pixelSize: 18
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter

            Button {
                text: "Accept"
                Material.background: "green"
                onClicked: {
                    backend.acceptCall(callerName)
                    incomingCallPopup.close()
                    nameField.text = callerName
                    callButton.connected()
                }
            }

            Button {
                text: "Reject"
                Material.background: "red"
                onClicked: {
                    backend.rejectCall(callerName)
                    incomingCallPopup.close()
                }
            }
        }
    }

    background: Rectangle {
        color: "lightgray"
        border.color: "gray"
        radius: 10
    }

    onOpened: SequentialAnimation {
        // PropertyAnimation {
        //     target: incomingCallPopup
        //     property: "opacity"
        //     from: 0
        //     to: 1
        //     duration: 300
        // }
        PropertyAnimation {
            target: incomingCallPopup
            property: "scale"
            from: 0.5
            to: 1
            duration: 300
        }
    }

    // Hide popup with a closing animation
    onClosed: SequentialAnimation {
        // PropertyAnimation {
        //     target: incomingCallPopup
        //     property: "opacity"
        //     from: 1
        //     to: 0
        //     duration: 300
        // }
        PropertyAnimation {
            target: incomingCallPopup
            property: "scale"
            from: 1
            to: 0.5
            duration: 300
        }
    }
}
