#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "op_command.h"

class Ping : public OpCommand {
public:
    Ping(const QJsonObject _arguments);
    QString get_op_command();
};

class VupList : public OpCommand {
public:
    VupList(const QJsonObject _arguments);
    QString get_op_command();
    bool is_succesful();
    void post_process();
};

class DupList : public VupList {
public:
    DupList(const QJsonObject _arguments);
    QString get_op_command();
};

class VregDeviceList : public OpCommand {
public:
    VregDeviceList(const QJsonObject _arguments);
    QString get_op_command();
};

class VregSetGet : public VregDeviceList {
public:
    VregSetGet(const QJsonObject _arguments);
    QString get_op_command();
    QVector<QString> get_parameters();
};

#endif // PROTOCOL_H
