#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H
#include <QAudioSource>
#include <QIODevice>
#include <opus.h>

class WebRTC;

class AudioInput : public QIODevice {
    Q_OBJECT

public:
    AudioInput(WebRTC *webRtc, QObject *parent = nullptr);

    ~AudioInput() {
        if (opusEncoder) {
            opus_encoder_destroy(opusEncoder);
        }
    }

    bool open(OpenMode mode) override;
    void close() override;

    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 maxlen) override;

Q_SIGNALS:
    void dataReady(const QByteArray &data);

private:
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;
};

#endif // AUDIOINPUT_H
