#include "movielabel.h"
#include <QMovie>
#include <QtDebug>

MovieLabel::MovieLabel(QWidget *parent) :
    QLabel(parent)
{
    this->setScaledContents(true);
    this->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void MovieLabel::setMovieFile(QString f)
{
    this->movie()->setFileName(f);
}

