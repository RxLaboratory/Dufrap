#include "isplayer.h"
#include <QtDebug>


ISPlayer::ISPlayer(QWidget *parent) :
    QLabel(parent)
{
    progra = true;

    frameRate = 25;
    currentFrame = 0;
    displayTimer = new QTimer();
    reverseTimer = new QTimer();
    imageBuffer = new ImageBuffer();

    forward = true;
    paused = true;

    connect(displayTimer,&QTimer::timeout,this,&ISPlayer::nextFrame);
    connect(reverseTimer,&QTimer::timeout,this,&ISPlayer::previousFrame);
    connect(imageBuffer,&ImageBuffer::newFrame,this,&ISPlayer::newBufferedFrame);

    bufferMaxSize = 1000;
    bufferAhead = 150;

    progra = false;
}

void ISPlayer::newBufferedFrame(BufferedFrame *f)
{
    qDebug() << "buffered frame " + QString::number(f->getFrameNumber());
    if (forward)
    {
        //add buffered frame
        buffer << *f;
        //check if the buffer is full, then remove one frame at start
        int last = buffer.last().getFrameNumber();
        int first = buffer.first().getFrameNumber();
        if (buffer.count() >= currentMaxSize && last < frames.count()-1 && first < currentFrame)
        {
            buffer.removeFirst();
        }
        else //if the buffered has arrived at the end
        {
            imageBuffer->quit();
        }
    }
    else
    {
        buffer.prepend(*f);
        int first = buffer.first().getFrameNumber();
        int last = buffer.last().getFrameNumber();
        if (buffer.count() >= currentMaxSize && first > 0 && last > currentFrame)
        {
            buffer.removeLast();
        }
        else
        {
            imageBuffer->quit();
        }
    }

    bufferState(buffer.count()*100/currentMaxSize);

}

void ISPlayer::emptyBuffer()
{
    //empty buffer
    if (buffer.count() == 0) return;
    buffer.clear();
    isBuffered.clear();
    for (int i = 0;i<frames.count();i++)
    {
        isBuffered << false;
    }
    bufferState(0);
}

void ISPlayer::setFrames(QStringList f)
{
    frames = f;
    emptyBuffer();
    //bufferSize
    if (bufferMaxSize > frames.count())
    {
        currentMaxSize = frames.count();
    }
    else
    {
        currentMaxSize = bufferMaxSize;
    }
    emit durationChanged(frames.count()-1);
}

void ISPlayer::setFrameRate(qreal fr)
{
    frameRate = fr;
}

void ISPlayer::play(bool ignoreEmptyBuffer)
{
    if (frames.count() ==0)
    {
        stop();
        return;
    }

    pause();
    forward = true;

    //launch buffering
    if (!checkFrameBuffer(currentFrame))
    {
        imageBuffer->quit();
        emptyBuffer();
        imageBuffer->setStartFrame(currentFrame);
        imageBuffer->setForward(true);
        imageBuffer->start();
    }

    //wait for the buffer to fill
    if(!ignoreEmptyBuffer)
    {
        emit mediaStatusChanged(QMediaPlayer::BufferingMedia);
        while( buffer.count() <= bufferAhead && currentFrame + bufferAhead <= frames.count()-currentFrame)
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
        emit mediaStatusChanged(QMediaPlayer::LoadedMedia);
    }

    //launch timer
    displayTimer->start(0);
    //et le timer de durée de frame
    frameTimer.start();
    paused = false;
    emit stateChanged(QMediaPlayer::PlayingState);
}

void ISPlayer::reversePlay()
{
    if (frames.count() ==0)
    {
        stop();
        return;
    }

    pause();
    forward = false;

    //launch buffering
    imageBuffer->quit();
    imageBuffer->setStartFrame(currentFrame);
    imageBuffer->setForward(false);
    imageBuffer->start();

    //launch timer
    reverseTimer->start(1000/frameRate);
    paused  = false;
    emit stateChanged(QMediaPlayer::PlayingState);
}

void ISPlayer::pause()
{
    displayTimer->stop();
    reverseTimer->stop();
    paused = true;
    emit stateChanged(QMediaPlayer::PausedState);
}

void ISPlayer::stop()
{
    displayTimer->stop();
    reverseTimer->stop();
    //stop buffering
    imageBuffer->quit();
    emptyBuffer();
    currentFrame = 0;
    forward = true;
    paused = true;
    emit stateChanged(QMediaPlayer::StoppedState);
    emit frameChanged(0);
}

void ISPlayer::nextFrame()
{
    if (frames.count() ==0)
    {
        stop();
        return;
    }
    //if the last frame was already displayed
    if (currentFrame >= frames.count()-1)
    {
        pause();
        currentFrame = frames.count()-1;
    }
    else if (buffer.count() > 0)
    {
        displayFrame(currentFrame +1 );
    }
    else
    {
        emit mediaStatusChanged(QMediaPlayer::BufferingMedia);
    }
}

void ISPlayer::previousFrame()
{
    if (frames.count() ==0)
    {
        stop();
        return;
    }
    //if the first frame was already displayed
    if (currentFrame <= 0)
    {
        pause();
        currentFrame = 0;
    }
    else if (buffer.count() > 0)
    {
        displayFrame(currentFrame -1);
    }
    else
    {
        emit mediaStatusChanged(QMediaPlayer::BufferingMedia);
    }
}

void ISPlayer::seek(int f)
{
    if (f < 0 || f >= frames.count()) return;

    displayFrame(f);
}

void ISPlayer::displayFrame(int f)
{
    if (f < 0 || f >= frames.count()) return;
    if (f==currentFrame) return;

    //chercher la frame dans le buffer
    if (buffer.count() > 0)
    {
        bool found = false;
        foreach(BufferedFrame *bf,buffer)
        {
            if (bf->getFrameNumber() == f)
            {
                //faire poireauter la durée d'une image
                if (!paused) while (!frameTimer.hasExpired(1000/frameRate))
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 1000/frameRate);
                emit frameTime(frameTimer.elapsed());
                frameTimer.start();
                this->setPixmap(bf->getPixmap());
                this->repaint();
                currentFrame = f;
                emit frameChanged(currentFrame);
                return;
            }
        }
        if (!found)
        {
            imageBuffer->quit();
            emptyBuffer();
            imageBuffer->setStartFrame(f);
            imageBuffer->start();
            displayFrame(f);
        }
    }
}

int ISPlayer::getCurrentFrame()
{
    return currentFrame;
}

void ISPlayer::requestBuffering()
{
    foreach
}
