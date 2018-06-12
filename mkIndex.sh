#!/bin/bash
time (
    echo "Tokenize..." &&
    # create Index/Parts/.../wiki..., Index/token_dict, Index/article_titles, Index/pre_stat
    time python3 tok.py Articles/ver3 > log/tok && 
    echo "Lemmatize..." &&
    # create Index/lemm.txt
    time ./mystem -cl Index/token_dict.txt > Index/lemm.txt &&
    echo "Build lemm table..." &&
    # create Index/lemm_token_dict.txt, Index/lemm_table
    time python3 buildLemmTable.py &&
    # change dict
    mv Index/lemm_token_dict.txt Index/token_dict.txt &&
    rm Index/lemm.txt &&
    echo "Renumber..." &&
    g++ -std=c++11 -O2 cpp/renumberToks.cpp -o renumber.out &&
    # create Index/Parts/.../niki...
    time ./renumber.out Index/Parts/Articles/ver3/*/* &&
    rm renumber.out Index/lemm_table &&
    g++ -std=c++11 -O2 cpp/merge.cpp -o merge.out &&
    echo "Merge files..." &&
    cp Index/pre_stat Index/stat &&
    # create Index/merge
    time ./merge.out Index/Parts/Articles/ver3/*/n* > log/merge &&
    rm merge.out &&
    echo "Total"
    )