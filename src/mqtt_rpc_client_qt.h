#ifndef MQTT_RPC_CLIENT_QT_H
#define MQTT_RPC_CLIENT_QT_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QHostAddress>
#include <QHash>
#include <QTimer>
#include <QMutex>
#include <QMqttClient>
#include <QMqttMessage>
#include <QWebSocket>

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
	// initialiser por websockets
	MqttRpcClientQt(const QHostAddress &_host, const QString &_site_id,  quint16 _port, const QString &cookie, const QString &_service_name = "");

	virtual ~MqttRpcClientQt();
	QString send_command(OpCommand* command); // This transfers ownership of the object to MqttRpcClientQt
	virtual void send_message(const QByteArray& payload);

	virtual void subscribe(const QString& topic);
	virtual void unsubscribe(const QString& topic);

	virtual void closeAndDelete();
	QString getClientId() { return mqtt_client.clientId(); }

signals:
	void mqtt_error(int errorCode);
	void command_result(const OpCommand& command, const QString& payload = "");
	void connected();

protected:
	class WebSocketIODevice;
	WebSocketIODevice* web_socket_device{nullptr};
	QMqttClient mqtt_client;
	QHostAddress host;
	QString host_name;
	quint16 port { 0 };
	QString username;
	QString password;
	QString site_id;
	QString service_name;
	QTimer message_expiration_timer;

private:
	void init_mqtt_client();
	void init_mqtt_client_websocket(const QString &cookie);

	QString get_full_topic(const QString& topic);
	void set_message_expiration_timer();
	void connectClientSignals();
	int mPendingUnsubs = 0;
	void cleanupCompleted();

	uint connection_attemtps{ 0 };
	QMutex commands_mutex;
	QHash<QString, OpCommand*> commands;
	QList<QMqttSubscription*> mSubscribedTopics;

public slots:
	virtual void on_connect();
	virtual void on_message(const QByteArray &message, const QMqttTopicName &topic);
	virtual void on_error(const QMqttClient::ClientError error);
	virtual void pingresp();
	virtual void on_message_timeout();
};

class MqttRpcClientQt::WebSocketIODevice : public QIODevice
{
	Q_OBJECT
public:
	WebSocketIODevice(QObject *parent = nullptr);

	bool isSequential() const override;
	qint64 bytesAvailable() const override;

	bool open(OpenMode mode) override;
	void close() override;

	qint64 readData(char *data, qint64 maxlen) override;
	qint64 writeData(const char *data, qint64 len) override;

	void setCookie(QString cookie);
	void setUrl(const QUrl &url);
	void setProtocol(const QByteArray &data);
	void setOutgoingFrameSize(quint64 outgoingFrameSize);
Q_SIGNALS:
	void socketConnected();

public slots:
	void handleBinaryMessage(const QByteArray &msg);
	void onSocketConnected();

private:
	QByteArray m_protocol;
	QByteArray m_buffer;
	QWebSocket m_socket;
	QUrl m_url;
	QString m_cookie;
};

#endif // MQTT_RPC_CLIENT_QT_H
