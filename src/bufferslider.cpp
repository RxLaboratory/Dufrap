#include "bufferslider.h"
#include <QtDebug>

BufferSlider::BufferSlider(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    frameSize = 1;
}

void BufferSlider::setNumFrames(int n)
{
    setUpdatesEnabled(false);

    int currentWidth = this->width();

    //vider
    while (frameLabels.count()>0)
    {
        bufferLayout->removeWidget(frameLabels[frameLabels.count()-1]);
        QLabel *l = frameLabels.takeLast();
        delete l;
    }

    if (n<=0) return;

    //remettre
    while (frameLabels.count()<n)
    {
        QLabel *newFrame = new QLabel(this);
        newFrame->resize(1,3);
        newFrame->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
        bufferLayout->addWidget(newFrame);
        frameLabels << newFrame;
    }

    this->resize(currentWidth,3);

    setUpdatesEnabled(true);
}

void BufferSlider::setBufferedFrame(int f)
{
    if (f >= 0 && f <frameLabels.count()) frameLabels[f]->setStyleSheet(bgColorCSS);
}

void BufferSlider::setUnBufferedFrame(int f)
{
    if (f >= 0 && f <frameLabels.count()) frameLabels[f]->setStyleSheet("");
}

void BufferSlider::setBgColorCSS(QString colorCSS)
{
    bgColorCSS = colorCSS;
}
