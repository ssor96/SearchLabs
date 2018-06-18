renumber:
	g++ -std=c++14 -O2 cpp/renumberToks.cpp -o renumber.out

merge:
	g++ -std=c++14 -O2 cpp/merge.cpp cpp/Writer.cpp -o merge.out

engine:
	g++ -g -std=c++14 -O2 cpp/ListIterator*.cpp cpp/Reader.cpp cpp/engine.cpp -o engine.out