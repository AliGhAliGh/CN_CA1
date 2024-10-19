#include "webrtc.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QtEndian>
#include "socket.io-client-cpp/src/sio_client.h"

static_assert(true);

#pragma pack(push, 1)
struct RtpHeader
{
    uint8_t first;
    uint8_t marker : 1;
    uint8_t payloadType : 7;
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint32_t ssrc;
};
#pragma pack(pop)

WebRTC::WebRTC(QObject *parent)
    : QObject{parent}
    , m_audio("Audio")
{
    m_instanceCounter++;
    qDebug() << "WebRTC instance created. Total instances:" << m_instanceCounter;
    connect(this, &WebRTC::gatheringComplited, [this](const QString &peerID) {
        qDebug() << "Gathering completed!";
        auto desc = m_peerConnections[peerID]->localDescription().value();
        m_localDescription = descriptionToJson(desc);
        m_peerSdps.insert(peerID, desc);
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);

        if (isOfferer())
            Q_EMIT this->offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerID, m_localDescription);
    });
}

WebRTC::~WebRTC()
{
    m_instanceCounter--;
    qDebug() << "WebRTC instance destroyed. Remaining instances:" << m_instanceCounter;
}

void WebRTC::startCall(const QString &name)
{
    // init(name, true);
    // addPeer(name);
    // addAudioTrack(name, "recv audio");
    // generateOfferSDP(name);
    // sio::client client;
    // client.connect("http://127.0.0.1:3030");
    // client.socket()->on("connect", [](sio::event event) {
    //     qDebug() << "connect: " << event.get_message()->get_string();
    // });
    // client.socket()->on("disconnect", [](sio::event event) {
    //     qDebug() << "disconnect: " << event.get_message()->get_string();
    // });
    // client.socket()->on("connect_error", [](sio::event event) {
    //     qDebug() << "connect_error" << event.get_message()->get_string();
    // });
    // client.socket()->on("message", [](sio::event event) {
    //     qDebug() << "message" << event.get_message()->get_string();
    // });
    // client.socket()->on("uniquenessError", [](sio::event event) {
    //     qDebug() << "uniquenessError" << event.get_message()->get_string();
    // });
    // client.socket()->emit("ready");
}

void WebRTC::endCall() {}

/**
 * ====================================================
 * ================= public methods ===================
 * ====================================================
 */

void WebRTC::init(const QString &id, bool isOfferer)
{
    rtc::InitLogger(rtc::LogLevel::Debug);
    m_localId = id;
    m_isOfferer = isOfferer;
    m_config = rtc::Configuration();
    m_config.iceServers.emplace_back("stun.l.google.com:19302");
    qDebug() << "WebRTC initialized for id:" << m_localId << "as offerer:" << isOfferer;
    // m_config.iceServers.emplace_back("turn:turnserver.example.com:3478", "username", "password");

    // Initialize WebRTC using libdatachannel library
    // Create an instance of rtc::Configuration to Set up ICE configuration
    // Add a STUN server to help peers find their public IP addresses
    // Add a TURN server for relaying media if a direct connection can't be established
    // Set up the audio stream configuration
}

void WebRTC::addPeer(const QString &peerId)
{
    // Create and add a new peer connection
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);

    // Set up a callback for when the local description is generated
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        // The local description should be Q_EMITted using the appropriate signals based on the peer's role (offerer or answerer)
        QString jsonDesc = descriptionToJson(description);
        Q_EMIT localDescriptionGenerated(peerId, jsonDesc);
        // m_localDescription = QString::fromStdString(std::string(description));
        // Q_EMIT localDescriptionGenerated(peerId, m_localDescription);
    });

    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        Q_EMIT localCandidateGenerated(peerId,
                                       QString::fromStdString(candidate.candidate()),
                                       QString::fromStdString(candidate.mid()));
        // Q_EMIT localCandidateGenerated(peerId, candidate, m_localDescription);
        // Emit the local candidates using the localCandidateGenerated signal
    });

    // Set up a callback for when the state of the peer connection changes
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        qDebug() << "PeerConnection state changed:" << static_cast<int>(state);
        // Handle different states like New, Connecting, Connected, Disconnected, etc.
    });

    // Set up a callback for monitoring the gathering state
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        // When the gathering is complete, Q_EMIT the gatheringComplited signal
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            qDebug() << "ICE Gathering complete for peer:" << peerId;
            Q_EMIT gatheringComplited(peerId);
        }
    });

    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        // handle the incoming media stream, Q_EMITting the incommingPacket signal if a stream is received
        qDebug() << "Track received from peer:" << peerId;

        // Listen for media packets on the incoming track
        track->onMessage([this, peerId](rtc::message_variant message) {
            auto data = readVariant(message);
            Q_EMIT incommingPacket(peerId, data, data.size());
        });

        qDebug() << "Listening for track messages from peer:" << peerId;
    });

    // newPeer->addTrack();
    // Add an audio track to the peer connection
    m_peerConnections[peerId] = newPeer;
    qDebug() << "Peer added with id:" << peerId;
}

// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
        qDebug() << "Offer SDP generated for peer:" << peerId;
    }
}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
        qDebug() << "Answer SDP generated for peer:" << peerId;
    }
}

// if (m_peerConnections.contains(peerId)) {
//     auto track = std::make_shared<rtc::Track>(rtc::Track::Audio);
//     m_peerConnections[peerId]->addTrack(track);
//     m_peerTracks[peerId] = track;
//     qDebug() << "Audio track added to peer:" << peerId << "Track name:" << trackName;
// }

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];

        rtc::Description::Audio media(trackName.toStdString(),
                                      rtc::Description::Direction::RecvOnly);

        // Add the track to the peer connection
        auto track = peerConnection->addTrack(media);

        m_peerTracks[peerId] = track;

        qDebug() << "Audio track added to peer:" << peerId << "Track name:" << trackName;

        // Handle track events: receiving messages
        track->onMessage([this, peerId](rtc::message_variant message) {
            auto data = readVariant(message);
            Q_EMIT incommingPacket(peerId, data, data.size());
            qDebug() << "Audio track message received from peer:" << peerId
                     << "of size:" << data.size();
        });

        // Handle track frame events (for example, when receiving audio frames)
        track->onFrame([this](rtc::binary frame, rtc::FrameInfo info) {
            qDebug() << "Audio track frame received of size:" << frame.size()
                     << "Timestamp:" << info.timestamp;
            // Here you could process the frame data or forward it to an audio pipeline
        });
    } else {
        qDebug() << "Peer connection not found for peerId:" << peerId;
    }
}

// Sends audio track data to the peer

void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    if (m_peerTracks.contains(peerId)) {
        auto track = m_peerTracks[peerId];
        if (track && track->isOpen()) {
            // track->send(reinterpret_cast<const uint8_t *>(buffer.constData()), buffer.size());
            qDebug() << "Audio track data sent to peer:" << peerId;
        } else {
            qDebug() << "Track is not open or invalid for peer:" << peerId;
        }
    }

    // Create the RTP header and initialize an RtpHeader struct

    // Create the RTP packet by appending the RTP header and the payload buffer

    // Send the packet, catch and handle any errors that occur during sending
}

/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    if (m_peerConnections.contains(peerID)) {
        rtc::Description desc(sdp.toStdString(),
                              isOfferer() ? rtc::Description::Type::Answer
                                          : rtc::Description::Type::Offer);
        m_peerConnections[peerID]->setRemoteDescription(desc);
        m_peerSdps.insert(peerID, desc);
        qDebug() << "Remote SDP set for peer:" << peerID;
    }
}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID,
                                const QString &candidate,
                                const QString &sdpMid)
{
    if (m_peerConnections.contains(peerID)) {
        rtc::Candidate cand(candidate.toStdString(), sdpMid.toStdString());
        m_peerConnections[peerID]->addRemoteCandidate(cand);
        qDebug() << "Remote ICE candidate added for peer:" << peerID;
    }
}

/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    if (std::holds_alternative<std::string>(data)) {
        return QByteArray::fromStdString(std::get<std::string>(data));
    }
    return QByteArray();
}

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description)
{
    QJsonObject jsonObject;
    jsonObject["sdp"] = QString::fromStdString(static_cast<std::string>(description));
    jsonObject["type"] = (description.type() == rtc::Description::Type::Offer) ? "offer" : "answer";
    QJsonDocument jsonDoc(jsonObject);
    return jsonDoc.toJson(QJsonDocument::Compact);
}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{
    return m_bitRate;
}

// Set a new bit rate and Q_EMIT the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate != newBitRate) {
        m_bitRate = newBitRate;
        Q_EMIT bitRateChanged();
    }
}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
    Q_EMIT bitRateChanged();
}

// Sets a new payload type and Q_EMIT the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType != newPayloadType) {
        m_payloadType = newPayloadType;
        Q_EMIT payloadTypeChanged();
    }
}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
    Q_EMIT payloadTypeChanged();
}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

// Set a new SSRC and Q_EMIT the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc != newSsrc) {
        m_ssrc = newSsrc;
        Q_EMIT ssrcChanged();
    }
}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{
    m_ssrc = 2;
    Q_EMIT ssrcChanged();
}

// Retrieve the current payload type
int WebRTC::payloadType() const
{
    return m_payloadType;
}

/**
 * ====================================================
 * ================= getters setters ==================
 * ====================================================
 */

bool WebRTC::isOfferer() const
{
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer)
{
    if (m_isOfferer != newIsOfferer) {
        m_isOfferer = newIsOfferer;
        Q_EMIT isOffererChanged();
    }
}

void WebRTC::resetIsOfferer()
{
    m_isOfferer = false;
    Q_EMIT isOffererChanged();
}
