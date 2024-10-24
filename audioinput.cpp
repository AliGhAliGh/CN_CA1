#include "audioinput.h"
#include <QAudioFormat>
#include <QDebug>
#include "webrtc.h"

AudioInput::AudioInput(WebRTC *webRtc, QObject *parent)
    : QIODevice(parent)
    , audioSource(nullptr)
    , opusEncoder(nullptr)
{
    connect(this, &AudioInput::dataReady, webRtc, &WebRTC::dataReady, Qt::AutoConnection);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    format.setSampleFormat(QAudioFormat::Int16);
    audioSource = new QAudioSource(format, this);
    int error;
    opusEncoder = opus_encoder_create(format.sampleRate(),
                                      format.channelCount(),
                                      OPUS_APPLICATION_VOIP,
                                      &error);
    if (error != OPUS_OK)
        qWarning("Failed to create Opus encoder: %s", opus_strerror(error));
}

bool AudioInput::open(OpenMode mode)
{
    QIODevice::open(mode);
    audioSource->start(this);
    return true;
}

void AudioInput::close()
{
    audioSource->stop();
    QIODevice::close();
}

qint64 AudioInput::writeData(const char *data, qint64 len)
{
    qDebug() << "asdffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    unsigned char opusData[4000];

    int encodedBytes = opus_encode(opusEncoder,
                                   reinterpret_cast<const opus_int16 *>(data),
                                   len / 2,
                                   opusData,
                                   sizeof(opusData));

    if (encodedBytes < 0) {
        qWarning("Opus encoding error: %s", opus_strerror(encodedBytes));
        return -1;
    }

    QByteArray byteArray(reinterpret_cast<const char *>(opusData), encodedBytes);
    Q_EMIT dataReady(byteArray);
    return len;
}

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return -1;
}
