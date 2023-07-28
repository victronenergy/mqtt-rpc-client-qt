#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "op_command.h"

class Ping : public OpCommand {
public:
	Ping(QJsonObject _arguments);
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


class DeviceUpdate : public OpCommand {
public:
	DeviceUpdate(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const override;
	QVector <QString> get_succesful_states() const override;
	void post_process() override;
};

class FileExists : public OpCommand {
public:
	FileExists(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const  override;
};

class FileUploadSingle : public OpCommand {
public:
	FileUploadSingle(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const  override;
	QVector <QString> get_succesful_states() const override;
};

class FileDownloadSingle: public OpCommand {
public:
	FileDownloadSingle(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const  override;
};


// old way to run vuplist: getting devices using DeviceList is preferred
class VupList : public OpCommand {
public:
	VupList(QJsonObject _arguments);
	QString get_op_command() const override;
	bool is_succesful();
	void post_process() override;
};

// old way to run duplist: getting devices using DeviceList is preferred
class DupList : public VupList {
public:
	DupList(QJsonObject _arguments);
	QString get_op_command() const override;
};

// Used for multi part upload, more complex to implement so unless there is a specific reason
// use the FileUploadSingle command above
class FileUploadSendChunk : public OpCommand {
public:
	FileUploadSendChunk(QJsonObject _arguments = QJsonObject());
	QString get_op_command() const override;
	QVector<QString> get_parameters() const override;
	QVector <QString> get_succesful_states() const override;
	bool is_finished() const override;
	bool is_successful() const override;
};

// Used for multi part upload, more complex to implement so unless there is a specific reason
// use the FileUploadSingle command above
class FileUploadAnnounce : public OpCommand {
public:
	FileUploadAnnounce(QJsonObject _arguments);
	QString get_op_command() const override;
	QVector<QString> get_parameters() const override;
	void process_response(QJsonObject op_response, qint32 msgnr) override;
	bool is_finished() const override;
	QVector <QString> get_succesful_states() const override;
protected:
	QHash<qint32, FileUploadSendChunk> send_chunk_commands;
};

class DeviceList : public VupList {
public:
	DeviceList(QJsonObject _arguments);
	QString get_op_command() const override;
};

#endif // PROTOCOL_H
