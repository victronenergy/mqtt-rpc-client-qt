#ifndef MQTT_RPC_CLIENT_QT_H
#define MQTT_RPC_CLIENT_QT_H

#include "mqtt-rpc-client-qt_global.h"
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

#define MQTT_SERVICE_NAME "qt"
#define MQTT_QOS 1
#define MQTT_TIMEOUT 60
#define MQTT_RPC_RX "out"
#define MQTT_RPC_TX "in"
#define MQTT_RPC_TOPIC_FORMAT "P/%1/%2"

class MQTTRPCCLIENTQTSHARED_EXPORT MqttRpcClientQt : public QObject
{
    Q_OBJECT
public:
    MqttRpcClientQt(QHostAddress _host, quint16 _port, QString _site_id);
    QMQTT::Client* mqtt_client;

public slots:
    void send_command(OpCommand* command);

signals:
    void command_result(OpCommand* command);

private:
    QHostAddress host;
    quint16 port;
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
