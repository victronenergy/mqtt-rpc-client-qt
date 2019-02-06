#include <QtTest>
#include <QCoreApplication>
#include <QHostAddress>
#include <QSignalSpy>
#include "../src/mqtt_rpc_client_qt.h"
#include "../src/commands.h"
#include "qmqtt.h"

#define HOST "172.16.1.184"
#define PORT 1883
#define SITE_ID "9884e3966ef4"
#define USERNAME "info@elnino-ict.com"
#define PASSWORD ""

class mqtt_rpc_client_qt : public QObject
{
    Q_OBJECT

public:
    mqtt_rpc_client_qt();
    ~mqtt_rpc_client_qt();
    MqttRpcClientQt mqtt_rpc;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_ping();
    void test_vreg_device_list();
};

mqtt_rpc_client_qt::mqtt_rpc_client_qt() : mqtt_rpc(QHostAddress(HOST), PORT, SITE_ID)
{

}

mqtt_rpc_client_qt::~mqtt_rpc_client_qt()
{

}

void mqtt_rpc_client_qt::initTestCase()
{
    qInfo() << "connecting to " << HOST << " at port " << PORT << " using site_id " << SITE_ID;
    QTest::qWait(1000);
    qInfo() << "mqtt broker connection: " << mqtt_rpc.mqtt_client->isConnectedToHost();
}

void mqtt_rpc_client_qt::cleanupTestCase()
{

}

void mqtt_rpc_client_qt::test_ping()
{
    const QJsonObject _arguments;
    Ping cmd(_arguments);

    QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);

    mqtt_rpc.send_command(&cmd);
}

void mqtt_rpc_client_qt::test_vreg_device_list()
{
    const QJsonObject _arguments;
    VregDeviceList cmd(_arguments);

    QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);

    mqtt_rpc.send_command(&cmd);

    QVERIFY(spy.wait(1000));
}


QTEST_MAIN(mqtt_rpc_client_qt)

#include "tst_mqtt_rpc_client_qt.moc"