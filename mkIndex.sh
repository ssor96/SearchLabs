#!/bin/bash
time (
    echo "Tokenize..." &&
    # create Index/Parts/.../wiki..., Index/pre_token_dict, Index/article_titles, Index/pre_stat
    time python3 tok.py Articles/ver3 > log/tok && 
    echo "Lemmatize..." &&
    # create Index/lemm.txt
    time ./mystem -cl Index/pre_token_dict.txt > Index/lemm.txt &&
    # cp Index/pre_token_dict.txt Index/lemm.txt && # this turns off lemmatization
    echo "Build lemm table..." &&
    # create Index/lemm_token_dict.txt, Index/lemm_table
    time python3 buildLemmTable.py &&
    echo "Renumber..." &&
    make renumber &&
    # create Index/Parts/.../niki...
    time ./renumber.out Index/Parts/Articles/ver3/*/w* &&
    rm renumber.out &&
    make merge &&
    echo "Merge files..." &&
    cp Index/pre_stat Index/stat &&
    # create Index/merge
    time ./merge.out Index/Parts/Articles/ver3/*/n* > log/merge &&
    rm merge.out &&
    echo "Total"
    )