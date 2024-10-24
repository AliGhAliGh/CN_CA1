import QtQuick 2.15
import QtQuick.Controls.Material 2.15
import WebRtc 1.0

Window {
    width: 280
    height: 320
    visible: true
    title: qsTr("Voice Call")
    property string username: ""

    WebRtc {
        id: backend
        onIncommigCall: (name) => {
                            incomingCallPopup.callerName = name;
                            incomingCallPopup.open()
                        }
        onAcceptedReceived: () => callButton.connected()
        onRejectReceived: () => callButton.reset()
        onEndReceived: ()=> callButton.reset()
    }

    Component.onCompleted: {
        namePopup.open()
    }

    Item {
        anchors.fill: parent

        Text {
            id: welcomeText
            text: username.length > 0 ? "Welcome, " + username + "!" : "Enter your name"
            font.pointSize: 24
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10
        }

        TextField{
            id: nameField
            placeholderText: "Enter Name..."
            anchors.bottom: callButton.top
            anchors.bottomMargin: 10
            anchors.left: callButton.left
            anchors.right: callButton.right
            enabled: !callButton.pushed
            onEditingFinished: {
                focus = false
            }
            onFocusChanged: {
                if (focus) {
                    InputMethod.hide()
                }
            }
        }

        Button {
            id: callButton
            property bool pushed: false

            text: "Call"
            Material.background: "green"
            Material.foreground: "white"
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.7
            height: 40
            onClicked: {
                if (!pushed) {
                    pushed = true
                    backend.startCall(nameField.text)
                    text = "Connecting..."
                    enabled = false
                }
                else{
                    backend.endCall(nameField.text)
                    reset()
                }
            }

            function connected() {
                Material.background = "red"
                text = "End Call"
                enabled = true
                pushed = true
            }

            function reset() {
                Material.background = "green"
                text = "Call"
                enabled = true
                pushed = false
                nameField.text = ""
            }
        }
    }

    // Load popups
    RegisterPopup {
        id: namePopup
    }

    IncommingCallPopup {
        id: incomingCallPopup
    }
}
