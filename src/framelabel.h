#ifndef FRAMELABEL_H
#define FRAMELABEL_H

#include <QLabel>

class FrameLabel : public QLabel
{
    Q_OBJECT
public:
    explicit FrameLabel(QWidget *parent = 0);
    void setStart(int s);
    void setEnd(int e);
    void setFrameSize(double s);

    int getStart();
    int getEnd();

signals:

public slots:

private:
    int end;
    int start;
    double frameSize;

};

#endif // FRAMELABEL_H
