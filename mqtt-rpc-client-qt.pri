QT += network
CONFIG += qt warn_on depend_includepath

#DEFINES += MQTTRPCCLIENTQT_LIBRARY

INCLUDEPATH *= $$PWD/src
SOURCES += \
    $$PWD/src/mqtt_rpc_client_qt.cpp \
    $$PWD/src/op_command.cpp \
    $$PWD/src/util.cpp \
    $$PWD/src/commands.cpp
HEADERS += \
    $$PWD/src/mqtt-rpc-client-qt_global.h \
    $$PWD/src/mqtt_rpc_client_qt.h \
    $$PWD/src/op_command.h \
    $$PWD/src/util.h \
    $$PWD/src/commands.h
