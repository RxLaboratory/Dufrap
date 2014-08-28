#ifndef PARAMS_H
#define PARAMS_H

#include "ui_params.h"

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

private slots:
    void on_maxSlider_valueChanged(int value);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
};

#endif // PARAMS_H
