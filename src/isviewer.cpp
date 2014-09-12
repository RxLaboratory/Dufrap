#include "isviewer.h"


ISViewer::ISViewer(ISPlayer *p,MovieLabel *m,QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    player = p;
    movie = m;
    viewerLayout->addWidget(player);
    viewerLayout->addWidget(movie);
    viewerLayout->setAlignment(player,Qt::AlignCenter);
    viewerLayout->setAlignment(movie,Qt::AlignCenter);
    movie->hide();
    zoomFactor = 0;
    oX = 0;
    oY = 0;
    scrollArea->setFocusPolicy(Qt::NoFocus);
}

void ISViewer::setMovie(bool m)
{
    if (m)
    {
        movie->show();
        player->hide();
    }
    else
    {
        movie->hide();
        player->show();
    }
}

void ISViewer::setZoomFactor(qreal z)
{
    zoomFactor = z;
    if (zoomFactor > 0)
    {
        int iW = player->pixmap()->width();
        int iH = player->pixmap()->height();
        player->setMinimumHeight(iH*zoomFactor);
        player->setMaximumHeight(iH*zoomFactor);
        player->setMinimumWidth(iW*zoomFactor);
        player->setMaximumWidth(iW*zoomFactor);
    }
    else
    {
        resizePlayer();
    }
}

void ISViewer::resizePlayer()
{
    if (!player->pixmap()) return;
    if (player->pixmap()->width() == 0 || player->pixmap()->height() == 0) return;

    int iW = player->pixmap()->width();
    int iH = player->pixmap()->height();
    qreal ratio = (iW + 0.0) / iH;
    int currentW = this->width();
    int currentH = this->height();
    qreal currentRatio = (currentW + 0.0) / currentH;

    if (zoomFactor < 0  || (zoomFactor == 0 && (iW > currentW || iH > currentH)))
    {
        if (ratio>=currentRatio)
        {
            iW = currentW;
            iH = iW/ratio;
        }
        else
        {
            iH = currentH;
            iW = iH*ratio;
        }
        player->setMinimumHeight(iH);
        player->setMaximumHeight(iH);
        player->setMinimumWidth(iW);
        player->setMaximumWidth(iW);
    }
}

void ISViewer::resizeEvent(QResizeEvent*)
{
    resizePlayer();
}

void ISViewer::mousePressEvent(QMouseEvent *event)
{
    oX = event->x();
    oY = event->y();
}

void ISViewer::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - oX;
    int dy = event->y() - oY;
    scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->value()-dy);
    scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value()-dx);
    oX = event->x();
    oY = event->y();
}

