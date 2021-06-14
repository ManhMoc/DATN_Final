#-------------------------------------------------
#
# Project created by QtCreator 2021-06-09T23:29:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = T4
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    lcd1602.cpp \
    mfrc522_debug.cpp \
    mfrc522_hal_linux.cpp \
    mfrc522.cpp \
    dump.c \
    PossibleChar.cpp \
    PossiblePlate.cpp \
    Preprocess.cpp \
    DetectChars.cpp \
    DetectPlates.cpp

HEADERS += \
        mainwindow.h \
    lcd1602.h \
    mfrc522.h \
    dump.h \
    PossibleChar.h \
    PossiblePlate.h \
    Preprocess.h \
    DetectChars.h \
    DetectPlates.h \
    main.h
INCLUDEPATH += /usr/local/include/opencv4
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_ml -lopencv_shape -lopencv_videoio -lbcm2835
FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    classifications.xml \
    images.xml
