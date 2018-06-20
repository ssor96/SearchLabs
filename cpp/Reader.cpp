#include "Reader.h"

int Reader::readInt() {
	int res = 0;
	do {
		res = (res << 7) | (data[p++] & 127);
	} while ((data[p - 1] & 128) == 0);
	return res;
}

int Reader::getNext() {
	return readInt();
}

int DiffReader::getNext() {
	int res = readInt() + prev + 1;
	prev = res;
	return res;
}