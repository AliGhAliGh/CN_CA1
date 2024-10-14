#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H
#include <QIODevice>
#include <QAudioSource>
// #include <opus/opus.h>
#include <opus.h>

class AudioInput : public QIODevice {
    Q_OBJECT

public:
    AudioInput(const QAudioFormat &format, QObject *parent = nullptr);

    ~AudioInput() {
        if (opusEncoder) {
            opus_encoder_destroy(opusEncoder);
        }
    }

    bool open(OpenMode mode) override;
    void close() override;

    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 maxlen) override;

private:
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;
};


// class AudioInput
// {
// public:
//     AudioInput();
// };

#endif // AUDIOINPUT_H
