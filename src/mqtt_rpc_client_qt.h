#ifndef MQTT_RPC_CLIENT_QT_H
#define MQTT_RPC_CLIENT_QT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QHash>

#include "qmqtt.h"
#include "op_command.h"

class MqttRpcClientQt : public QObject
{
	Q_OBJECT
public:
	// initialiser for mqtt rpc client for local mqtt broker
	MqttRpcClientQt(QHostAddress _host, quint16 _port, QString _site_id);
	// initialiser for mqtt rpc client for online/global mqtt broker
	MqttRpcClientQt(QString username, QString password, QString _site_id);

public slots:
	QString send_command(OpCommand* command);

signals:
	void command_result(OpCommand* command);
	void connected();

private:
	QMQTT::Client * mqtt_client;
	QHostAddress host;
	quint16 port;
	QString username;
	QString password;
	QString site_id;

	QHash<QString, OpCommand*> commands;
	quint16 mqtt_message_id = 0;

	void init_mqtt_client();

	QString get_full_topic(QString topic);

private slots:
	void on_connect();
	void on_message(const QMQTT::Message&);
	void on_error(const QMQTT::ClientError error);
	void on_subscribe(const QString& topic, const quint8 qos);
	void send_message(QByteArray payload);
	void pingresp();

};

#endif // MQTT_RPC_CLIENT_QT_H
