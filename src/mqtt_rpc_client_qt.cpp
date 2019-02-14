#include "mqtt_rpc_client_qt.h"

#include <QJsonDocument>
#include <QSslConfiguration>
#include <QtDebug>

#define MQTT_GLOBAL_BROKER_HOST "mqtt-rpc.victronenergy.com"
#define MQTT_GLOBAL_BROKER_PORT 8883
#define MQTT_GLOBAL_BROKER_CERT "-----BEGIN CERTIFICATE-----\n"\
"MIIECTCCAvGgAwIBAgIJAM+t3iC8ybEHMA0GCSqGSIb3DQEBCwUAMIGZMQswCQYD\n"\
"VQQGEwJOTDESMBAGA1UECAwJR3JvbmluZ2VuMRIwEAYDVQQHDAlHcm9uaW5nZW4x\n"\
"HDAaBgNVBAoME1ZpY3Ryb24gRW5lcmd5IEIuVi4xIzAhBgNVBAsMGkNDR1ggQ2Vy\n"\
"dGlmaWNhdGUgQXV0aG9yaXR5MR8wHQYJKoZIhvcNAQkBFhBzeXNhZG1pbkB5dGVj\n"\
"Lm5sMCAXDTE0MDkxNzExNTQxOVoYDzIxMTQwODI0MTE1NDE5WjCBmTELMAkGA1UE\n"\
"BhMCTkwxEjAQBgNVBAgMCUdyb25pbmdlbjESMBAGA1UEBwwJR3JvbmluZ2VuMRww\n"\
"GgYDVQQKDBNWaWN0cm9uIEVuZXJneSBCLlYuMSMwIQYDVQQLDBpDQ0dYIENlcnRp\n"\
"ZmljYXRlIEF1dGhvcml0eTEfMB0GCSqGSIb3DQEJARYQc3lzYWRtaW5AeXRlYy5u\n"\
"bDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKVdbAUAElbX+Sh0FATX\n"\
"yhlJ6zqYMHbqCXawgsOe09zHynDCT4GTXuSuoH2kR/1jE8zvWNLHORXa/eahzWJP\n"\
"V4WpXuYsFEyU3r8hxA6y+SR06IT7WHdfN6LXN+qt5KLQbmQLxeb1zElMKW4io/WE\n"\
"N+SWpo5dklXAS6vnq+VRTNwRYnPOUIXKduhvTQp6hEHnLBjYC/Ot8SkC8KtL88cW\n"\
"pH6d7UmeW3333/vNMEMOTLWlOWrR30P6R+gTjbvzasaB6tlcYqW+jO1YDlBwhSEV\n"\
"4As4ziQysuy4qvm41KY/o4Q6P6npsh8MaZuRmi/UTxU2DHAbs/on7qaRi6IkVgvg\n"\
"o6kCAwEAAaNQME4wHQYDVR0OBBYEFPjmM5NYXMw7Wc/TgbLtwPnMAfewMB8GA1Ud\n"\
"IwQYMBaAFPjmM5NYXMw7Wc/TgbLtwPnMAfewMAwGA1UdEwQFMAMBAf8wDQYJKoZI\n"\
"hvcNAQELBQADggEBAEFTeGcmxzzXJIfgUrfKLki+hi2mR9g7qomvw6IB1JQHefIw\n"\
"iKXe14gdp0ytjYL6QoTeEbS2A8VI2FvSbusAzn0JqXdZI+Gwt/CuH0XH40QHpaQ5\n"\
"UAB5d7EGvbB2op7AA/IyR5TwF/cDb1fRbTaTmwDOIo3kuFGEyNCc+PFrN2MvtPHn\n"\
"hHH7fo7joY7mUKdP573bJXFsLwZxlqiycJreroLPFqYwgChaMTStQ71rP5i1eGtg\n"\
"ealQ7kPVtlHmX89tCkfkK77ojm48qgl4gwsI01SikstaPP9fr4ck+U/qIKhSg+Bg\n"\
"nc9OImY9ubQxe+/GQP4KFme2PPqthEWys7ut2HM=\n"\
"-----END CERTIFICATE-----"

#define MQTT_SERVICE_NAME "qt"
#define MQTT_QOS 1
#define MQTT_TIMEOUT 60

#define MQTT_RPC_RX "out"
#define MQTT_RPC_TX "in"
#define MQTT_RPC_TOPIC_FORMAT "P/%1/%2"
#define MQTT_RPC_CLIENT_LOGGING_PREFIX "[MQTTRPCCLIENT]" << __FUNCTION__ << ": "

MqttRpcClientQt::MqttRpcClientQt (
		QHostAddress _host,
		quint16 _port,
		QString _site_id
		) : host(_host), port(_port), site_id(_site_id) {
	init_mqtt_client();
}

MqttRpcClientQt::MqttRpcClientQt(
		QString _username,
		QString _password,
		QString _site_id
		) : username(_username), password(_password), site_id(_site_id) {
	init_mqtt_client();
}

void MqttRpcClientQt::init_mqtt_client()
{
	// Unfortunatly we cannot prevent the dynamic allocation of mqtt_client
	// because there is no method to set the sslConfig once it is created.
    if(!username.isEmpty() && !password.isEmpty()) {
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
        QList<QSslCertificate> certList;
        certList.append(QSslCertificate(QByteArrayLiteral(MQTT_GLOBAL_BROKER_CERT), QSsl::Pem));
        sslConfig.setCaCertificates(certList);

        mqtt_client = new QMQTT::Client(MQTT_GLOBAL_BROKER_HOST, MQTT_GLOBAL_BROKER_PORT, sslConfig);
        mqtt_client->setUsername(username);
        mqtt_client->setPassword(password.toUtf8());
    } else {
        mqtt_client = new QMQTT::Client(host, port);
    }

	mqtt_client->setClientId(MQTT_SERVICE_NAME + static_cast<QString>("-") + site_id);
	connect(mqtt_client, &QMQTT::Client::connected, this, &MqttRpcClientQt::on_connect);
	connect(mqtt_client, &QMQTT::Client::received, this, &MqttRpcClientQt::on_message);
	connect(mqtt_client, &QMQTT::Client::error, this, &MqttRpcClientQt::on_error);
	connect(mqtt_client, &QMQTT::Client::subscribed, this, &MqttRpcClientQt::on_subscribe);
	connect(mqtt_client, &QMQTT::Client::pingresp, this, &MqttRpcClientQt::pingresp);
    mqtt_client->connectToHost();
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
	QString commandId;
	do {
		commandId = token_urlsafe(8);
	} while (commands.contains(commandId));

	cmd->update_timestamp();
	cmd->command_id = commandId;
	commands.insert(commandId, cmd);

	QJsonObject message {{MQTT_RPC_REQ_FIELD_OPCMD, cmd->serialize(commandId)}, {MQTT_RPC_FIELD_TIMESTAMP, QString::number(cmd->get_timestamp())}};
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
