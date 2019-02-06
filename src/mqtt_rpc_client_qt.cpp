#include "mqtt_rpc_client_qt.h"

MqttRpcClientQt::MqttRpcClientQt(
		QHostAddress _host,
		quint16 _port,
		QString _site_id
		) : host(_host), port(_port), site_id(_site_id) {
	mqtt_client = get_mqtt_client();
}

MqttRpcClientQt::MqttRpcClientQt(
        QString _username,
        QString _password,
                QString _site_id
        ) : username(_username), password(_password), site_id(_site_id) {
    mqtt_client = get_mqtt_client();
}


QMQTT::Client* MqttRpcClientQt::get_mqtt_client() {
    QMQTT::Client *client;

    if(!username.isEmpty() && !password.isEmpty()) {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        QList<QSslCertificate> certList;
        QSslCertificate cert(QByteArray(MQTT_GLOBAL_BROKER_CERT), QSsl::Pem);
        certList.append(cert);
        sslConfig.setCaCertificates(certList);

        client = new QMQTT::Client(MQTT_GLOBAL_BROKER_HOST, MQTT_GLOBAL_BROKER_PORT, sslConfig);
        client->setUsername(username);
        client->setPassword(password.toUtf8());
    } else {
        client = new QMQTT::Client(host, port);
    }

	client->setClientId(MQTT_SERVICE_NAME + static_cast<QString>("-") + site_id);
	connect(client, &QMQTT::Client::connected, this, &MqttRpcClientQt::on_connect);
	connect(client, &QMQTT::Client::received, this, &MqttRpcClientQt::on_message);
	connect(client, &QMQTT::Client::error, this, &MqttRpcClientQt::on_error);
	connect(client, &QMQTT::Client::subscribed, this, &MqttRpcClientQt::on_subscribe);
	connect(client, &QMQTT::Client::pingresp, this, &MqttRpcClientQt::pingresp);
    client->connectToHost();
	return client;
}

void MqttRpcClientQt::pingresp() {
	qDebug() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "MQTT Ping response";
}

QString MqttRpcClientQt::get_full_topic(QString topic) {
	return QString("P/%1/%2").arg(site_id, topic);
}

void MqttRpcClientQt::on_error(const QMQTT::ClientError error) {
	qWarning() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "MQTT Error: " << error;
}

void MqttRpcClientQt::on_subscribe(const QString& topic, const quint8 qos) {
	qInfo() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Subscribed to topic: " << topic << " with qos: " << qos;
}

void MqttRpcClientQt::on_connect() {
	qInfo() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Connected to broker: " << host.toString() << port;
	mqtt_client->subscribe(get_full_topic(MQTT_RPC_RX));
	emit connected();
}

void MqttRpcClientQt::on_message(const QMQTT::Message& message) {
	// https://erickveil.github.io/2016/04/06/How-To-Manipulate-JSON-With-C++-and-Qt.html
	qDebug().noquote() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "got message: " << message.payload();
	auto json_document = QJsonDocument::fromJson(message.payload());

	if(json_document.isNull() && !json_document.isObject()){
		qWarning() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Received invalid data (not json or not a json object)";
	}

	QJsonObject json_obj=json_document.object();

	if(json_obj.isEmpty()) {
		qWarning() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Received empty json object";
	}

	// TODO: Error handling
	// TODO: retry command if already processing or time out (?)
	QVariantMap root_json_map = json_obj.toVariantMap();
	QVariantMap op_response_map = root_json_map[MQTT_RPC_RESP_FIELD_OPRESP].toMap();
	QVariantMap op_response_feedback_map = op_response_map[MQTT_RPC_RESP_FIELD_FEEDBACK].toMap();
	QString command_id = op_response_map[MQTT_RPC_FIELD_COMMAND_ID].toString();
	int msg_nr = op_response_map[MQTT_RPC_RESP_FIELD_MSG_NR].toInt();

	if(commands.contains(command_id)) {
		qInfo().noquote() << QString("receive < %1 qos=%2: %3").arg(message.topic()).arg(message.qos()).arg(QString::fromUtf8(message.payload()));
		OpCommand* command = commands[command_id];

		if(op_response_map.value(MQTT_RPC_RESP_FIELD_FINISHED).toBool()) {
			command->set_finished();
			qDebug() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "command with id: " << command_id << " is finished: " << command->is_finished();
		}
		if (json_obj.contains(MQTT_RPC_RESP_FIELD_OPRESP)) {
			QJsonObject obj = json_obj.find(MQTT_RPC_RESP_FIELD_OPRESP).value().toObject();

			if (obj.contains(MQTT_RPC_RESP_FIELD_FEEDBACK)) {
				command->process_response(obj.find(MQTT_RPC_RESP_FIELD_FEEDBACK).value().toObject(), msg_nr);
				if(command->is_finished()) {
					qDebug() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Command finished and successful";
					command->post_process();
					qDebug() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "Post processed! Emitting command_result signal";
					emit command_result(command);
				} else {
					qDebug() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "received a response for a command, but it was unsuccessful or it is part of a command which has multiple response messages. finished: " << command->is_finished() << "successful: " << (command->is_finished() && command->is_successful());
				}
			} else {
				qWarning() << "received an invalid command missing the " << MQTT_RPC_RESP_FIELD_FEEDBACK << " field in the " << MQTT_RPC_RESP_FIELD_OPRESP << " object";
			}
		} else {
			qWarning() << "received an invalid command missing the " << MQTT_RPC_RESP_FIELD_OPRESP << " field";
		}

		if(command->is_timed_out() || command->is_finished()) {
			// command is finished or it timed out, removing it from the client commands to free memory
			commands.remove(command_id);
		}
	} else {
		qWarning() << MQTT_RPC_CLIENT_LOGGING_PREFIX << "received command with unknown command_id, possibly belongs to another mqtt-rpc client";
	}
}

QString MqttRpcClientQt::send_command(OpCommand* cmd) {
	cmd->update_timestamp();
	QJsonArray command = cmd->serialize();
	QJsonObject message {{MQTT_RPC_REQ_FIELD_OPCMD, command}, {MQTT_RPC_FIELD_TIMESTAMP, QString::number(cmd->get_timestamp())}};

	QString commandId = command[1].toObject().value(MQTT_RPC_FIELD_COMMAND_ID).toString();
	cmd->command_id = commandId;

	commands.insert(commandId, cmd);
	QJsonDocument message_doc(message);
	send_message(message_doc.toJson(QJsonDocument::Compact));

	return commandId;
}

void MqttRpcClientQt::send_message(QByteArray payload) {
	QString topic = get_full_topic(MQTT_RPC_TX);
	mqtt_client->publish(QMQTT::Message(mqtt_message_id, topic, payload));
	mqtt_message_id++;
	qInfo().noquote() << MQTT_RPC_CLIENT_LOGGING_PREFIX << QString("Send > %1: %2").arg(topic, QString::fromUtf8(payload));
}
