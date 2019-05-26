#include <QtTest>
#include <QCoreApplication>
#include <QHostAddress>
#include <QSignalSpy>
#include "../src/mqtt_rpc_client_qt.h"
#include "../src/commands.h"
#include "qmqtt.h"

#define HOST "172.25.9.90"
#define PORT 1883
#define SITE_ID "883314f6a735"
#define USERNAME ""
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
	void test_vuplist();
	void test_duplist();
	void test_fileexists();
	void test_fileuploadsingle();
	void test_filedownloadsingle();
	void test_deviceupdate();
	void test_devicelist();
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
}

void mqtt_rpc_client_qt::cleanupTestCase()
{

}

void mqtt_rpc_client_qt::test_ping()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject _arguments;
	Ping cmd(_arguments);

	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));

	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd.is_successful());
}

void mqtt_rpc_client_qt::test_vreg_device_list()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject _arguments;
	VregDeviceList cmd(_arguments);

	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));

	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd.is_successful());
}

void mqtt_rpc_client_qt::test_vuplist()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject _arguments;
	VupList cmd(_arguments);

	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));

	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd.is_successful());
}

void mqtt_rpc_client_qt::test_duplist()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject _arguments;
	DupList cmd(_arguments);

	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));

	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd.is_successful());
}

void mqtt_rpc_client_qt::test_fileexists()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject arguments {{"file_md5_hash", "abc"}};
	FileExists cmd(arguments);
	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));
	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}
	QVERIFY(cmd.is_successful() && !cmd.get_result().value("file_exists").toBool());
}

void mqtt_rpc_client_qt::test_fileuploadsingle()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject arguments1 {
		{"file_md5_hash", "591785b794601e212b260e25925636fd"},
		{"file", "d29ybGQK"},
		{"filename", "test.txt"}
	};

	FileUploadSingle cmd1(arguments1);
	mqtt_rpc.send_command(&cmd1);
	QVERIFY(spy.wait(20000));

	while(!cmd1.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd1.is_successful());

	const QJsonObject arguments2 {{"file_md5_hash", "591785b794601e212b260e25925636fd"}};
	FileExists cmd2(arguments2);

	mqtt_rpc.send_command(&cmd2);
	QVERIFY(spy.wait(20000));

	while(!cmd2.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd2.is_successful() && cmd2.get_result().value("file_exists").toBool());
}

void mqtt_rpc_client_qt::test_filedownloadsingle()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject arguments1 {
		{"file_md5_hash", "591785b794601e212b260e25925636fd"},
		{"file", "d29ybGQK"},
		{"filename", "test.txt"}
	};

	FileUploadSingle cmd1(arguments1);
	mqtt_rpc.send_command(&cmd1);
	QVERIFY(spy.wait(20000));

	while(!cmd1.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd1.is_successful());

	const QJsonObject arguments2 {{"file_md5_hash", "591785b794601e212b260e25925636fd"}};
	FileExists cmd2(arguments2);
	mqtt_rpc.send_command(&cmd2);
	QVERIFY(spy.wait(20000));

	while(!cmd2.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd2.is_successful() && cmd2.get_result().value("file_exists").toBool());

	FileDownloadSingle cmd3(arguments2);
	mqtt_rpc.send_command(&cmd3);
	QVERIFY(spy.wait(20000));
	while(!cmd3.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd3.is_successful() && cmd3.get_result().value("file").toString() == "d29ybGQK");}

void mqtt_rpc_client_qt::test_deviceupdate()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	QFile file(":/BMV-712.dup.b64");
	file.open(QFile::ReadOnly);
	QTextStream in(&file);
	QString firmware_file = in.readAll();
	const QJsonObject arguments1 {
		{"file_md5_hash", "137082b3bebd08b5f5b7cbc8e6adac17"},
		{"file", firmware_file},
		{"filename", "test.dup"}
	};
	FileUploadSingle cmd1(arguments1);
	mqtt_rpc.send_command(&cmd1);
	QVERIFY(spy.wait(20000));

	while(!cmd1.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd1.is_successful());

	const QJsonObject arguments2 {
		{"connection_type", "direct"},
		{"connection_id", "/dev/ttyO2"},
		{"file_md5_hash", "137082b3bebd08b5f5b7cbc8e6adac17"}
	};

	DeviceUpdate cmd2(arguments2);
	mqtt_rpc.send_command(&cmd2);
	QVERIFY(spy.wait(300000));

	while(!cmd2.is_finished()) {
		QTest::qWait(100);
	}

	QVERIFY(cmd2.is_successful());
}

void mqtt_rpc_client_qt::test_devicelist()
{
	QSignalSpy spy(&mqtt_rpc, &MqttRpcClientQt::command_result);
	const QJsonObject _arguments;
	DeviceList cmd(_arguments);

	mqtt_rpc.send_command(&cmd);
	QVERIFY(spy.wait(20000));

	while(!cmd.is_finished()) {
		QTest::qWait(100);
	}
	qInfo() << cmd.get_result();
	QVERIFY(cmd.is_successful());

}

QTEST_MAIN(mqtt_rpc_client_qt)

#include "tst_mqtt_rpc_client_qt.moc"
