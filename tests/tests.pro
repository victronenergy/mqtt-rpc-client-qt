QT       -= gui
QT       += qmqtt network testlib

TEMPLATE = app
TARGET = tests
INCLUDEPATH += .

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += MQTTRPCCLIENTQT_LIBRARY

SOURCES += tst_mqtt_rpc_client_qt.cpp

SOURCES += \
    ../src/mqtt_rpc_client_qt.cpp \
    ../src/op_command.cpp \
    ../src/util.cpp \
    ../src/commands.cpp

HEADERS += \
    ../src/mqtt_rpc_client_qt.h \
    ../src/op_command.h \
    ../src/util.h \
    ../src/commands.h
