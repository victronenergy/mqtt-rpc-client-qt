#include "commands.h"

Ping::Ping(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString Ping::get_op_command() {
	return "ping";
}

VupList::VupList(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString VupList::get_op_command() {
	return "vuplist";
}
bool VupList::is_succesful() {
	return last_response->value("exitstatus") == EXIT_STATUS_NORMAL_EXIT;
}
void VupList::post_process() {
	ensure_succesful();
	// TODO: convert xml to result
	/*
	self.result = ""
	for msg_nr, response in self.responses.items():
		if "xmloutput" in response:
		self.result += response["xmloutput"]

	parsed_xml = ElementTree.fromstring(self.result)
	self.result = [device.attrib for device in parsed_xml.findall(".//device")]
	*/
}

DupList::DupList(const QJsonObject _arguments) : VupList(_arguments) {}
QString DupList::get_op_command() {
	return "duplist";
}

VregDeviceList::VregDeviceList(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString VregDeviceList::get_op_command() {
	return "vreg-device-list";
}

VregSetGet::VregSetGet(const QJsonObject _arguments) : VregDeviceList(_arguments) {}
QString VregSetGet::get_op_command() {
	return "vreg-get-set";
}
QVector<QString> VregSetGet::get_parameters() {
	return QVector<QString>({"id", "vregs"});
}
