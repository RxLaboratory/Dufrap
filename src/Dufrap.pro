#-------------------------------------------------
#
# Project created by QtCreator 2014-08-17T18:10:45
#
#-------------------------------------------------

QT       += core gui\
         multimedia\
         multimediawidgets\
         winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dufrap
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    opener.cpp \
    isplayer.cpp \
    bufferedframe.cpp \
    jumpingslider.cpp \
    imagebuffer.cpp \
    unbufferedframe.cpp \
    params.cpp \
    about.cpp \
    bufferslider.cpp \
    framelabel.cpp

HEADERS  += mainwindow.h \
    opener.h \
    isplayer.h \
    bufferedframe.h \
    jumpingslider.h \
    imagebuffer.h \
    unbufferedframe.h \
    params.h \
    about.h \
    bufferslider.h \
    framelabel.h

FORMS    += mainwindow.ui \
    opener.ui \
    params.ui \
    about.ui \
    bufferslider.ui

RESOURCES += \
    ressources.qrc

OTHER_FILES += \
    ico.rc

RC_FILE = ico.rc
