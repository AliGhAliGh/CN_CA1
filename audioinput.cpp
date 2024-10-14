#include "audioinput.h"
#include <QAudioFormat>
#include <QDebug>


AudioInput::AudioInput(const QAudioFormat &format, QObject *parent)
    : QIODevice(parent), audioSource(nullptr), opusEncoder(nullptr) {


    audioSource = new QAudioSource(format, this);


    int error;
    opusEncoder = opus_encoder_create(format.sampleRate(), format.channelCount(), OPUS_APPLICATION_VOIP, &error);

    if (error != OPUS_OK) {
        qWarning("Failed to create Opus encoder: %s", opus_strerror(error));
    }
}

bool AudioInput::open(OpenMode mode) {
    QIODevice::open(mode);
    audioSource->start(this);
    return true;
}


void AudioInput::close() {
    audioSource->stop();
    QIODevice::close();
}


qint64 AudioInput::writeData(const char *data, qint64 len) {
    //storing compressed data
    unsigned char opusData[4000];


    int encodedBytes = opus_encode(opusEncoder, reinterpret_cast<const opus_int16*>(data), len / 2, opusData, sizeof(opusData));

    if (encodedBytes < 0) {
        qWarning("Opus encoding error: %s", opus_strerror(encodedBytes));
        return -1;
    }

    // webRTC ??

    return len;
}


qint64 AudioInput::readData(char *data, qint64 maxlen) {
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return -1;
}
