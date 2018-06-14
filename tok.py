import os
import re
import sys
import time
import subprocess as sub
from struct import pack
from pathlib import Path
from collections import defaultdict

# d = defaultdict(int)
tok_to_int = dict()
current_doc_id = 0

def memorize_and_count(f):
    w = dict()
    def wrapper(s):
        if s not in w:
            w[s] = f(s)
        # d[w[s]] += 1
        return w[s]
    return wrapper


#@memorize_and_count
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

# @memorize_and_count
def normalize(token):
    return token.lower()


def get_id(s):
    if s not in tok_to_int:
        tok_to_int[s] = len(tok_to_int) + 1
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
    tok_pos = 0
    line = 0
    doc = []
    doc_feature = file_name[-10:-8] + file_name[-2:] # AA/wiki_00
    for s in file_in:
        if s.startswith('<doc '):
            tok_pos = 0
            l = s.split('"') # [id=, id, url=, url, title=, title]
            article_titles.write(doc_feature + str(line) + '\n')
        elif s.startswith('</doc>'):
            current_doc_id += 1
        else:
            cur_toks = parse_str(s)
            for tok in cur_toks:
                doc.append((tok, current_doc_id, tok_pos))
                tok_pos += 1
        line += 1
    file_in.close()
    file_out = open(output_file_name, 'wb')
    for tok_id, doc_id, tok_pos in sorted(doc):
        file_out.write(pack('<III', tok_id, doc_id, tok_pos))
    file_out.close()


def parse_dir(dir_name):
    path = Path(os.path.join('Index/Parts', dir_name))
    path.parent.mkdir(parents=True, exist_ok=True)
    for name in sorted(os.listdir(dir_name)):
        child_name = os.path.join(dir_name, name)
        if os.path.isdir(child_name):
            parse_dir(child_name)
        else:
            parse_file(child_name)
            print(child_name)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write("tok.py: Wrong format. Path to articles expected\n")
        exit(1)
    global article_titles
    article_titles = open('Index/article_titles.txt', 'w')
    parse_dir(sys.argv[1])
    article_titles.close()
    start = time.time()
    cpu_start = time.clock()
    token_dict = open('Index/token_dict.txt', 'w')
    for tok, _ in sorted(tok_to_int.items(), key = lambda x:x[1]):
        token_dict.write(tok + '\n')
    token_dict.close()
    stat = open('Index/pre_stat', 'wb')
    stat.write(pack('<I', current_doc_id))
    stat.close()
    mid = time.time()
    cpu_mid = time.clock()
    print(mid - start, cpu_mid - cpu_start)