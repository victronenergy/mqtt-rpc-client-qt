QT += testlib gui
QT += network
QT += qmqtt

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app
DEFINES += MQTTRPCCLIENTQT_LIBRARY

SOURCES +=  src/tst_mqtt_rpc_client_qt.cpp

SOURCES += \
    src/mqtt_rpc_client_qt.cpp \
    src/op_command.cpp \
    src/util.cpp \
    src/commands.cpp

HEADERS += \
    src/mqtt-rpc-client-qt_global.h \
    src/mqtt_rpc_client_qt.h \
    src/op_command.h \
    src/util.h \
    src/commands.h
