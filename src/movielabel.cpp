#include "movielabel.h"
#include <QMovie>
#include <QtDebug>

MovieLabel::MovieLabel(QWidget *parent) :
    QLabel(parent)
{
    zoomFactor = 0;

    this->setScaledContents(false);
    this->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
}

void MovieLabel::setMovieFile(QString f)
{
    this->movie()->setFileName(f);
    QImage imageRef(f);
    iWidth = imageRef.width();
    iHeight = imageRef.height();
}

void MovieLabel::setZoomFactor(qreal z)
{
    zoomFactor = z;
    adaptSize();
}

void MovieLabel::resizeEvent(QResizeEvent*)
{
    if (zoomFactor <= 0) adaptSize();
}

void MovieLabel::adaptSize()
{
    if (!this->movie()) return;
    if (!this->movie()->currentPixmap()) return;

    int w = iWidth;
    int h = iHeight;
    double ratio = (w + 0.0) / h;

    int currentW = this->width();
    int currentH = this->height();
    double currentRatio = (currentW + 0.0) / currentH;

    if (zoomFactor < 0)
    {
        if (ratio > currentRatio) //adapter la largeur
        {
            w = currentW;
            h = w/ratio;
        }
        else //adapter la hauteur
        {
            h = currentH;
            w = h*ratio;
        }
    }
    else if (zoomFactor == 0)
    {
        if (currentW < w || currentH < h)
        {
            if (ratio > currentRatio)
            {
                w = currentW;
                h = ratio/w;
            }
            else
            {
                h = currentH;
                w = h*ratio;
            }
        }
    }
    else
    {
        w = w*zoomFactor;
        h = h*zoomFactor;
    }

    this->movie()->setScaledSize(QSize(w,h));

    this->movie()->jumpToNextFrame();
}
