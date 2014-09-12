#ifndef MOVIELABEL_H
#define MOVIELABEL_H

#include <QLabel>

class MovieLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MovieLabel(QWidget *parent = 0);
    void setMovieFile(QString f);

signals:

public slots:
    void setZoomFactor(qreal z);
private:
    qreal zoomFactor; // -1 auto ; 0 Auto until 100%
    void adaptSize();
    int iWidth;
    int iHeight;
protected:
    void resizeEvent(QResizeEvent*);

};

#endif // MOVIELABEL_H
