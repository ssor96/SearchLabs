import os
import re
import sys
import subprocess as sub
from struct import pack
# from pathlib import Path
import unicodedata

tok_to_int = dict()
current_doc_id = 1


def lemmatize(s):
    p = sub.Popen(['../mystem', '-cl'], 
                  stdin=sub.PIPE, 
                  stdout=sub.PIPE)

    p.stdin.write(bytes(s, encoding='utf8'))
    p.stdin.close()

    res = p.stdout.read().decode('utf8')

    if res.startswith('{'):
        en = re.search("[?|}]", res, 1).start(0)
        res = res[1:en]

    return res


def strip_accents(s):
    return ''.join(c for c in unicodedata.normalize('NFD', s)
                    if unicodedata.category(c) != 'Mn')


def normalize(token):
    return strip_accents(token).lower()


lst_tok_id = 1


def get_id(s):
    global lst_tok_id
    if s not in tok_to_int:
        tok_to_int[s] = lst_tok_id
        lst_tok_id += 1
    return tok_to_int[s]


def is_valid_symbol(c):
    return c.isalpha() or c.isdigit()


def parse_str(s):
    res = []
    prev = 0
    cur = -1
    for c in s:
        cur += 1
        if not is_valid_symbol(c):
            if prev != cur:
                res.append(get_id(normalize(s[prev:cur])))
            prev = cur + 1
    if prev <= cur:
        res.append(get_id(normalize(s[prev:cur + 1])))
    return res


def parse_file(file_name):
    global current_doc_id
    file_in = open(file_name, 'r')
    output_file_name = os.path.join('Index/Parts', file_name)
    tok_pos = 1
    line = 0
    doc_feature = file_name[-10:-8] + file_name[-2:] # AA/wiki_00
    file_out = open(output_file_name, 'wb')
    for s in file_in:
        if s.startswith('<doc '):
            tok_pos = 1
            l = s.split('"') # [id=, id, url=, url, title=, title]
            for tok_id in parse_str(l[-2]) + [0]:
                titles_toks.write(pack('<I', tok_id))
            article_titles.write(doc_feature + str(line) + '\n')
        elif s.startswith('</doc>'):
            current_doc_id += 1
        else:
            for tok_id in parse_str(s):
                file_out.write(pack('<III', tok_id, current_doc_id, tok_pos))
                tok_pos += 1
        line += 1
    file_in.close()
    file_out.close()


def parse_dir(dir_name):
    # path = Path(os.path.join('Index/Parts', dir_name))
    # path.mkdir(parents=True, exist_ok=True)
    for name in os.listdir(dir_name):
        child_name = os.path.join(dir_name, name)
        if os.path.isdir(child_name):
            parse_dir(child_name)
        else:
            parse_file(child_name)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("tok.py: Wrong format. Path to articles expected\n")
        exit(1)
    global article_titles
    article_titles = open('Index/article_titles.txt', 'w')
    global titles_toks
    titles_toks = open('Index/pre_titles_toks', 'wb')
    parse_dir(sys.argv[1])
    article_titles.close()
    token_dict = open('Index/pre_token_dict.txt', 'w')
    for tok, _ in sorted(tok_to_int.items(), key = lambda x:x[1]):
        token_dict.write(tok + '\n')
    token_dict.close()
    stat = open('Index/pre_stat', 'wb')
    stat.write(pack('<I', current_doc_id - 1))
    stat.close()