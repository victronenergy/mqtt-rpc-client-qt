#include "commands.h"

Ping::Ping(const QHash<QString, QString> _arguments) : OpCommand(_arguments) {}
QString Ping::get_op_command() {
    return "ping";
}

VupList::VupList(const QHash<QString, QString> _arguments) : OpCommand(_arguments) {}
QString VupList::get_op_command() {
    return "vuplist";
}
bool VupList::is_succesful() {
    return last_response["exitstatus"] == EXIT_STATUS_NORMAL_EXIT;
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

DupList::DupList(const QHash<QString, QString> _arguments) : VupList(_arguments) {}
QString DupList::get_op_command() {
    return "duplist";
}

VregDeviceList::VregDeviceList(const QHash<QString, QString> _arguments) : OpCommand(_arguments) {}
QString VregDeviceList::get_op_command() {
    return "vregdevicelist";
}

VregSetGet::VregSetGet(const QHash<QString, QString> _arguments) : VregDeviceList(_arguments) {}
QString VregSetGet::get_op_command() {
    // TODO: change this to "vreg-set-get" when it has changed in mqtt-rpc
    return "vregremoteconfig";
}
QVector<QString> VregSetGet::get_parameters() {
    return QVector<QString>({"id", "vregs"});
}
