#include "webrtc.h"
#include <QIODevice>
#include <QtEndian>
#include "audioinput.h"
#include "signalmanager.h"

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
    , m_signaller(new SignalManager(this))
, m_audioInput(new AudioInput(this))
{
    m_instanceCounter++;
    qDebug() << "WebRTC instance created. Total instances:" << m_instanceCounter;
    connect(this, &WebRTC::gatheringComplited, [this](const QString &peerID) {
        qDebug() << "Gathering completed!";
        auto desc = m_peerConnections[peerID]->localDescription().value();
        m_localDescription = descriptionToJson(desc);
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

/**
 * ====================================================
 * ================= Invokable methods ===================
 * ====================================================
 */

void WebRTC::startCall(const QString &targetName)
{
    setIsOfferer(true);
    addPeer(targetName);
    addAudioTrack(targetName, "recv audio");
    generateOfferSDP(targetName);
}

void WebRTC::registerName(const QString &username)
{
    init(username);
    addPeer(username);
    m_signaller->connectToSignalingServer(username.toStdString());
}

void WebRTC::endCall(const QString &name)
{
    m_peerConnections[name]->close();
}

void WebRTC::acceptCall(const QString &name)
{
    if (!m_peerSdps.contains(name))
        return;
    rtc::Description desc(m_peerSdps[name].toStdString(), rtc::Description::Type::Offer);
    acceptPeer(desc, name);
}

void WebRTC::rejectCall(const QString &name)
{
    Q_EMIT callRejected(name);
}

/**
 * ====================================================
 * ================= public methods ===================
 * ====================================================
 */

void WebRTC::init(const QString &id)
{
    rtc::InitLogger(rtc::LogLevel::Warning);
    m_localId = id;
    m_isOfferer = false;
    m_config.iceServers.emplace_back("stun.l.google.com:19302");
    qDebug() << "WebRTC initialized for id:" << m_localId;
    // m_config.iceServers.emplace_back("turn:turnserver.example.com:3478", "username", "password");
    // Add a TURN server for relaying media if a direct connection can't be established
}

void WebRTC::addPeer(const QString &peerId)
{
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        QString jsonDesc = descriptionToJson(description);
        Q_EMIT localDescriptionGenerated(peerId, jsonDesc);
    });
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        Q_EMIT localCandidateGenerated(peerId,
                                       QString::fromStdString(candidate.candidate()),
                                       QString::fromStdString(candidate.mid()));
    });
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        auto res = static_cast<int>(state);
        qDebug() << "PeerConnection state changed:" << res;
        if (res == 2)
            QMetaObject::invokeMethod(this, "connectionReady", Qt::QueuedConnection);

        if (res == 5)
            endConnection();
    });
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            qDebug() << "ICE Gathering complete for peer:" << peerId;
            Q_EMIT gatheringComplited(peerId);
        }
    });
    m_peerConnections[peerId] = newPeer;
    qDebug() << "Peer added with id:" << peerId;
}

void WebRTC::generateOfferSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
        qDebug() << "Offer SDP generated for peer:" << peerId;
    }
}

void WebRTC::generateAnswerSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
        qDebug() << "Answer SDP generated for peer:" << peerId;
    }
}

void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];
        rtc::Description::Audio media(trackName.toStdString(),
                                      rtc::Description::Direction::RecvOnly);
        auto track = peerConnection->addTrack(media);
        m_peerTrack = track;
        qDebug() << "Audio track added to peer:" << peerId << "Track name:" << trackName;
        track->onMessage([this, peerId](rtc::message_variant message) {
            auto data = readVariant(message);
            Q_EMIT incommingPacket(peerId, data, data.size());
            qDebug() << "Audio track message received from peer:" << peerId
                     << "of size:" << data.size();
        });
        track->onFrame([this](rtc::binary frame, rtc::FrameInfo info) {
            qDebug() << "Audio track frame received of size:" << frame.size()
                     << "Timestamp:" << info.timestamp;
        });
    } else {
        qDebug() << "Peer connection not found for peerId:" << peerId;
    }
}

void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    // if (m_peerTracks.contains(peerId)) {
    //     auto track = m_peerTracks[peerId];
    //     if (track && track->isOpen()) {
    //         qDebug() << "Audio track data sent to peer:" << peerId;
    //     } else {
    //         qDebug() << "Track is not open or invalid for peer:" << peerId;
    //     }
    // }

    // Create the RTP header and initialize an RtpHeader struct
    // Create the RTP packet by appending the RTP header and the payload buffer
    // Send the packet, catch and handle any errors that occur during sending
}

/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

void WebRTC::offerReceived(const QString &peerID, const QString &sdp)
{
    m_peerSdps.insert(peerID, sdp);
    setIsOfferer(false);
    Q_EMIT incommigCall(peerID);
}

void WebRTC::answerReceived(const QString &peerID, const QString &sdp)
{
    m_peerSdps.insert(peerID, sdp);
    rtc::Description desc(sdp.toStdString(), rtc::Description::Type::Answer);
    acceptPeer(desc, peerID);
    Q_EMIT acceptedReceived();
}

void WebRTC::rejectedReceived(const QString &peerID)
{
    qDebug() << "rejected from: " << peerID;
    Q_EMIT rejectReceived();
}

void WebRTC::setRemoteCandidate(const QString &peerID,
                                const QString &candidate,
                                const QString &sdpMid)
{
    if (!m_peerConnections.contains(peerID))
        addPeer(peerID);
    rtc::Candidate cand(candidate.toStdString(), sdpMid.toStdString());
    m_peerConnections[peerID]->addRemoteCandidate(cand);
    qDebug() << "Remote ICE candidate added for peer:" << peerID;
}

void WebRTC::dataReady(const QByteArray &data)
{
    qDebug() << "send";
    m_peerTrack->send(reinterpret_cast<const std::byte *>(data.data()),
                      data.size() / sizeof(std::byte));
}

/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

void WebRTC::acceptPeer(const rtc::Description &desc, const QString &peerId)
{
    if (!m_peerConnections.contains(peerId) || !isOfferer())
        addPeer(peerId);
    m_peerConnections[peerId]->setRemoteDescription(desc);
    qDebug() << "Remote SDP set for peer:" << peerId;
}

void WebRTC::endConnection()
{
    m_peerSdps.clear();
    Q_EMIT endReceived();
}

void WebRTC::connectionReady()
{
    m_audioInput->open(QIODevice::ReadOnly);
}

QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    if (std::holds_alternative<std::string>(data)) {
        return QByteArray::fromStdString(std::get<std::string>(data));
    }
    return QByteArray();
}

QString WebRTC::descriptionToJson(const rtc::Description &description)
{
    return QString::fromStdString(static_cast<std::string>(description));
}

int WebRTC::bitRate() const
{
    return m_bitRate;
}

void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate != newBitRate) {
        m_bitRate = newBitRate;
        Q_EMIT bitRateChanged();
    }
}

void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
    Q_EMIT bitRateChanged();
}

void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType != newPayloadType) {
        m_payloadType = newPayloadType;
        Q_EMIT payloadTypeChanged();
    }
}

void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
    Q_EMIT payloadTypeChanged();
}

rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc != newSsrc) {
        m_ssrc = newSsrc;
        Q_EMIT ssrcChanged();
    }
}

void WebRTC::resetSsrc()
{
    m_ssrc = 2;
    Q_EMIT ssrcChanged();
}

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
