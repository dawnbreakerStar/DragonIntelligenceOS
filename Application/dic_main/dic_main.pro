QT += core
QT -= gui

CONFIG += c++17

TARGET = dic_main
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH = $$PWD/../../Include

DESTDIR = $$PWD/../../Debug

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Source/dic_main/main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
