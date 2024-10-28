#include "audiooutput.h"
#include <QAudioFormat>
#include <QDebug>
#include <QMediaDevices>
#include "webrtc.h"

AudioOutput::AudioOutput(WebRTC *webRtc, QObject *parent)
    : QObject(parent)
{
    connect(webRtc, &WebRTC::incommingPacket, this, &AudioOutput::receivedData, Qt::AutoConnection);

    QAudioDevice outputDevice = QMediaDevices::defaultAudioOutput();
    audioFormat.setSampleRate(44100);
    audioFormat.setChannelConfig(QAudioFormat::ChannelConfigMono);
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    if (!outputDevice.isFormatSupported(audioFormat)) {
        qWarning() << "فرمت انتخابی پشتیبانی نمی‌شود. فرمت "
                      "پیشنهادی:"
                   << outputDevice.preferredFormat();
        audioFormat = outputDevice.preferredFormat();
    }

    int error;
    opusDecoder = opus_decoder_create(audioFormat.sampleRate(), audioFormat.channelCount(), &error);
    if (error != OPUS_OK) {
        qWarning() << "Failed to create Opus decoder:" << opus_strerror(error);
    }

    audioSink = new QAudioSink(audioFormat, this);
}

AudioOutput::~AudioOutput()
{
    if (opusDecoder)
        opus_decoder_destroy(opusDecoder);
    stop();
}

//
void AudioOutput::start()
{
    if (!device) {
        device = audioSink->start(); // Start once and store the device for continuous playback
        if (!device || !device->isOpen()) {
            qWarning() << "Failed to start audio sink: device is not open.";
        }
    }
}

void AudioOutput::stop()
{
    if (audioSink->state() != QAudio::StoppedState) {
        audioSink->stop();
        device = nullptr; // Reset device handle on stop
    }
}

void AudioOutput::receivedData(const QByteArray &data, qint64 len)
{
    QMutexLocker locker(&mutex);

    if (device && device->isOpen())
        device->write(data.data(), len);
    else
        qWarning() << "Failed to write data to audio sink: device is not open.";
}

// void AudioOutput::receivedData(const QByteArray &data, qint64 len)
// {
//     QMutexLocker locker(&mutex);

//     if (!opusDecoder) {
//         qWarning() << "Opus decoder is not initialized.";
//         return;
//     }

//     int maxFrameSize = 441; // 10 ms frame size at 44100 Hz, mono
//     QVector<opus_int16> pcmBuffer(maxFrameSize * audioFormat.channelCount());

//     int frameCount = opus_decode(opusDecoder,
//                                  reinterpret_cast<const unsigned char *>(data.data()),
//                                  static_cast<opus_int32>(len),
//                                  pcmBuffer.data(),
//                                  maxFrameSize,
//                                  0);

//     if (frameCount < 0) {
//         qWarning() << "Failed to decode Opus data:" << opus_strerror(frameCount);
//         return;
//     }

//     if (device && device->isOpen()) {
//         qint64 pcmSize = frameCount * audioFormat.channelCount() * sizeof(opus_int16);
//         device->write(reinterpret_cast<const char *>(pcmBuffer.data()), pcmSize);
//     } else {
//         qWarning() << "Failed to write data to audio sink: device is not open.";
//     }
// }
