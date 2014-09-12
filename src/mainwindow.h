#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QFile>
#include <QTime>
#include "isplayer.h"
#include "jumpingslider.h"
#include "bufferslider.h"
#include <QMovie>
#include "movielabel.h"
#include "isviewer.h"
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#include <QWinJumpList>
#include <QWinJumpListCategory>
#endif

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char *argv[], QWidget *parent = 0);

private slots:
    void on_actionOuvrir_triggered();
    void on_actionQuitter_triggered();
    void on_actionLecture_triggered(bool checked);
    void seeked(int position);
    void on_actionStop_triggered();
    void on_actionImage_Suivante_triggered();
    void on_actionImage_pr_c_dente_triggered();
    void on_actionBoucle_triggered(bool checked);
    void iSBufferStatusChanged(int percentFilled);
    void iSStatusChanged(QMediaPlayer::MediaStatus status);
    void movieStateChanged(QMovie::MovieState state);
    void iSStateChanged(QMediaPlayer::State state);
    void iSPositionChanged(qint64 position);
    void iSDurationChanged(qint64 duration);
    void on_fpsBox_currentIndexChanged(int index);
    void evalFramerate(qint64 currentFrameDuration);
    void on_actionParam_tres_triggered();
    void on_zoomButton_currentIndexChanged(int index);
    void on_actionA_propos_triggered();
    void on_speedBox_valueChanged(double arg1);

private:
    void addFrames(QStringList f);
    ISViewer *iSViewer; //container for players
    ISPlayer *iSPlayer; //Image sequence player and widget
    MovieLabel *movieLabel; //label displaying movie (GIF or MNG file)
    QMovie *movie; //movie player
    bool progra; //used to test if a button has been triggered programmatically
    JumpingSlider *seekBar; //seekbar inherits QSlider but handles mouse event differently
    qreal frameRate;
    bool buttonStyleAuto;
    BufferSlider *bufferSlider;
    bool isMovie;
#ifdef Q_OS_WIN
    QWinTaskbarButton *taskbarButton;
    QWinTaskbarProgress *taskbarProgress;
#endif

protected:
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent* );
};

#endif // MAINWINDOW_H
