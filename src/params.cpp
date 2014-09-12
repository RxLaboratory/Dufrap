#include "params.h"
#include <QtDebug>

Params::Params(QWidget *parent) :
    QDialog(parent)
{
    progra = true;
    setupUi(this);

    buttonStyleAuto = true;

    QJsonObject params = getParams();


    Qt::ToolButtonStyle style = Qt::ToolButtonStyle(params.value("buttonStyle").toInt());
    setStyle(style);
    setNumRecents(params.value("numRecents").toInt());
    QList<int> rvb;
    rvb << params.value("rBG").toInt() << params.value("vBG").toInt() << params.value("bBG").toInt();
    setRVB(rvb);
    setBufferEnabled(params.value("bufferEnabled").toBool());
    setBufferMaxSize(params.value("bufferMaxSize").toInt());
    setBufferAhead(params.value("bufferAhead").toInt());
    setSkipFrames(params.value("skipFrames").toBool());
    setLastBrowsed(params.value("lastBrowsed").toString());
    setLastFPS(params.value("lastFPS").toDouble());
    progra = false;
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

void Params::on_comboBox_activated(int index)
{
    if (index == 0) buttonStyleAuto = true;
    if (index == 1)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
    if (index == 2)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    if (index == 3)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    if (index == 4)
    {
        buttonStyleAuto = false;
        toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
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

Qt::ToolButtonStyle Params::getStyle()
{
    Qt::ToolButtonStyle style = toolButton->toolButtonStyle();
    if (buttonStyleAuto == true) style = Qt::ToolButtonFollowStyle;
    return style;
}

void Params::setStyle(Qt::ToolButtonStyle style)
{

    if (style == Qt::ToolButtonIconOnly)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(1);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextOnly)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(2);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextBesideIcon)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(3);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonTextUnderIcon)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(4);
        buttonStyleAuto = false;
    }
    else if (style == Qt::ToolButtonFollowStyle)
    {
        toolButton->setToolButtonStyle(style);
        comboBox->setCurrentIndex(0);
        buttonStyleAuto = true;
    }
}

void Params::setLastBrowsed(QString l)
{
    lastBrowsed = l;
    on_buttonBox_accepted();
}

QString Params::getLastBrowsed()
{
    return lastBrowsed;
}

void Params::setLastFPS(double fps)
{
    lastFPS = fps;
    on_buttonBox_accepted();
}

double Params::getLastFPS()
{
    return lastFPS;
}

void Params::setNumRecents(int n)
{
    numRecentsSpinner->setValue(n);
}

int Params::getNumRecents()
{
    return numRecentsSpinner->value();
}

QStringList Params::getRVB()
{
    QStringList rvb;
    rvb << QString::number(rSpinner->value()) << QString::number(vSpinner->value()) << QString::number(bSpinner->value());
    return rvb;
}

void Params::setRVB(QList<int> c)
{
    rSpinner->setValue(c[0]);
    vSpinner->setValue(c[1]);
    bSpinner->setValue(c[2]);
}

QJsonObject Params::getParams()
{
    //charger
    QJsonDocument paramsDoc;
    QFile paramsFile(QDir::homePath() + "/Dumep/params.dufrap");
    if (paramsFile.exists())
    {
        if (paramsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&paramsFile);
            paramsDoc = QJsonDocument::fromJson(in.readAll().toUtf8());
            paramsFile.close();
        }
    }
    else
    {
        //enregistrer
        QJsonObject params;
        params.insert("bufferEnabled",true);
        params.insert("bufferMaxSize",200);
        params.insert("bufferAhead",50);
        params.insert("skipFrames",false);
        params.insert("buttonStyle",Qt::ToolButtonFollowStyle);
        params.insert("lastBrowsed","");
        params.insert("lastFPS",25);
        params.insert("numRecents",10);
        params.insert("rBG",-1);
        params.insert("vBG",-1);
        params.insert("bBG",-1);
        paramsDoc.setObject(params);
        if (paramsFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&paramsFile);
            out << paramsDoc.toJson();
            paramsFile.close();
        }
    }
    return paramsDoc.object();
}

void Params::setParams(QJsonObject p)
{
    QJsonDocument paramsDoc;
    paramsDoc.setObject(p);
    QFile paramsFile(QDir::homePath() + "/Dumep/params.dufrap");
    //enregistrer
    if (paramsFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&paramsFile);
        out << paramsDoc.toJson();
        paramsFile.close();
    }
}

void Params::on_buttonBox_accepted()
{
    QJsonObject params;
    params.insert("buttonStyle",getStyle());
    params.insert("bufferEnabled",getBufferEnabled());
    params.insert("bufferMaxSize",getBufferMaxSize());
    params.insert("bufferAhead",getBufferAhead());
    params.insert("skipFrames",getSkipFrames());
    params.insert("lastBrowsed",lastBrowsed);
    params.insert("lastFPS",lastFPS);
    params.insert("numRecents",getNumRecents());
    params.insert("rBG",rSpinner->value());
    params.insert("vBG",vSpinner->value());
    params.insert("bBG",bSpinner->value());
    setParams(params);
    accept();
}

void Params::on_buttonBox_rejected()
{
    reject();
}

void Params::resizeEvent(QResizeEvent*)
{
    if (buttonStyleAuto)
    {
        if (this->width() > 450) toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (this->width() > 420) toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        else toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

