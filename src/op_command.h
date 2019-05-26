#ifndef OP_COMMAND_H
#define OP_COMMAND_H

#include "util.h"
#include <QJsonArray>
#include <QVector>
#include <QHash>
#include <QString>
#include <QHashIterator>
#include <QJsonObject>
#include <QtDebug>
#include <QDateTime>
#include <QObject>
#include <QAtomicInteger>

#define MQTT_RPC_CMD_LOGGING_PREFIX "[MQTTRPCCMD]" << __FUNCTION__ << ": "

#define MQTT_RPC_FIELD_COMMAND_ID "commandid"
#define MQTT_RPC_FIELD_TIMESTAMP "ts"
#define MQTT_RPC_RESP_FIELD_OPRESP "opResponse"
#define MQTT_RPC_RESP_FIELD_FEEDBACK "feedback"
#define MQTT_RPC_RESP_FIELD_FINISHED "finished"
#define MQTT_RPC_RESP_FIELD_MSG_NR "msgnr"
#define MQTT_RPC_REQ_FIELD_OPCMD "opCommand"
#define MQTT_RPC_REQ_FIELD_SUBCMD "cmd"
#define MQTT_RPC_RESP_FIELD_STATUS "status"
#define MQTT_RPC_RESP_FIELD_ERROR "error"
#define MQTT_RPC_RESP_FIELD_ERROR_CODE "error_code"

#define EXIT_STATUS_NORMAL_EXIT "normal_exit"
#define EXIT_STATUS_TIMEOUT "stopped_by_timeout"
#define EXIT_STATUS_CRASHED "crashed"
#define STATUS_FINISHED "finished"
#define STATUS_ERROR "error"
#define STATUS_WORKING "working"
#define STATUS_RUNNING "running"
#define STATUS_STARTING "starting"
#define STATUS_DONE "done"
#define STATUS_FILE_EXISTS "fileexists"
#define STATUS_WAITING_FOR_CHUNKS "waitingforchunks"
#define STATUS_FILE_RECEIVED "filereceivedok"
#define STATUS_NO_FILE "nofile"

class OpCommand
{
public:
	OpCommand(const QJsonObject _arguments = QJsonObject());
	virtual ~OpCommand() {}

	QJsonArray serialize(const QString & commandId);
	bool ensure_succesful();
	void set_finished();
	// should be void, throws error if fails, doesn't throw error if succesful, error handling not implemented yet hence bool
	virtual void process_response(QJsonObject op_response, qint32 msgnr);
	virtual void post_process();
	void update_timestamp();
	qint64 get_timestamp();

	const QJsonObject & get_result() const;

	virtual bool is_finished() const;
	virtual bool is_successful() const;
	virtual bool is_timed_out() const;
	QString command_id;

protected:
	qint32 get_timeout() const;
	virtual QString get_op_command() const = 0;
	virtual QVector <QString> get_succesful_states() const;
	virtual QVector <QString> get_parameters() const;

	qint64 timestamp = 0;
	bool finished = false;
	QJsonObject arguments;
	QString error_code;
	QString error_message;
	QJsonObject last_response;
	qint32 last_response_msg_nr = -1;
	QJsonObject result;

	QHash<qint32, QJsonObject> responses;
	// missing: fields to define the fields that should be set dynamically
};

#endif // OP_COMMAND_H
