import os
import re
import sys
import time
from struct import pack
from pathlib import Path
from collections import defaultdict
# from nltk.stem.snowball import SnowballStemmer
# from nltk.tokenize.moses import MosesTokenizer

# stem = SnowballStemmer('russian')
# tokenizer = MosesTokenizer('ru')

# d = defaultdict(int)
tok_to_int = dict()
current_doc_id = 0
df = defaultdict(int)
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


def get_path_to_text_file(doc_id):
    str_doc_id = str(doc_id)
    str_doc_id = '0' * (7 - len(str_doc_id)) + str_doc_id
    path_to_text_file = os.path.join(str_doc_id[:2],
                                     str_doc_id[2:4],
                                     str_doc_id[4:])
    return path_to_text_file


def parse_file(file_name):
    if file_name.endswith('parsed'): return
    global current_doc_id
    file_in = open(file_name, 'r')
    output_file_name = os.path.join('Index/Parts', file_name)
    tokens = set()
    tf = defaultdict(int)
    doc = []
    small_index = []
    tok_pos = 0
    for s in file_in:
        if s.startswith('<doc '):
            path_to_article = get_path_to_text_file(current_doc_id)
            if path_to_article.endswith('000'):
                path = Path(os.path.join('Index/Text', path_to_article))
                path.parent.mkdir(parents=True, exist_ok=True)
                path = Path(os.path.join('Index/Small_Index', path_to_article))
                path.parent.mkdir(parents=True, exist_ok=True)
            article_text = open(os.path.join('Index/Text', 
                                             path_to_article), 'w')
            article_index = open(os.path.join('Index/Small_Index', 
                                             path_to_article), 'wb')
            small_index = []
            tok_pos = 0
            tf = defaultdict(int)
            l = s.split('"') # [id=, id, url=, url, title=, title]
            article_text.write(l[5] + '\n')
            cur_toks = parse_str(l[5])
            for tok in cur_toks:
                tf[tok] += 1
                small_index.append((tok, tok_pos))
                tok_pos += 1
            tokens.update(cur_toks)
            article_titles.write(l[1] + ' ' + l[5] + '\n')
        elif s.startswith('</doc>'):
            for tok in tokens:
                df[tok] += 1
                doc.append((tok, current_doc_id, tf[tok]))
            tokens = set()
            current_doc_id += 1
            article_text.close()
            prev_tok = 0
            for tok, pos in sorted(small_index):
                if tok != prev_tok:
                    prev_tok = tok
                    article_index.write(pack('<II', tok, tf[tok]))
                article_index.write(pack('<I', pos))
            article_index.close()
        else:
            article_text.write(s)
            cur_toks = parse_str(s)
            for tok in cur_toks:
                tf[tok] += 1
                small_index.append((tok, tok_pos))
                tok_pos += 1
            tokens.update(cur_toks)
    file_in.close()
    file_out = open(output_file_name, 'wb')
    for tok_id, doc_id, tf in sorted(doc):
        file_out.write(pack('<III', tok_id, doc_id, tf))
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
    stat = open('Index/pre_stat', 'wb')
    stat.write(pack('<I', current_doc_id))
    for tok, _ in sorted(tok_to_int.items(), key = lambda x:x[1]):
        token_dict.write(tok + '\n')
        stat.write(pack('<I', df[tok]))
    token_dict.close()
    stat.close()
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