#include "Reader.h"

int Reader::getNext() {
	int res = 0;
	do {
		res = (res << 7) | (data[p++] & 127);
	} while ((data[p - 1] & 128) == 0);
	return res;
}

int DiffReader::getNext() {
	int res = 0;
	do {
		res = (res << 7) | (data[p++] & 127);
	} while ((data[p - 1] & 128) == 0);
	res += prev + 1;
	prev = res;
	return res;
}