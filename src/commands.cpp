#include "commands.h"

#include <QXmlStreamReader>


Ping::Ping(QJsonObject _arguments) : OpCommand(_arguments) {}
QString Ping::get_op_command() const {
	return "ping";
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

DeviceList::DeviceList(const QJsonObject _arguments) : VupList(_arguments) {}
QString DeviceList::get_op_command() const {
	return "device-list";
}

DeviceUpdate::DeviceUpdate(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString DeviceUpdate::get_op_command() const {
	return "device-update";
}
QVector<QString> DeviceUpdate::get_parameters() const {
	return QVector<QString>({"connection_type", "connection_id", "file_md5_hash"});
}
QVector<QString> DeviceUpdate::get_succesful_states() const {
	return QVector<QString>({STATUS_FINISHED});
}
void DeviceUpdate::post_process() {
	return;
}

FileExists::FileExists(const QJsonObject _arguments) :OpCommand(_arguments) {}
QString FileExists::get_op_command() const {
	return "file-exists";
}
QVector<QString> FileExists::get_parameters() const {
	return QVector<QString>({"file_md5_hash"});
}

FileUploadSingle::FileUploadSingle(const QJsonObject _arguments) :OpCommand(_arguments) {}
QString FileUploadSingle::get_op_command() const {
	return "file-upload-single";
}
QVector<QString> FileUploadSingle::get_parameters() const {
	return QVector<QString>({"file_md5_hash", "file", "filename"});
}
QVector<QString> FileUploadSingle::get_succesful_states() const {
	return QVector<QString>({STATUS_FILE_EXISTS, STATUS_FILE_RECEIVED});
}

FileDownloadSingle::FileDownloadSingle(const QJsonObject _arguments) :OpCommand(_arguments) {}
QString FileDownloadSingle::get_op_command() const {
	return "file-download-single";
}
QVector<QString> FileDownloadSingle::get_parameters() const {
	return QVector<QString>({"file_md5_hash"});
}

// old way to run vuplist: getting devices using DeviceList is preferred
VupList::VupList(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString VupList::get_op_command() const {
	return "vuplist";
}
bool VupList::is_succesful() {
	return last_response.value("exitstatus") == EXIT_STATUS_NORMAL_EXIT;
}
void VupList::post_process() {
	ensure_succesful();

	QString xml_string = "";
	for(int i = 0; i < responses.size(); i++) {
		QJsonObject response = responses.value(i);
		if(response.contains("xmloutput")) {
			xml_string.append(response.value("xmloutput").toString());
		}
	}

	QXmlStreamReader xmlReader(xml_string);
	QJsonArray devices;
	while (!xmlReader.atEnd()) {
			if(xmlReader.name().toString() == "device") {
				QJsonObject device;
				foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
					device.insert(attr.name().toString(), attr.value().toString());
				}
				devices.append(device);
			}
			xmlReader.readNext();
	}
	result.insert("devices", devices);
}

// old way to run duplist: getting devices using DeviceList is preferred
DupList::DupList(const QJsonObject _arguments) : VupList(_arguments) {}
QString DupList::get_op_command() const {
	return "duplist";
}

// Used for multi part upload, more complex to implement so unless there is a specific reason
// Use the FileUploadSingle command above
FileUploadAnnounce::FileUploadAnnounce(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString FileUploadAnnounce::get_op_command() const {
	return "fileupload_announce";
}
QVector<QString> FileUploadAnnounce::get_parameters() const {
	return QVector<QString>({"file_md5_hash", "numberofchunks", "file"});
}
QVector<QString> FileUploadAnnounce::get_succesful_states() const {
	return QVector<QString>({STATUS_WAITING_FOR_CHUNKS, STATUS_FILE_EXISTS});
}
void FileUploadAnnounce::process_response(QJsonObject op_response, qint32 msgnr) {
	if(op_response.contains("ackchunk")) {
		qint32 ack_chunk = op_response.value("ackchunk").toInt();
		FileUploadSendChunk send_chunk_cmd = send_chunk_commands.value(ack_chunk);
		send_chunk_cmd.process_response(op_response, msgnr);
	} else {
		OpCommand::process_response(op_response, msgnr);
	}
}
bool FileUploadAnnounce::is_finished() const {
	return QVector<QString>({STATUS_FILE_EXISTS, STATUS_WAITING_FOR_CHUNKS}).contains(last_response.value("status").toString());
}

// Used for multi part upload, more complex to implement so unless there is a specific reason
// Use the FileUploadSingle command above
FileUploadSendChunk::FileUploadSendChunk(const QJsonObject _arguments) : OpCommand(_arguments) {}
QString FileUploadSendChunk::get_op_command() const {
	return "fileupload_sendchunk";
}
QVector<QString> FileUploadSendChunk::get_parameters() const {
	return QVector<QString>({"chunknr", "chunk"});
}
QVector<QString> FileUploadSendChunk::get_succesful_states() const {
	return QVector<QString>({STATUS_WAITING_FOR_CHUNKS, STATUS_FILE_EXISTS});
}
bool FileUploadSendChunk::is_finished() const {
	return responses.size() > 0;
}
bool FileUploadSendChunk::is_successful() const {
	return is_finished();
}
