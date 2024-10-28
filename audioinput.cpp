#include "audioinput.h"
#include <QAudioFormat>
#include <QDebug>
#include <QMediaDevices>
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

    // opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &error);

    if (error != OPUS_OK)
        qWarning("Failed to create Opus encoder: %s", opus_strerror(error));
}

AudioInput::~AudioInput()
{
    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
    }
}

bool AudioInput::open(OpenMode mode)
{
    QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelConfig(QAudioFormat::ChannelConfigMono);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!inputDevice.isFormatSupported(format)) {
        qWarning() << "فرمت انتخابی پشتیبانی نمی‌شود. فرمت "
                      "پیشنهادی:"
                   << inputDevice.preferredFormat();
        format = inputDevice.preferredFormat();
    }

    audioSource = new QAudioSource(format, this);

    if (!QIODevice::open(mode)) {
        qWarning() << "eror opening AudioInput in WriteOnly";
        return false;
    }

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
    // Emit the raw audio data as-is
    QByteArray byteArray(data, len);
    Q_EMIT dataReady(byteArray);

    return len;
}

// qint64 AudioInput::writeData(const char *data, qint64 len)
// {
//     unsigned char opusData[4000]; // Buffer for encoded data
//     int frameSize = 441;          // 10 ms frame at 44100 Hz, mono

//     int encodedBytes = opus_encode(opusEncoder,
//                                    reinterpret_cast<const opus_int16 *>(data),
//                                    frameSize,
//                                    opusData,
//                                    sizeof(opusData));

//     if (encodedBytes < 0) {
//         qWarning("Opus encoding error: %s", opus_strerror(encodedBytes));
//         return -1;
//     }

//     if (encodedBytes < 0) {
//         qWarning("Opus encoding error: %s", opus_strerror(encodedBytes));
//         return -1;
//     }

//     QByteArray byteArray(reinterpret_cast<const char *>(opusData), encodedBytes);
//     Q_EMIT dataReady(byteArray);

//     return len;
// }

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return -1;
}
