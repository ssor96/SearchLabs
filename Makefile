CC=g++-7
CFLAGS=-std=c++17 -O3
COMPILE_COMMAND=$(CC) $(CFLAGS)

renumber:
	$(COMPILE_COMMAND) cpp/renumberToks.cpp -o renumber.out

merge:
	$(COMPILE_COMMAND) cpp/merge.cpp cpp/Writer.cpp -o merge.out

engine:
	$(COMPILE_COMMAND) cpp/ListIterator*.cpp cpp/Reader.cpp cpp/JumpTable.cpp cpp/engine.cpp -o engine.out