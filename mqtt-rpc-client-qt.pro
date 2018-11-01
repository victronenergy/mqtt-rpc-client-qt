QT       -= gui
QT       += qmqtt network

TARGET = mqtt-rpc-client-qt
TEMPLATE = lib

DEFINES += MQTTRPCCLIENTQT_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/mqtt_rpc_client_qt.cpp \
    src/op_command.cpp \
    src/util.cpp \
    src/commands.cpp

HEADERS += \
    src/mqtt_rpc_client_qt.h \
    src/mqtt-rpc-client-qt_global.h \
    src/op_command.h \
    src/util.h \
    src/commands.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
