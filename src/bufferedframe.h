#ifndef BUFFEREDFRAME_H
#define BUFFEREDFRAME_H

#include <QPixmap>

class BufferedFrame : public QObject
{
    Q_OBJECT
public:
    explicit BufferedFrame(QString f, int fN);
    int getFrameNumber();
    QString getFilePath();
    QPixmap getPixmap();

signals:

public slots:

private:
    int frameNumber;
    QString fichier;
    QPixmap pix;

};

#endif // BUFFEREDFRAME_H
