#ifndef __READER_H__
#define __READER_H__

class Reader {
public:
	const unsigned char *data;
	int p;
	Reader() {
		p = 0;
	}
	int getNext();
};

class DiffReader {
public:
	const unsigned char *data;
	int p;
	int prev;
	DiffReader() {
		p = prev = 0;
	}
	int getNext();
};

#endif