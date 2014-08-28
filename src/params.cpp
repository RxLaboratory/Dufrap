#include "params.h"

Params::Params(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

void Params::on_maxSlider_valueChanged(int value)
{
    if (value < aheadSlider->value())
    {
        aheadSlider->setValue(value);
    }

    aheadSlider->setMaximum(value);
    aheadSpinner->setMaximum(value);



    //calculs
    t720p8->setValue(value*1.280*0.720*3/1.048576+0.3);
    t1080p8->setValue(value*1.920*1.080*3/1.048576+0.3);
    t2KS8->setValue(value*2.048*0.858*3/1.048576+0.3);
    t2KF8->setValue(value*1.998*1.080*3/1.048576+0.3);
    t4KS8->setValue(value*4.096*1.716*3/1.048576+0.3);
    t4KF8->setValue(value*3.996*2.160*3/1.048576+0.3);

    t720p12->setValue(value*1.280*0.720*4.5/1.048576+0.3);
    t1080p12->setValue(value*1.920*1.080*4.5/1.048576+0.3);
    t2KS12->setValue(value*2.048*0.858*4.5/1.048576+0.3);
    t2KF12->setValue(value*1.998*1.080*4.5/1.048576+0.3);
    t4KS12->setValue(value*4.096*1.716*4.5/1.048576+0.3);
    t4KF12->setValue(value*3.996*2.160*4.5/1.048576+0.3);
}

bool Params::getBufferEnabled()
{
    return bufferBox->isChecked();
}

int Params::getBufferMaxSize()
{
    return maxSpinner->value();
}

int Params::getBufferAhead()
{
    return aheadSpinner->value();
}

bool Params::getSkipFrames()
{
    return skipFramesButton->isChecked();
}

void Params::setBufferEnabled(bool b)
{
    bufferBox->setChecked(b);
}

void Params::setBufferMaxSize(int b)
{
    maxSlider->setValue(b);
    maxSpinner->setValue(b);
    on_maxSlider_valueChanged(b);
}

void Params::setBufferAhead(int b)
{
    aheadSlider->setValue(b);
    aheadSpinner->setValue(b);
}

void Params::setSkipFrames(bool b)
{
    skipFramesButton->setChecked(b);
}

void Params::on_buttonBox_accepted()
{
    accept();
}

void Params::on_buttonBox_rejected()
{
    reject();
}
