#include "util.h"

QString token_urlsafe(qint32 length) {
	// from: https://stackoverflow.com/a/18866593
	const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	QString randomString;
	for(int i=0; i<length; ++i)
	{
		int index = qrand() % possibleCharacters.length();
		QChar nextChar = possibleCharacters.at(index);
		randomString.append(nextChar);
	}
	return randomString;
}
