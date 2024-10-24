#ifndef AUDIOCAPTURE_H
#define AUDIOCAPTURE_H
#include <qobject.h>

class AudioCapture : QIODevice
{
    Q_OBJECT
public:
    AudioCapture();
};

#endif // AUDIOCAPTURE_H
