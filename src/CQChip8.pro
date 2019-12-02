TEMPLATE = app

QT += widgets

TARGET = CQChip8

DEPENDPATH += .

MOC_DIR = .moc

INCLUDEPATH += . ../include

QMAKE_CXXFLAGS += -std=c++14

CONFIG += debug

SOURCES += \
CQChip8.cpp \
CQChip8Test.cpp \

HEADERS += \
CChip8.h \
CQChip8.h \
CQChip8Test.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. ../include \

unix:LIBS += \
-L$$LIB_DIR \
