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
    void setBufferEnabled(bool b);
    void setBufferMaxSize(int m);
    void setBufferAhead(int b);
    void setSkipFrames(bool s);
signals:
    void frameChanged(int frame);
    void stateChanged(QMediaPlayer::State);
    void mediaStatusChanged(QMediaPlayer::MediaStatus);
    void bufferState(int);
    void durationChanged(int);
    void frameTime(qint64 ms);
    void removedFrameFromBuffer(int f);
    void adddedFrameToBuffer(int f);
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
    void setLoop(bool l);
private:
    qreal frameRate;
    int currentFrame;
    int bufferMaxSize;
    int bufferAhead;
    bool skipFrames;
    bool bufferEnabled;
    int currentMaxSize;
    int bufferSize;
    bool forward;
    bool paused;
    bool playing;
    bool stopped;
    bool progra;
    bool loop;
    QTimer *displayTimer;
    QTimer *reverseTimer;
    QList<UnBufferedFrame> frames;
    QList<BufferedFrame *> buffer;
    QList<bool> isBuffered;
    ImageBuffer *imageBuffer;
    QElapsedTimer frameTimer;
    QElapsedTimer seekTimer;
    void emptyBuffer();
    void insertBufferedFrame(BufferedFrame *f);
    bool removeFirstBufferedFrame();
    bool removeLastBufferedFrame();
    void displayFrame(int f);
    void requestBuffering();
    QSize playerSize;

};
#endif // ISPLAYER_H
