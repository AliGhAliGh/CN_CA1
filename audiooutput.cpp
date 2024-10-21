#include "audiooutput.h"
#include <QDebug>

AudioOutput::AudioOutput(const QAudioFormat &format, QObject *parent)
    : QObject(parent), audioFormat(format) {
    audioSink = new QAudioSink(audioFormat, this); //QAudioSink
}


AudioOutput::~AudioOutput() {
    stop();
}

//
void AudioOutput::start() {
    audioSink->start();
}


void AudioOutput::stop() {
    audioSink->stop();
}


void AudioOutput::addData(const unsigned char *data, int len) {
    QMutexLocker locker(&mutex);
    // audioSink->write(reinterpret_cast<const char *>(data), len); //QAudioSink
}
