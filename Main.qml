import QtQuick
import QtQuick.Controls.Material
import QtQuick.Layouts
import WebRtc

Window {
    width: 280
    height: 520
    visible: true
    title: qsTr("Voice Call")

    WebRtc{
        id: backend
        onGatheringComplited: (name)=> {
                                  if(name === nameField.text)
                                  callbtn.connected()
                              }
    }

    Item{
        anchors.fill: parent

        ColumnLayout {
            anchors{
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: textfield.top
                margins: 20
            }

            // Label{
            //     text: "Ip: " + "172.16.142.176"
            //     Layout.fillWidth: true
            //     Layout.preferredHeight: 40
            // }
            // Label{
            //     text: "IceCandidate: " + "172.16.142.176"
            //     Layout.fillWidth: true
            //     Layout.preferredHeight: 40
            // }
            // Label{
            //     text: "CallerId: " + textfield.text
            //     Layout.fillWidth: true
            //     Layout.preferredHeight: 40
            // }

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
                InputMethod.hide();
                focus = false
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
                    textfield.clear()
                    backend.endCall()
                }
            }

            function connected(){
                Material.background = "red"
                text = "End Call"
                enabled=true
            }
        }
    }
}
