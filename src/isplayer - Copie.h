#ifndef ISPLAYER_H
#define ISPLAYER_H

#include <QLabel>
#include <QTimer>
#include <QMediaPlayer>
#include <QCoreApplication>
#include <QElapsedTimer>
#include "imagebuffer.h"

class ISPlayer : public QLabel
{
    Q_OBJECT
public:
    explicit ISPlayer(QWidget *parent = 0);
    void setFrames(QStringList f);
    int getCurrentFrame();

signals:
    void frameChanged(int frame);
    void stateChanged(QMediaPlayer::State);
    void mediaStatusChanged(QMediaPlayer::MediaStatus);
    void bufferState(int);
    void durationChanged(int);
    void frameTime(qint64 ms);

public slots:
    void play(bool ignoreEmptyBuffer = false);
    void reversePlay();
    void pause();
    void stop();
    void nextFrame();
    void previousFrame();
    void seek(int f);
    void setFrameRate(qreal fr);
    void newBufferedFrame(BufferedFrame *f);

private:
    qreal frameRate;
    int currentFrame;
    int bufferMaxSize;
    int currentMaxSize;
    int bufferAhead;
    bool forward;
    bool paused;
    bool progra;
    QTimer *displayTimer;
    QTimer *reverseTimer;
    QStringList frames;
    QList<bool> isBuffered;
    QList<BufferedFrame> buffer;
    ImageBuffer *imageBuffer;
    QElapsedTimer frameTimer;
    void emptyBuffer();
    void displayFrame(int f);
    void requestBuffering();
};

#endif // ISPLAYER_H
