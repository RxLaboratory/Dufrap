#include "framelabel.h"

FrameLabel::FrameLabel(QWidget *parent) :
    QLabel(parent)
{
    start = 0;
    end = 0;
    frameSize = 1.0;
}

int FrameLabel::getStart()
{
    return start;
}

int FrameLabel::getEnd()
{
    return end;
}

void FrameLabel::setStart(int s)
{
    start = s;
}

void FrameLabel::setEnd(int e)
{
    end = s;
}
