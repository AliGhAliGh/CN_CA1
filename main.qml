import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import WebRtc

Window {
    width: 280
    height: 320
    visible: true
    title: qsTr("Voice Call")
    property string username: ""

    WebRtc{
        id: backend
        onGatheringComplited: (name)=> {
                                  if(name === nameField.text)
                                  callbtn.connected()
                              }
    }

    Popup {
        id: namePopup
        x: 0
        y: 0
        width: parent.width
        modal: true

        Column {
            spacing: 10
            width: parent.width
            padding: 10

            TextField {
                id: nameInput
                placeholderText: "Enter your name"
                width: parent.width * 0.9
                horizontalAlignment: Text.AlignHCenter
                onEditingFinished: {
                    focus = false
                }
                onAccepted: {
                    namePopup.registerName()
                }
                Component.onCompleted: {
                    forceActiveFocus();
                    selectAll();
                    InputMethod.hide()
                }
                onFocusChanged: {
                    if (focus) {
                        InputMethod.hide()
                    }
                }
            }

            Button {
                text: "Submit"
                onClicked: {
                    namePopup.registerName()
                }
            }
        }

        function registerName(){
            if (nameInput.text.trim() !== "") {
                username = nameInput.text.trim()
                backend.registerName(username)
                namePopup.close()
            } else {
                console.log("Please enter a valid name.")
            }
        }

        background: Rectangle {
            color: "cyan"
            border.color: "gray"
        }
    }

    Component.onCompleted: {
        namePopup.open()
    }

    Item{
        anchors.fill: parent

        Text {
            id: titleText
            text: username.length > 0 ? "Welcome, " + username + "!" : "Enter your name"
            font.pointSize: 24
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 10
        }

        TextField{
            id: nameField
            placeholderText: "Enter Name..."
            anchors.bottom: callbtn.top
            anchors.bottomMargin: 10
            anchors.left: callbtn.left
            anchors.right: callbtn.right
            enabled: !callbtn.pushed
            onEditingFinished: {
                focus = false
            }
            onFocusChanged: {
                if (focus) {
                    InputMethod.hide()
                }
            }
        }

        Button{
            id: callbtn

            property bool pushed: false

            height: 47
            text: "Call"
            Material.background: "green"
            Material.foreground: "white"
            anchors{
                bottom: parent.bottom
                left: parent.left
                right: parent.right
                margins: 20
            }

            onClicked: {
                pushed = !pushed
                if(pushed){
                    text = "Connecting..."
                    enabled = false
                    backend.startCall(nameField.text)
                }
                else{
                    Material.background = "green"
                    text = "Call"
                    nameField.clear()
                    backend.endCall()
                }
            }

            function connected(){
                Material.background = "red"
                text = "End Call"
                enabled=true
            }
        }

        // MouseArea {
        //     anchors.fill: parent
        //     onClicked: {
        //         InputMethod.hide();
        //         nameInput.forceActiveFocus(false);
        //     }
        // }
    }
}
