#include "op_command.h"

OpCommand::OpCommand(const QJsonObject _arguments) : arguments(_arguments) {
    for (int i = 0; i < get_parameters().size(); ++i) {
        QString parameter = get_parameters().at(i);
        if(!arguments.contains(parameter)) {
            // error, missing parameter
            qWarning() << MQTT_RPC_CMD_LOGGING_PREFIX << "OpCommand construction failed, missing one or multiple arguments in the arguments hashmap. At least: " << parameter;
        }
    }
    // no error, everything you need is in the arguments
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
        message_parameters.insert(MQTT_RPC_REQ_FIELD_SUBCMD, op_command_split[1]);
    }

    message_parameters.insert(MQTT_RPC_FIELD_COMMAND_ID, token_urlsafe(8));

    QJsonArray result = QJsonArray();
    result.append(command);
    result.append(message_parameters);
    return result;
}

void OpCommand::update_timestamp() {
    timestamp = QDateTime::currentSecsSinceEpoch();
}

qint64 OpCommand::get_timestamp() {
    return timestamp;
}

QJsonObject* OpCommand::get_result() {
    return result;
}

void OpCommand::set_finished() {
    finished = true;
}

bool OpCommand::is_finished() {
    return finished;
}

bool OpCommand::is_successful() {
    qDebug() << MQTT_RPC_CMD_LOGGING_PREFIX << "is the command finished? " << is_finished();

    if(!is_finished()) {
        qWarning() << MQTT_RPC_CMD_LOGGING_PREFIX << "Not finished error";
        // TODO: return notfinishederror
        return false;
    }

    bool success = true;
    if(last_response->contains(MQTT_RPC_RESP_FIELD_STATUS)) {
        QString last_response_status = last_response->value(MQTT_RPC_RESP_FIELD_STATUS).toString();
        if(last_response_status == MQTT_RPC_RESP_FIELD_ERROR) {
            success = false;
        } else {
            success = get_succesful_states().contains(last_response_status);
        }
    }
    return success;
}

bool OpCommand::is_timed_out() {
    qDebug() << QDateTime::currentSecsSinceEpoch();
    if(timestamp == 0) {
        qWarning() << MQTT_RPC_CMD_LOGGING_PREFIX << "Command not sent error";
        // TODO: raise CommandNotSentError
        return false;
    }
    return (QDateTime::currentSecsSinceEpoch() - timestamp) > get_timeout() && !is_finished();
}

bool OpCommand::ensure_succesful() {
    if(!is_successful()) {
        if(last_response->value(MQTT_RPC_RESP_FIELD_ERROR_CODE).toInt() == 999) {
            qWarning() << MQTT_RPC_CMD_LOGGING_PREFIX << "Already processing error";
            // TODO: return AlreadyProcessingError
            return false;
        }
        qWarning() << MQTT_RPC_CMD_LOGGING_PREFIX << "Command unsuccessful error";
        // TODO: return CommandUnsucessfulError
        return false;
    }
    return true;
}

void OpCommand::process_response(QJsonObject op_response, qint32 msg_nr) {
    last_response = &op_response;
    qDebug() << MQTT_RPC_CMD_LOGGING_PREFIX << "Processed message " << msg_nr << " --- JSON Object: " << op_response;
    // TOOD: set the last_response to the response with the highest msgnr
    // TODO: save response to a responses array to be able to handle multi message response mqtt-rpc commands
}

void OpCommand::post_process() {
    ensure_succesful();
    // if(responses.count() > 1) {
    //    qWarning() << "Not implemented error, please add custom post-processing for this command!";
        // TODO: return AlreadyProcessingError
    //    return;
    //}

    // copy the last response into a QJsonObject on the heap, be sure to call delete on it afterwards to prevent memory leaking.
    result = new QJsonObject(*last_response);
}

