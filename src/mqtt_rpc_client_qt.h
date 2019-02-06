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
	void pingresp();

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

	void on_connect();

	void on_message(const QMQTT::Message&);

	void on_error(const QMQTT::ClientError error);

	void on_subscribe(const QString& topic, const quint8 qos);

	void send_message(QByteArray payload);

};

#endif // MQTT_RPC_CLIENT_QT_H
