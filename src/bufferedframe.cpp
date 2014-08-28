#include "bufferedframe.h"

BufferedFrame::BufferedFrame(QString f, int fN) :
    QObject()
{
    if (fN < 0)
    {
        frameNumber = -1;
        fichier = "";
        pix = QPixmap();
    }
    else
    {
        frameNumber = fN;
        fichier = f;
        pix = QPixmap(f);
    }
}

int BufferedFrame::getFrameNumber()
{
    return frameNumber;
}

QString BufferedFrame::getFilePath()
{
    return fichier;
}

QPixmap BufferedFrame::getPixmap()
{
    return pix;
}
