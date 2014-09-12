#ifndef PARAMS_H
#define PARAMS_H

#include "ui_params.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QTextStream>

class Params : public QDialog, private Ui::Params
{
    Q_OBJECT

public:
    explicit Params(QWidget *parent = 0);
    void setBufferEnabled(bool b);
    void setBufferMaxSize(int b);
    void setBufferAhead(int b);
    void setSkipFrames(bool b);

    bool getBufferEnabled();
    int getBufferMaxSize();
    int getBufferAhead();
    bool getSkipFrames();

    Qt::ToolButtonStyle getStyle();
    void setStyle(Qt::ToolButtonStyle style);

    void setLastBrowsed(QString l);
    QString getLastBrowsed();

    void setLastFPS(double fps);
    double getLastFPS();

    void setNumRecents(int n);
    int getNumRecents();

    QStringList getRVB();
    void setRVB(QList<int> c);

private slots:
    void on_maxSlider_valueChanged(int value);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_comboBox_activated(int index);

private:
    QJsonObject getParams();
    void setParams(QJsonObject p);
    bool buttonStyleAuto;
    QString lastBrowsed;
    double lastFPS;
    bool progra;
protected:
    void resizeEvent(QResizeEvent*);
};

#endif // PARAMS_H
