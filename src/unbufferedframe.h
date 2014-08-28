#ifndef UNBUFFEREDFRAME_H
#define UNBUFFEREDFRAME_H

#include <QObject>

class UnBufferedFrame : public QObject
{
    Q_OBJECT
public:
    explicit UnBufferedFrame(const UnBufferedFrame & uf);
    explicit UnBufferedFrame(const QString f, const int fN);
    int getFrameNumber();
    QString getFilePath();

signals:

public slots:

private:
    int frameNumber;
    QString fichier;

};

#endif // UNBUFFEREDFRAME_H
