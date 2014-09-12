#include "mainwindow.h"
#include <QMimeData>
#include <QDropEvent>
#include <QDir>
#include <QTimer>
#include "opener.h"
#include "params.h"
#include "about.h"
#include <QStyleFactory>

MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
    QMainWindow(parent)
{
    progra = true;
    setupUi(this);
    //widgets and players
    iSPlayer = new ISPlayer(this);

    movieLabel = new MovieLabel(this);
    movie = new QMovie(this);
    movieLabel->setMovie(movie);
    seekBar = new JumpingSlider;
    seekBar->setOrientation(Qt::Horizontal);
    seekBar->setStyleSheet("selection-background-color: rgb(255, 68, 68);");
    seekBar->setMaximumHeight(10);
    playControlsLayout->insertWidget(0,seekBar);

    bufferSlider = new BufferSlider(this);
    bufferSlider->setBgColorCSS("background-color: rgb(137, 31, 31);");
    playControlsLayout->insertWidget(1,bufferSlider);

    bufferBar->setStyleSheet("selection-background-color: rgb(137, 31, 31);");
    bufferBar->setStyle(QStyleFactory::create("Fusion"));

    //configuring IS Player
    connect(iSPlayer,&ISPlayer::bufferState,this,&MainWindow::iSBufferStatusChanged);
    connect(iSPlayer,&ISPlayer::durationChanged,this,&MainWindow::iSDurationChanged);
    connect(iSPlayer,&ISPlayer::frameChanged,this,&MainWindow::iSPositionChanged);
    connect(iSPlayer,&ISPlayer::stateChanged,this,&MainWindow::iSStateChanged);
    connect(iSPlayer,&ISPlayer::mediaStatusChanged,this,&MainWindow::iSStatusChanged);
    connect(seekBar,&JumpingSlider::valueChanged,this,&MainWindow::seeked);
    connect(iSPlayer,&ISPlayer::frameTime,this,&MainWindow::evalFramerate);
    connect(iSPlayer,&ISPlayer::adddedFrameToBuffer,bufferSlider,&BufferSlider::setBufferedFrame);
    connect(iSPlayer,&ISPlayer::removedFrameFromBuffer,bufferSlider,&BufferSlider::setUnBufferedFrame);

    iSPlayer->setFrameRate(25);

    //configuring movie player
    connect(movie,&QMovie::frameChanged,this,&MainWindow::iSPositionChanged);
    connect(movie,&QMovie::stateChanged,this,&MainWindow::movieStateChanged);
    iSViewer = new ISViewer(iSPlayer,movieLabel,this);
    mainLayout->insertWidget(0,iSViewer);
    isMovie = false;

    //récupérer les params enregistrés
    QDir::home().mkdir("Dumep");
    QJsonDocument paramsDoc;
    QJsonObject params;
    QFile paramsFile(QDir::homePath() + "/Dumep/params.dufrap");
    if (paramsFile.exists())
    {
        if (paramsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&paramsFile);
            paramsDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
            paramsFile.close();
        }
        params = paramsDoc.object();

        iSPlayer->setBufferMaxSize(params.value("bufferMaxSize").toInt());
        iSPlayer->setBufferEnabled(params.value("bufferEnabled").toBool());
        iSPlayer->setBufferAhead(params.value("bufferAhead").toInt());
        iSPlayer->setSkipFrames(params.value("skipFrames").toBool());
    }

    frameRate = 25;

    progra = false;

    this->setAcceptDrops(true);

    //adding a folder in the home folder for preferences and favs
    QDir::home().mkdir("Dumep");

    buttonStyleAuto = true;

    //charge les préférences
    Params p(this);
    Qt::ToolButtonStyle style = p.getStyle();
    if (style == Qt::ToolButtonFollowStyle) buttonStyleAuto = true;
    else
    {
        buttonStyleAuto = false;
        mainToolBar->setToolButtonStyle(style);
    }
    fpsBox->setCurrentText(QString::number(p.getLastFPS()) + " fps");


    //load arguments
    if (argc > 1)
    {
        QStringList frames = Opener::getSequence(argv[1]);
        if (frames.count() > 0)
        {
            //stoppe
            iSPlayer->stop();
            //ajoute
            addFrames(frames);
        }

    }


    //Ajouter les contrôles dans la TaskBar si win
