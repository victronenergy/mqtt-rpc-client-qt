#include "mqtt_rpc_client_qt.h"

MqttRpcClientQt::MqttRpcClientQt(
        QHostAddress _host,
        quint16 _port,
        QString _site_id
        ) : host(_host), port(_port), site_id(_site_id) {
    mqtt_client = get_mqtt_client();
}

QMQTT::Client* MqttRpcClientQt::get_mqtt_client() {
    QMQTT::Client *client = new QMQTT::Client(host, port);
    client->setClientId(MQTT_SERVICE_NAME + static_cast<QString>("-") + site_id);
    connect(client, &QMQTT::Client::connected, this, &MqttRpcClientQt::on_connect);
    connect(client, &QMQTT::Client::received, this, &MqttRpcClientQt::on_message);
    connect(client, &QMQTT::Client::error, this, &MqttRpcClientQt::on_error);
    connect(client, &QMQTT::Client::subscribed, this, &MqttRpcClientQt::on_subscribe);
    client->connectToHost();
    return client;
}

QString MqttRpcClientQt::get_full_topic(QString topic) {
    return QString("P/%1/%2").arg(site_id, topic);
}

void MqttRpcClientQt::on_error(const QMQTT::ClientError error) {
    qWarning() << "mqtt client error: " << error;
}

void MqttRpcClientQt::on_subscribe(const QString& topic, const quint8 qos) {
    qInfo() << "subscribed to topic: " << topic << " with qos: " << qos;
}

void MqttRpcClientQt::on_connect() {
    qInfo() << QString("connected to mqtt broker: %1:%2").arg(host.toString(), port);
    mqtt_client->subscribe(get_full_topic("out"));
    emit connected();
}

void MqttRpcClientQt::on_message(const QMQTT::Message& message) {
    // https://erickveil.github.io/2016/04/06/How-To-Manipulate-JSON-With-C++-and-Qt.html
    auto json_document = QJsonDocument::fromJson(message.payload());

    if(json_document.isNull() && !json_document.isObject()){
        qWarning() << "Received invalid data (not json or not a json object)";
    }

    QJsonObject json_obj=json_document.object();

    if(json_obj.isEmpty()) {
        qWarning() << "Received empty json object";
    }

    // TODO: Error handling
    QVariantMap root_json_map = json_obj.toVariantMap();
    QVariantMap op_response_map = root_json_map["opResponse"].toMap();
    QVariantMap op_response_feedback_map = op_response_map["feedback"].toMap();
    QString command_id = op_response_map["commandid"].toString();
    int msg_nr = op_response_map["msgnr"].toInt();
    qInfo() << "here: " << command_id << "  " << commands << "  " << op_response_map;

    if(commands.contains(command_id)) {
       qInfo() << QString("receive < %1 qos=%2: %3").arg(message.topic()).arg(message.qos()).arg(QString::fromUtf8(message.payload()));
       OpCommand* command = commands[command_id];
       op_response_map.insert("ts", QDateTime::currentSecsSinceEpoch());
       // TODO: verify the type of the finished field
       command->finished = op_response_map.value("finished").toBool() || command->finished;
       command->process_response(json_obj.value("opResponse").toObject().value("feedback").toObject(), msg_nr);
       if(command->is_finished() && command->is_successful()) {
           command->post_process();
           emit command_result(command);
       }
    }
}

/*
 *         retry = self.config['mqtt_rpc']['command_retries'] if retry is None else retry

        command = cmd.serialize()
        cmd.timestamp = int(time.time())
        message = {"opCommand": command, 'ts': str(cmd.timestamp)}
        self.commands[command[1]['commandid']] = cmd
        self.send_message(json.dumps(message))

        if wait_for_response:
            try:
                self.wait_until_finished(cmd)
            except TimeoutError as e:
                if retry == 0:
                    logger.exception("command timed out during the last attempt: {} @ {}".format(
                        cmd.op_command, datetime.fromtimestamp(cmd.timestamp)))
                    raise e
                else:
                    self.send_command(cmd, wait_for_response=True, retry=retry - 1)
            cmd.ensure_successful()
        return cmd*/

void MqttRpcClientQt::send_command(OpCommand* cmd) {
    // probably not doing wait_for_response, going to use SIGNAL/SLOT
    QJsonArray command = cmd->serialize();
    cmd->timestamp = QDateTime::currentSecsSinceEpoch();
    QJsonObject message {{"opCommand", command}, {"ts", QString::number(cmd->timestamp)}};
    qInfo() << "abaasdf asdfas" << command[1].toObject();
    commands.insert(command[1].toObject().value("commandid").toString(), cmd);
    QJsonDocument message_doc(message);
    send_message(message_doc.toJson(QJsonDocument::Compact));
}

void MqttRpcClientQt::send_message(QByteArray payload) {
    qInfo() << mqtt_message_id << " topic: " << get_full_topic("in") << "  " << payload.toBase64();
    mqtt_client->publish(QMQTT::Message(mqtt_message_id, get_full_topic("in"), payload));
    //qInfo() << QString("send > %1: %2").arg(get_full_topic("rpc_tx"), payload.toBase64());
    mqtt_message_id++;
}
