#include "util.h"

static const char possibleCharacters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

QString token_urlsafe(qint32 length) {
	// from: https://stackoverflow.com/a/18866593
	QString randomString(length, ' ');
	for(int i=0; i<length; ++i)
	{
		int index = qrand() % sizeof(possibleCharacters);
		randomString[i] = possibleCharacters[index];
	}
	return randomString;
}
