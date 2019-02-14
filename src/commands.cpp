#include "commands.h"

Ping::Ping(QJsonObject _arguments) : OpCommand(_arguments) {}
QString Ping::get_op_command() const {
	return "ping";
}

VupList::VupList(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString VupList::get_op_command() const {
	return "vuplist";
}
bool VupList::is_succesful() {
	return last_response.value("exitstatus") == EXIT_STATUS_NORMAL_EXIT;
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
QString DupList::get_op_command() const {
	return "duplist";
}

VregDeviceList::VregDeviceList(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString VregDeviceList::get_op_command() const {
	return "vreg-device-list";
}

VregSetGet::VregSetGet(const QJsonObject _arguments) : VregDeviceList(_arguments) {}
QString VregSetGet::get_op_command() const {
	return "vreg-get-set";
}
QVector<QString> VregSetGet::get_parameters() const {
	return QVector<QString>({"id", "vregs"});
}
