#include "isplayer.h"
ISPlayer::ISPlayer(QWidget *parent) :
    QLabel(parent)
{
    progra = true;
    frameRate = 25;
    frameTimer.start();
    currentFrame = 0;
    displayTimer = new QTimer();
    reverseTimer = new QTimer();
    imageBuffer = new ImageBuffer();
    forward = true;
    paused = false;
    playing = false;
    stopped = false;
    connect(displayTimer,&QTimer::timeout,this,&ISPlayer::nextFrame);
    connect(reverseTimer,&QTimer::timeout,this,&ISPlayer::previousFrame);
    connect(imageBuffer,&ImageBuffer::newFrame,this,&ISPlayer::newBufferedFrame);
    bufferMaxSize = 400;
    bufferAhead = 30;
    bufferEnabled = true;
    skipFrames = false;
    progra = false;
    loop = false;
    this->setScaledContents(true);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void ISPlayer::setBufferEnabled(bool b)
{
    bufferEnabled = b;
}

void ISPlayer::setBufferMaxSize(int m)
{
    bufferMaxSize = m;
}

void ISPlayer::setBufferAhead(int b)
{
    bufferAhead = b;
}

void ISPlayer::setSkipFrames(bool s)
{
    skipFrames = s;
}

void ISPlayer::newBufferedFrame(BufferedFrame *f)
{
    int frameNumber = f->getFrameNumber();
    //if the buffer is full and the frame is new, make some room
    if (!isBuffered[frameNumber] && bufferSize >= currentMaxSize)
    {
        if (forward)
        {
            if (!removeFirstBufferedFrame()) removeLastBufferedFrame();
        }
        else if (!removeLastBufferedFrame()) removeFirstBufferedFrame();
    }
    //ajouter la frame si on a la place
    if (bufferSize < currentMaxSize && !isBuffered[frameNumber])
    {
        insertBufferedFrame(f);
    }
    else
    {
        delete f;
    }
    emit bufferState(bufferSize*100/currentMaxSize);
}

void ISPlayer::emptyBuffer()
{
    //empty buffer
    int i = -1;
    while(buffer.count() > 0)
    {
        BufferedFrame *f = buffer.takeFirst();
        emit removedFrameFromBuffer(i++);
        delete f;
    }
    isBuffered.clear();
    if (frames.count() > 0)
    {
        for(int i = 0 ; i < frames.count() ; i++)
        {
            buffer << new BufferedFrame("",-1);
            isBuffered << false;
        }
    }
    bufferSize = 0;
    emit bufferState(0);
}

void ISPlayer::insertBufferedFrame(BufferedFrame *f)
{
    int frameNumber = f->getFrameNumber();
    //virer l'ancienne
    BufferedFrame *rf = buffer.takeAt(frameNumber);
    delete rf;
    //mettre la nouvelle
    buffer.insert(frameNumber,f);
    isBuffered[frameNumber] = true;
    bufferSize++;
    emit adddedFrameToBuffer(frameNumber);
    /*//si c'est la currentframe on en profite pour l'afficher
if (frameNumber == currentFrame)
{
displayFrame(currentFrame);
}*/
}

bool ISPlayer::removeFirstBufferedFrame()
{
    //deletes the first frame
    for (int i = 0;i<currentFrame;i++)
    {
        if (isBuffered[i])
        {
            BufferedFrame *f = buffer.takeAt(i);
            delete f;
            buffer.insert(i,new BufferedFrame("",-1));
            bufferSize--;
            isBuffered[i] = false;
            emit removedFrameFromBuffer(i);
            return true;
        }
    }
    return false;
}

bool ISPlayer::removeLastBufferedFrame()
{
    //deletes the last frame
    for (int i = buffer.count()-1;i>currentFrame;i--)
    {
        if (isBuffered[i])
        {
            BufferedFrame *f = buffer.takeAt(i);
            delete f;
            buffer.insert(i,new BufferedFrame("",-1));
            bufferSize--;
            isBuffered[i] = false;
            emit removedFrameFromBuffer(i);
            break;
            return true;
        }
    }
    return false;
}

void ISPlayer::displayFrame(int f)
{
    //par sécu, on vérifie qu'elle est dans les frames
    if (f < 0 || f > frames.count()-1) return ;
    bool surePlay = playing;
    //faire poireauter la durée d'une image si on est en lecture
    if (playing)
    {
        while (!frameTimer.hasExpired(1000/frameRate))
            QCoreApplication::processEvents(QEventLoop::AllEvents, 1000/frameRate);
    }
    //si entre deux on a cliqué sur stop
    if (stopped && surePlay) return;
    //si entre deux on a cliqué sur pause
    if (paused && surePlay) return;
    emit frameTime(frameTimer.elapsed());
    frameTimer.start();
    currentFrame = f;
    //si elle est buffered, on la prend dans le buffer
    if (isBuffered[f])
    {
        QPixmap pix = buffer[f]->getPixmap();
        this->setPixmap(pix);
    }
    //sinon on l'affiche
    else if (!skipFrames)
    {
        //on l'ajoute au buffer
        BufferedFrame *pix = new BufferedFrame(frames[f].getFilePath(),f);
        newBufferedFrame(pix);
        emit adddedFrameToBuffer(f);
        this->setPixmap(pix->getPixmap());
    }
    this->repaint();
    emit frameChanged(currentFrame);
}

void ISPlayer::requestBuffering()
{
    QList<UnBufferedFrame> requestFrames;
    //laisser le temps au buffer de se stopper avant de le redémarrer
    QElapsedTimer timer;
    timer.start();
    imageBuffer->stop();
    if (forward)
    {
        for (int i = currentFrame;i<frames.count();i++)
        {
            if (!isBuffered[i])
            {
                requestFrames << frames[i];
            }
        }
    }
    else
    {
        for (int i = currentFrame;i>=0;i--)
        {
            if (!isBuffered[i])
            {
                requestFrames << frames[i];
            }
        }
    }
    while(!timer.hasExpired(100))
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    imageBuffer->setFrames(requestFrames);
    imageBuffer->start();
}

void ISPlayer::setFrames(QStringList f)
{
    frames.clear();
    for(int i = 0;i<f.count();i++)
    {
        UnBufferedFrame uf(f[i],i);
        frames << uf;
    }
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
    //se remettre au début
    displayFrame(0);
}

void ISPlayer::setFrameRate(qreal fr)
{
    frameRate = fr;
}

void ISPlayer::play(bool ignoreEmptyBuffer)
{
    //vérifs de sécurité
    if (frames.count() ==0) stop();
    if (currentFrame < 0)
    {
        if (stopped) stop();
        if (paused) pause();
    }
    if (currentFrame >= frames.count()-1)
    {
        currentFrame = 0;
    }
    //si déjà en lecture dans le bon sens
    if (playing && forward) return;
    //si en lecture dans le mauvais sens, pause
    if (playing && !forward) pause();
    //on passe dans le bon sens
    forward = true;
    paused = false;
    playing = true;
    stopped = false;
    //launch buffering
    if (bufferEnabled)
    {
        requestBuffering();
        //wait for the buffer to fill //TODO ignorer si on buffer pas
        if(!ignoreEmptyBuffer)
        {
            emit mediaStatusChanged(QMediaPlayer::BufferingMedia);
            bool bufferReadyToPlay = false;
            while(!bufferReadyToPlay)
            {
                QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                int last = currentFrame + bufferAhead;
                if (last >= frames.count()) last = frames.count()-1;
                bufferReadyToPlay = true;
                for (int i = currentFrame;i<=last;i++)
                {
                    if (!isBuffered[i])
                    {
                        bufferReadyToPlay = false;
                        break;
                    }
                }
            }
            emit mediaStatusChanged(QMediaPlayer::LoadedMedia);
        }
    }
    //launch timer pour lecture
    displayTimer->start(0);
    //et le timer de durée de frame
    frameTimer.start();
    emit stateChanged(QMediaPlayer::PlayingState);
}

void ISPlayer::reversePlay()
{
    if (frames.count() ==0)
    {
        stop();
        return;
    }
    if (playing && !forward) return;
    if (playing && forward) pause();
    forward = false;
    paused = false;
    playing = true;
    stopped = false;
    //launch buffering
    if (bufferEnabled) requestBuffering();
    //launch timer
    reverseTimer->start(0);
    paused = false;
    emit stateChanged(QMediaPlayer::PlayingState);
}

void ISPlayer::pause()
{
    displayTimer->stop();
    reverseTimer->stop();
    paused = true;
    stopped = false;
    playing = false;
    imageBuffer->stop();
    emit stateChanged(QMediaPlayer::PausedState);
}

void ISPlayer::stop()
{
    forward = true;
    paused = false;
    playing = false;
    stopped = true;
    displayTimer->stop();
    reverseTimer->stop();
    imageBuffer->stop();
    //wait for everything to be effectively stopped
    QElapsedTimer timer;
    timer.start();
    while(!timer.hasExpired(100))
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    //empty buffer and reinit everything
    emptyBuffer();
    currentFrame = 0;
    setPixmap(QPixmap());
    setText("Stopped");
    this->repaint();
    emit stateChanged(QMediaPlayer::StoppedState);
    emit frameChanged(0);
}

void ISPlayer::setLoop(bool l)
{
    loop = l;
}

void ISPlayer::nextFrame()
{
    if (frames.count() ==0)
    {
        return;
    }
    //if the last frame was already displayed
    if (currentFrame >= frames.count()-1)
    {
        if (loop)
        {
            displayFrame(0);
        }
        else
        {
            displayFrame(frames.count() -1);
            pause();
        }
    }
    else
    {
        displayFrame(currentFrame + 1);
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
    else
    {
        displayFrame(currentFrame -1);
    }
}

void ISPlayer::seek(int f)
{
    if (f < 0 || f > frames.count()-1) return;
    pause();
    displayFrame(f);
}

int ISPlayer::getCurrentFrame()
{
    return currentFrame;
}

