
CONFIG += c++11 console widgets
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    mxpredict.cpp \
    bufferfile.cpp

HEADERS += \
    mxpredict.h \
    bufferfile.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/mxnet/lib/release/ -lmxnet
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/mxnet/lib/debug/ -lmxnet
else:unix: LIBS += -L$$PWD/mxnet/lib/ -lmxnet

INCLUDEPATH += $$PWD/mxnet/include
DEPENDPATH += $$PWD/mxnet/include
