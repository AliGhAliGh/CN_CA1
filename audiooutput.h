#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QAudioFormat>
#include <QAudioSink>
#include <QAudioSource>
#include <QIODevice>
#include <QMutex>
#include <QObject>
#include <opus.h>

class WebRTC;

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    AudioOutput(WebRTC *webRtc, QObject *parent = nullptr);
    ~AudioOutput();

    void start();
    void stop();

public Q_SLOTS:
    void receivedData(const QByteArray &data, qint64 len);

private:
    QAudioSink *audioSink;
    QAudioFormat audioFormat;
    QMutex mutex;
    OpusDecoder *opusDecoder;
    QIODevice *device;
};

#endif