#ifdef Q_OS_WIN

    //Jumplist
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
    jumplist.frequent()->setVisible(true);
    //progress
    taskbarButton = new QWinTaskbarButton(this);
    taskbarButton->setWindow(windowHandle());

    taskbarProgress = taskbarButton->progress();
    connect(seekBar, SIGNAL(valueChanged(int)), taskbarProgress, SLOT(setValue(int)));
#endif

}


//USEFUL

void MainWindow::addFrames(QStringList f)
{
    iSPlayer->setFrames(f);
}

//BUTTONS

void MainWindow::on_actionOuvrir_triggered()
{
        Opener o(this);
        o.setTitle("Ouvrir");
        if (o.exec())
        {
            //stoppe
            iSPlayer->stop();
            movie->stop();
            //ajoute
            if (o.isMovie())
            {
                iSViewer->setMovie(true);
                fpsBox->hide();
                bufferBar->hide();
                speedBox->show();
                statusLabel->hide();
                speedBox->setValue(1.0);
                isMovie = true;
                movieLabel->setMovieFile(o.getMovie());
                seekBar->setMaximum(movie->frameCount());
                //displaying first frame
                movie->jumpToFrame(0);
            }
            else
            {
                iSViewer->setMovie(false);
                fpsBox->show();
                bufferBar->show();
                speedBox->hide();
                statusLabel->show();
                isMovie = false;
                addFrames(o.getFrames());
                iSViewer->resizePlayer();
            }

        }
}

void MainWindow::on_actionQuitter_triggered()
{
    iSPlayer->stop();
    movie->stop();
    this->close();
}

void MainWindow::on_actionLecture_triggered(bool checked)
{
    if (progra) return;
    if (checked)
    {
        if (isMovie)
        {
            movie->start();
            movie->setPaused(false);
        }
        else
        {
            iSPlayer->play();;
        }
    }
    else
    {
        isMovie ? movie->setPaused(true) : iSPlayer->pause();
    }
}

void MainWindow::on_actionBoucle_triggered(bool checked)
{
    iSPlayer->setLoop(checked);
}

void MainWindow::on_actionStop_triggered()
{
    currentTCLabel->setText("Stoppé");
    if (isMovie)
    {
        movie->stop();
        movie->jumpToFrame(0);
    }
    else
    {
        iSPlayer->stop();
    }

}

void MainWindow::on_actionImage_Suivante_triggered()
{
    if (isMovie)
    {
        movie->setPaused(true);
        if (movie->currentFrameNumber() < movie->frameCount()-1)
        {
            movie->jumpToNextFrame();
        }
    }
    else
    {
        iSPlayer->pause();
        iSPlayer->nextFrame();
    }
}

void MainWindow::on_actionImage_pr_c_dente_triggered()
{ 
    if (isMovie)
    {
        movie->setPaused(true);
        if (movie->currentFrameNumber() > 0)
        {
            movie->jumpToFrame(movie->currentFrameNumber()-1);
        }
    }
    else
    {
        iSPlayer->pause();
        iSPlayer->previousFrame();
    }
}

void MainWindow::on_fpsBox_currentIndexChanged(int index)
{
    if (index == 0)
        frameRate = 8;
    if (index == 1)
        frameRate = 12;
    if (index == 2)
        frameRate = 15;
    if (index == 3)
        frameRate = 23.976;
    if (index == 4)
        frameRate = 24;
    if (index == 5)
        frameRate = 25;
    if (index == 6)
        frameRate = 29.97;
    if (index == 7)
        frameRate = 30;
    if (index == 8)
        frameRate = 50;
    if (index == 9)
        frameRate = 59.94;
    if (index == 10)
        frameRate = 60;

    Params p;
    p.setLastFPS(frameRate);
    iSPlayer->setFrameRate(frameRate);

}

