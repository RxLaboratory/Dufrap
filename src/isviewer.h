#ifndef ISVIEWER_H
#define ISVIEWER_H

#include "ui_isviewer.h"
#include "isplayer.h"
#include "movielabel.h"
#include <QMouseEvent>
#include <QScrollBar>

class ISViewer : public QWidget, private Ui::ISViewer
{
    Q_OBJECT

public:
    explicit ISViewer(ISPlayer *p, MovieLabel *m, QWidget *parent = 0);
    void resizePlayer();
    void setMovie(bool m);
public slots:
    void setZoomFactor(qreal z);
private:
    ISPlayer *player;
    MovieLabel *movie;
    qreal zoomFactor; // -1 auto ; 0 Auto until 100%
    int oX;
    int oY;
protected:
    void resizeEvent(QResizeEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // ISVIEWER_H
