#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QAudioSink>
#include <QAudioFormat>
#include <QMutex>

class AudioOutput : public QObject {
    Q_OBJECT

public:
    AudioOutput(const QAudioFormat &format, QObject *parent = nullptr);
    ~AudioOutput();

    void start();
    void stop();
    void addData(const unsigned char *data, int len);

private:
    QAudioSink *audioSink;
    QAudioFormat audioFormat;
    QMutex mutex;
};

#endif
