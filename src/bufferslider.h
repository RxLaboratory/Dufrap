#ifndef BUFFERSLIDER_H
#define BUFFERSLIDER_H

#include "ui_bufferslider.h"
#include <QLabel>

class BufferSlider : public QWidget, private Ui::BufferSlider
{
    Q_OBJECT

public:
    explicit BufferSlider(QWidget *parent = 0);
    void setNumFrames(int n);
    void setBgColorCSS(QString colorCSS);

public slots:
    void setBufferedFrame(int f);
    void setUnBufferedFrame(int f);
private:
    QList<QLabel *> frameLabels;
    QString bgColorCSS;
    double frameSize;
};

#endif // BUFFERSLIDER_H
