#ifndef MQTT_RPC_CLIENT_QT_H
#define MQTT_RPC_CLIENT_QT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QHash>
#include <QTimer>
#include <QMutex>

#include "qmqtt.h"
#include "op_command.h"

class MqttRpcClientQt : public QObject
{
	Q_OBJECT
public:
	MqttRpcClientQt() = default;
	// initialiser for mqtt rpc client for local mqtt broker
	MqttRpcClientQt(const QHostAddress& _host, quint16 _port, const QString& _site_id, const QString& _service_name = "");
	// initialiser for mqtt rpc client for online/global mqtt broker
	MqttRpcClientQt(const QString& username, const QString& password, const QString& _site_id, const QString& _service_name = "");
	// custom initialiser
	MqttRpcClientQt(const QString& _host_name, quint16 _port, const QString& _username, const QString& _password, const QString& _site_id, const QString& _service_name = "");

	virtual ~MqttRpcClientQt();
	QString send_command(OpCommand* command); // This transfers ownership of the object to MqttRpcClientQt
	virtual void send_message(const QByteArray& payload, QString topic = NULL);

	virtual void subscribe(const QString& topic);
	virtual void unsubscribe(const QString& topic);
	virtual void publish(quint16 message_id, const QString& topic, const QString& message);

signals:
	void mqtt_error(int errorCode);
	void command_result(const OpCommand& command, const QString& payload = "");
	void connected();

protected:
	QMQTT::Client* mqtt_client{ nullptr };
	QHostAddress host;
	QString host_name;
	quint16 port { 0 };
	QString username;
	QString password;
	QString site_id;
	QString service_name;
	QTimer message_expiration_timer;

	quint16 mqtt_message_id{ 0 };

private:
	void init_mqtt_client();

	QString get_full_topic(const QString& topic);
	void set_message_expiration_timer();

	uint connection_attemtps{ 0 };
	QMutex commands_mutex;
	QHash<QString, OpCommand*> commands;

public slots:
	virtual void on_connect();
	virtual void on_message(const QMQTT::Message&);
	virtual void on_error(const QMQTT::ClientError error);
	virtual void on_subscribe(const QString& topic, const quint8 qos);
	virtual void pingresp();
	virtual void on_message_timeout();
};

#endif // MQTT_RPC_CLIENT_QT_H
