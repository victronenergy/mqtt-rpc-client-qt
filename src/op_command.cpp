#include "op_command.h"

OpCommand::OpCommand(const QHash<QString, QString> _arguments) : arguments(_arguments) {
    for (int i = 0; i < get_parameters().size(); ++i) {
        QString parameter = get_parameters().at(i);
        if(!arguments.contains(parameter)) {
            // error, missing parameter
        }
    }
    // no error, everything you need is in the arguments
}

QString OpCommand::get_op_command() {
    return "";
}

QVector<QString> OpCommand::get_succesful_states() {
    return QVector<QString>({STATUS_FINISHED, STATUS_DONE});
}

QVector<QString> OpCommand::get_parameters() {
    return QVector<QString>();
}

qint32 OpCommand::get_timeout() {
    return 60;
}

QJsonArray OpCommand::serialize() {
    QJsonObject message_parameters = QJsonObject();
    // set all parameters and their values
    for (int i = 0; i < get_parameters().size(); ++i) {
        QString parameter = get_parameters().at(i);
        message_parameters.insert(parameter, arguments.value(parameter));
    }

    // set the command and if necessary the subcommand
    QList<QString> op_command_split = get_op_command().split('_');
    QString command = op_command_split[0];
    if(op_command_split.length() == 2) {
        message_parameters.insert("cmd", op_command_split[1]);
    }

    message_parameters.insert("commandid", token_urlsafe(8));

    QJsonArray result = QJsonArray();
    result.append(command);
    result.append(message_parameters);
    return result;
}

bool OpCommand::is_finished() {
    return finished;
}

bool OpCommand::is_successful() {
    if(!is_finished()) {
        qWarning() << "Not finished error";
        // TODO: return notfinishederror
        return false;
    }

    bool success = true;
    if(last_response.contains("status")) {
        QString last_response_status = last_response.value("status").toString();
        if(last_response_status == "error") {
            success = false;
        } else {
            success = get_succesful_states().contains(last_response_status);
        }
    }
    return success;
}

bool OpCommand::is_timed_out() {
    if(timestamp == 0) {
        qWarning() << "Command not sent error";
        // TODO: raise CommandNotSentError
        return false;
    }
    qint64 last_activity_timestamp = timestamp;
    if(last_response.contains("ts")) {
        last_activity_timestamp = last_response.value("ts").toInt();
    }

    return QDateTime::currentSecsSinceEpoch() - last_activity_timestamp > get_timeout() && !is_finished();
}

bool OpCommand::ensure_succesful() {
    if(!is_successful()) {
        if(last_response.value("error_code").toInt() == 999) {
            qWarning() << "Already processing error";
            // TODO: return AlreadyProcessingError
            return false;
        }
        qWarning() << "Command unsuccessful error";
        // TODO: return CommandUnsucessfulError
        return false;
    }
    return true;
}

void OpCommand::process_response(QJsonObject op_response, qint32 msgnr) {
    responses[msgnr] = op_response;
    // TOOD: set the last_response to the response with the highest msgnr
    last_response = op_response;
}

void OpCommand::post_process() {
    ensure_succesful();

    if(responses.count() > 1) {
        qWarning() << "Not implemented error, please add custom post-processing for this command!";
        // TODO: return AlreadyProcessingError
        return;
    }

    result = responses[0];
}

