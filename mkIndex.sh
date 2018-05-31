#!/bin/bash
time (
    echo "Tokenize..." &&
    time python3 tok.py Articles/ver3 > log/tok&&
    g++ -std=c++11 -O2 cpp/merge.cpp -o merge.out &&
    echo "Merge files..." &&
    cp Index/pre_stat Index/stat &&
    time ./merge.out Index/Parts/Articles/ver3/*/* > log/merge &&
    rm merge.out &&
    echo "Total"
    )