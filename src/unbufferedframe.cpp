#include "unbufferedframe.h"

UnBufferedFrame::UnBufferedFrame(const UnBufferedFrame &uf) :
    QObject()
{
    frameNumber = uf.frameNumber;
    fichier = uf.fichier;
}

UnBufferedFrame::UnBufferedFrame(const QString f, const int fN) :
    QObject()
{
    if (fN < 0)
    {
        frameNumber = -1;
        fichier = "";
    }
    else
    {
        frameNumber = fN;
        fichier = f;
    }
}

int UnBufferedFrame::getFrameNumber()
{
    return frameNumber;
}

QString UnBufferedFrame::getFilePath()
{
    return fichier;
}
