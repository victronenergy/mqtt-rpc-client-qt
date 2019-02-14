#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "op_command.h"

class Ping : public OpCommand {
public:
	Ping(QJsonObject _arguments);
	QString get_op_command() const override;
};

class VupList : public OpCommand {
public:
	VupList(QJsonObject _arguments);
	QString get_op_command() const override;
	bool is_succesful();
	void post_process();
};

class DupList : public VupList {
public:
	DupList(QJsonObject _arguments);
	QString get_op_command() const override;
};

class VregDeviceList : public OpCommand {
public:
	VregDeviceList(QJsonObject _arguments);
	QString get_op_command() const override;
};

class VregSetGet : public VregDeviceList {
public:
	VregSetGet(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const  override;
};

#endif // PROTOCOL_H
