#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QThread>
#include "bufferedframe.h"
#include "unbufferedframe.h"
#include <QStringList>

class ImageBuffer : public QThread
{
    Q_OBJECT
public:
    explicit ImageBuffer(QObject *parent = 0);
    void run();

signals:
    void newFrame(BufferedFrame *);
public slots:
    void setFrames(QList<UnBufferedFrame> f);
    void rejectedFrame(BufferedFrame *f);
    void stop(bool s = true);

private:
    QList<UnBufferedFrame> frames;
    bool stopped;
};

#endif // IMAGEBUFFER_H
