import os
import re
import sys
import time
from pathlib import Path
from collections import defaultdict
# from nltk.stem.snowball import SnowballStemmer
# from nltk.tokenize.moses import MosesTokenizer

# stem = SnowballStemmer('russian')
# tokenizer = MosesTokenizer('ru')

# d = defaultdict(int)
tok_to_int = dict()
current_doc_id = 1
#article_titles = open('Index/article_titles.txt', 'w')

def memorize_and_count(f):
    w = dict()
    def wrapper(s):
        if s not in w:
            w[s] = f(s)
        # d[w[s]] += 1
        return w[s]
    return wrapper


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
    if prev < cur:
        res.append(get_id(normalize(s[prev:cur])))
    return res


def mkstr(a):
    return bytes([(a >> (8 * i)) & 255 for i in range(4)])


def parse_file(file_name):
    if file_name.endswith('parsed'): return
    global current_doc_id
    file_in = open(file_name, 'r')
    output_file_name = os.path.join('Index/Parts', file_name)
    path = Path(output_file_name)
    path.parent.mkdir(parents=True, exist_ok=True)
    tokens = set()
    doc = []
    for s in file_in:
        if s.startswith('<doc '):
            l = s.split('"') # [id=, id, url=, url, title=, title]
            tokens.update(parse_str(l[5]))
            article_titles.write(l[1] + ' ' + l[5] + '\n')
        elif s.startswith('</doc>'):
            doc += [(tok, current_doc_id) for tok in tokens]
            tokens = set()
            current_doc_id += 1
        else:
            tokens.update(parse_str(s))
    file_in.close()
    file_out = open(output_file_name, 'wb')
    for tokId, docId in sorted(doc):
        file_out.write(mkstr(tokId) + mkstr(docId))
    file_out.close()


def parse_dir(dir_name):
    for name in sorted(os.listdir(dir_name)):
        child_name = os.path.join(dir_name, name)
        if os.path.isdir(child_name):
            parse_dir(child_name)
        else:
            parse_file(child_name)
            # print(child_name)


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
    for tok, idx in sorted(tok_to_int.items(), key = lambda x:x[1]):
        token_dict.write(tok + '\n')
    token_dict.close()
    mid = time.time()
    cpu_mid = time.clock()
    print(mid - start, cpu_mid - cpu_start)
    # s = 0
    # cnt = 0
    # for t in d.items():
    #     cnt += t[1]
    #     s += len(t[0]) * t[1]
    # print(time.time() - mid, time.clock() - cpu_mid)
    # print(time.time() - start, time.clock() - cpu_start)
    print('did =', current_doc_id)