void MainWindow::on_speedBox_valueChanged(double arg1)
{
    movie->setSpeed(arg1*100);
}

void MainWindow::on_zoomButton_currentIndexChanged(int index)
{
    if (index == 0)
    {
        iSViewer->setZoomFactor(0);
    }
    else if (index == 1)
    {
        iSViewer->setZoomFactor(-1);
    }
    else if (index == 2)
    {
        iSViewer->setZoomFactor(2.0);
    }
    else if (index == 3)
    {
        iSViewer->setZoomFactor(1.5);
    }
    else if (index == 4)
    {
        iSViewer->setZoomFactor(1.0);
    }
    else if (index == 5)
    {
        iSViewer->setZoomFactor(0.75);
    }
    else if (index == 6)
    {
        iSViewer->setZoomFactor(0.5);
    }
    else if (index == 7)
    {
        iSViewer->setZoomFactor(0.25);
    }
    else if (index == 8)
    {
        iSViewer->setZoomFactor(0.12);
    }
    else if (index == 9)
    {
        iSViewer->setZoomFactor(0.05);
    }
}

void MainWindow::on_actionParam_tres_triggered()
{
    Params p;

    if (p.exec())
    {

        iSPlayer->setBufferEnabled(p.getBufferEnabled());
        iSPlayer->setBufferMaxSize(p.getBufferMaxSize());
        iSPlayer->setBufferAhead(p.getBufferAhead());
        iSPlayer->setSkipFrames(p.getSkipFrames());

        Qt::ToolButtonStyle style = p.getStyle();

        if (style == Qt::ToolButtonFollowStyle)
        {
            buttonStyleAuto = true;
            if (this->width() > 790) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            else if (this->width() > 540) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            else mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }
        else
        {
            buttonStyleAuto = false;
            mainToolBar->setToolButtonStyle(style);
        }
    }
}

void MainWindow::on_actionA_propos_triggered()
{
    About a(0);
    a.exec();
}

//PLAYER SLOTS

void MainWindow::iSDurationChanged(qint64 duration)
{
    if (duration == 0)
    {
        durationLabel->setText("----");
    }
    else
    {
        durationLabel->setText(QString::number(duration));
    }
    bufferSlider->setNumFrames(duration);
    seekBar->setMaximum(duration);
}

void MainWindow::iSPositionChanged(qint64 position)
{
    progra = true;
    seekBar->setValue(position);
    repaint();
    progra = false;
}

void MainWindow::seeked(int position)
{
        currentTCLabel->setText(QString::number(position));
        repaint();
        if (progra) return;
        if (isMovie)
        {
            movie->jumpToFrame(position);
        }
        else
        {
            iSPlayer->seek(position);
        }
}

void MainWindow::iSStateChanged(QMediaPlayer::State state)
{
    progra = true;
    if (state == QMediaPlayer::PausedState)
    {
        actionLecture->setChecked(false);
        this->setWindowIcon(QIcon(":/icones/pause"));
    }
    else if (state == QMediaPlayer::StoppedState)
    {
        actionLecture->setChecked(false);
        this->setWindowIcon(QIcon(":/icones/stop"));
    }
    else if (state == QMediaPlayer::PlayingState)
    {
        actionLecture->setChecked(true);
        this->setWindowIcon(QIcon(":/icones/play"));
    }
    progra = false;
}

void MainWindow::movieStateChanged(QMovie::MovieState state)
{
    progra = true;
    if (state == QMovie::Paused)
    {
        actionLecture->setChecked(false);
        this->setWindowIcon(QIcon(":/icones/pause"));
    }
    else if (state == QMovie::NotRunning)
    {
        actionLecture->setChecked(false);
        this->setWindowIcon(QIcon(":/icones/stop"));
    }
    else if (state == QMovie::Running)
    {
        actionLecture->setChecked(true);
        this->setWindowIcon(QIcon(":/icones/play"));
    }
    progra = false;
}

