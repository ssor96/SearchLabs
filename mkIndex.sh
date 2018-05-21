#!/bin/bash
time (
    echo "Tokenize..." &&
    time python3 tok.py Articles/ver3 > tokLog &&
    g++ -std=c++11 -O2 cpp/merge.cpp -o merge.out &&
    echo "Merge files..." &&
    time ./merge.out Index/Parts/Articles/ver3/*/* > mergeLog &&
    echo "Total"
    )