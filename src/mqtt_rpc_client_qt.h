#ifndef MQTT_RPC_CLIENT_QT_H
#define MQTT_RPC_CLIENT_QT_H

#include "op_command.h"
#include "qmqtt.h"

#include <QByteArray>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>
#include <QVariantMap>
#include <QtDebug>
#include <QHash>
#include <string>
#include <QObject>
#include <QSslConfiguration>


class MqttRpcClientQt : public QObject
{
	Q_OBJECT
public:
	// initialiser for mqtt rpc client for local mqtt broker
	MqttRpcClientQt(QHostAddress _host, quint16 _port, QString _site_id);
	// initialiser for mqtt rpc client for online/global mqtt broker
	MqttRpcClientQt(QString username, QString password, QString _site_id);
	QMQTT::Client* mqtt_client;

public slots:
	QString send_command(OpCommand* command);

signals:
	void command_result(OpCommand* command);
	void connected();

private:
	QHostAddress host;
	quint16 port;
	QString username;
	QString password;
	QString site_id;

	QHash<QString, OpCommand*> commands;
	quint16 mqtt_message_id = 0;

    QMQTT::Client* get_mqtt_client();

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