void MainWindow::iSStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::UnknownMediaStatus)
    {
        statusLabel->setText("");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::NoMedia)
    {
        statusLabel->setText("Rien à lire");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::LoadingMedia)
    {
        statusLabel->setText("Chargement...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::LoadedMedia)
    {
        statusLabel->setText("----");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::StalledMedia)
    {
        statusLabel->setText("Mise en mémoire tampon...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::BufferingMedia)
    {
        statusLabel->setText("Mise en mémoire tampon...");
        this->setCursor(QCursor(Qt::BusyCursor));
    }
    if (status == QMediaPlayer::BufferedMedia)
    {
        statusLabel->setText("Lecture en streaming");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::EndOfMedia)
    {
        statusLabel->setText("Rien à lire");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    if (status == QMediaPlayer::InvalidMedia)
    {
        statusLabel->setText("Média invalide");
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}

void MainWindow::iSBufferStatusChanged(int percentFilled)
{
      bufferBar->setValue(percentFilled);
}

void MainWindow::evalFramerate(qint64 currentFrameDuration)
{
    if (currentFrameDuration < 5)
    {
        statusLabel->setText("--");
        return;
    }
    //calculer la frameDuration théorique
    qint64 frameDuration = (1000.0/frameRate)+0.5;
    qint64 ecart = frameDuration - currentFrameDuration;
    //on tolère 2ms d'écart par frame
    if (ecart < 2 && ecart > -2)
    {
        statusLabel->setText("Temps Réel");
    }
    else
    {
        statusLabel->setText(QString::number(1000/currentFrameDuration) + " fps");
    }
}

//DRAG AND DROP

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    QStringList frames;

    if (mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        if (urlList.count() == 1)
        {
            QDir dossier(urlList[0].toLocalFile());
            if (dossier.exists())
            {
                foreach(QString f,dossier.entryList(QStringList("*"),QDir::Files))
                {
                    frames << urlList[0].toLocalFile() + "/" + f;
                }
            }
            else
            {
                frames = Opener::getSequence(urlList[0].toLocalFile());
            }
        }

    }
    else if (mimeData->hasText())
    {
        if (QFile(mimeData->text()).exists())
        {
            frames = Opener::getSequence(mimeData->text());
        }
        else if (QDir(mimeData->text()).exists())
        {
            QDir dossier(mimeData->text());
            if (dossier.exists())
            {
                foreach(QString f,dossier.entryList(QStringList("*"),QDir::Files))
                {
                    frames << mimeData->text() + "/" + f;
                }
            }
        }
    }

    if (frames.count() > 0)
    {
        //stoppe
        iSPlayer->stop();
        //ajoute
        addFrames(frames);
    }

    event->acceptProposedAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
     event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

//KEYBOARD

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        if (!event->isAutoRepeat())
        {
            if (isMovie) movie->setPaused(true);
            else iSPlayer->pause();
            on_actionImage_Suivante_triggered();
        }
        else
        {
            if (isMovie) movie->start();
            else iSPlayer->play(true);
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (!event->isAutoRepeat())
        {
            if (isMovie) movie->setPaused(true);
            else iSPlayer->pause();
            on_actionImage_pr_c_dente_triggered();
        }
        else
        {
            if (!isMovie) iSPlayer->reversePlay();
        }
        event->accept();
    }
    else if (event->key() == Qt::Key_Up)
    {
        if (zoomButton->currentIndex() > 0) zoomButton->setCurrentIndex(zoomButton->currentIndex()-1);
    }
    else if (event->key() == Qt::Key_Down)
    {
        if (zoomButton->currentIndex() < 9) zoomButton->setCurrentIndex(zoomButton->currentIndex()+1);
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right)
    {
        if (!event->isAutoRepeat())
        {
            iSPlayer->pause();
        }
    }
    else if (event->key() == Qt::Key_Left)
    {
        if (!event->isAutoRepeat())
        {
            iSPlayer->pause();
        }
    }
    else
    {
        QMainWindow::keyReleaseEvent(event);
    }
}

//RESIZE

void MainWindow::resizeEvent(QResizeEvent* )
{
    if (buttonStyleAuto)
    {
        if (this->width() > 790) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (this->width() > 540) mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else mainToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}


