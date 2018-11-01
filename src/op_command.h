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

class OpCommand {
public:
    OpCommand(const QHash<QString, QString> _arguments = QHash<QString, QString>());
    virtual ~OpCommand() {}

    QJsonArray serialize();
    bool is_finished();
    bool is_successful();
    bool is_timed_out();

    // should be void, throws error if fails, doesn't throw error if succesful, error handling not implemented yet hence bool
    bool ensure_succesful();
    void process_response(QJsonObject op_response, qint32 msgnr);
    void post_process();
    bool finished = false;
    qint64 timestamp;
protected:
    virtual QString get_op_command() = 0;
    virtual QVector <QString> get_succesful_states();
    virtual QVector <QString> get_parameters();
    virtual qint32 get_timeout();

    QHash<QString, QString> arguments;
    QJsonObject result;
    QString error_code;
    QString error_message;
    QHash<qint32, QJsonObject> responses;
    QJsonObject last_response;
    // missing: fields to define the fields that should be set dynamically
};

#endif // OP_COMMAND_H
