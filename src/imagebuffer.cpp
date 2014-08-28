#include "imagebuffer.h"

ImageBuffer::ImageBuffer(QObject *parent) :
    QThread(parent)
{
    stopped = false;
}

void ImageBuffer::setFrames(QList<UnBufferedFrame> f)
{
    frames = f;
}


void ImageBuffer::run()
{
    stopped = false;

    while (frames.count()>0)
    {
        if (stopped)
        {
            frames.clear();
            return;
        }
        BufferedFrame *frame = new BufferedFrame(frames[0].getFilePath(),frames[0].getFrameNumber());
        emit newFrame(frame);
        frames.removeFirst();
    }
}


void ImageBuffer::rejectedFrame(BufferedFrame *f)
{
    UnBufferedFrame uf(f->getFilePath(),f->getFrameNumber());
    frames.prepend(uf);
    delete f;
}

void ImageBuffer::stop(bool s)
{
    stopped = s;
}

