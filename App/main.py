import os
import sys
sys.path.append('../')
import time
from tok import is_valid_symbol, normalize, lemmatize
from flask import Flask, request, render_template, send_from_directory

path_cpp_py = '/tmp/pipeCppPy'
path_py_cpp = '/tmp/pipePyCpp'

app = Flask(__name__)
article_titles = []
tok_to_int = {}
current_res = []
current_query = ''
use_engine = True

def parse_req(s):
    norm = lambda x:str(tok_to_int.get(normalize(x), 0))
    l = []
    cur = ''
    boolean = False
    i = 0
    while i < len(s):
        if is_valid_symbol(s[i]):
            cur += s[i]
        else:
            if cur:
                cur = norm(cur)
                l.append('.' + cur)
            cur = ''
            if s[i] in '&|':
                boolean = True
                if l and l[-1] != s[i]:
                    l.append(s[i])
            elif s[i] in '!()':
                boolean = True
                l.append(s[i])
            elif s[i] == '"':
                boolean = True
                tmp = ''
                cur = ''
                cnt = 0
                i += 1
                while i < len(s) and s[i] != '"':
                    if is_valid_symbol(s[i]):
                        cur += s[i]
                    elif cur:
                        tmp += '.' + norm(cur)
                        cur = ''
                        cnt += 1
                    i += 1
                if cur:
                    tmp += '.' + norm(cur)
                    cnt += 1
                cur = ''
                if i == len(s) or cnt == 0:
                    return ['&'], True
                i += 1
                tmp = '&' * (cnt - 1) + tmp
                sz = cnt
                if i < len(s) and s[i] == '/':
                    i += 1
                    num = 0
                    while i < len(s) and s[i].isdigit():
                        num = num * 10 + ord(s[i]) - ord('0')
                        i += 1
                    if num < cnt:
                        return ['&'], True
                    sz = num
                tmp = '"' + str(sz) + tmp
                l.append(tmp)
                i -= 1
            else:
                if l and l[-1] != ' ':
                    l.append(' ')
        i += 1
    if cur:
        cur = norm(cur)
        l.append('.' + cur)
    l2 = []
    i = 0
    was = False
    while i < len(l):
        if l[i] == ' ':
            if l2 and l2[-1] not in '&|!(' and i + 1 < len(l) and l[i + 1] not in ')&|':
                l2.append('&')
        else:
            l2.append(l[i])
        i += 1
    return l2, boolean


def prepare_query(l):
    #TODO: use Dijkstra )))
    l1 = []
    beg = -1
    bal = 0
    # del ()
    for i in range(len(l)):
        if l[i] == '(':
            if bal == 0:
                beg = i
            bal += 1
        elif l[i] == ')':
            bal -= 1
            if bal == 0:
                tmp = prepare_query(l[beg + 1:i])
                if tmp == '':
                    return ''
                l1.append(tmp)
            elif bal < 0:
                return ''
        elif bal == 0:
            l1.append(l[i])
    if bal:
        return ''
    # now only | & and !
    # del !
    l2 = []
    i = len(l1) - 1
    while i >= 0:
        if l1[i] == '!':
            if l2 and len(l2[-1]) > 1:
                new = '!' + l2[-1]
                del l2[-1]
                l2.append(new)
            else:
                return ''
        else:
            l2.append(l1[i])
        i -= 1
    l2 = l2[::-1]
    # now only & and |
    #del &
    l3 = []
    i = 0
    while i < len(l2):
        if l2[i] == '&':
            if l3 and l3[-1] != '|' and i + 1 < len(l2) and l2[i + 1] != '|':
                new = '&' + l3[-1] + l2[i + 1]
                del l3[-1]
                l3.append(new)
                i += 2
            else:
                return ''
        else:
            l3.append(l2[i])
            i += 1
    #then just sum all
    l4 = []
    i = 0
    while i < len(l3):
        if l3[i] == '|':
            if l4 and i + 1 < len(l3) and l3[i + 1] != '|':
                new = '|' + l4[-1] + l3[i + 1]
                del l4[-1]
                l4.append(new)
                i += 2
            else:
                return ''
        else:
            l4.append(l3[i])
            i += 1
    if len(l4) != 1:
        return ''
    return l4[0]
    #sorry for this, but it was easy to code


def get_response_from_engine():
    global current_res
    if use_engine:
        with open(path_cpp_py, 'r') as f:
            s = f.readline()
        current_res = [int(x) for x in s.split()]
    else:
        current_res = list(range(1, 101))
    # print('get "', s, '"')


def send_req_to_engine(req):
    if use_engine:
        print('send', req)
        with open(path_py_cpp, 'w') as f:
            f.write(req + '\n')


@app.route('/')
def index():
    return render_template("index.html")


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                          'favicon.ico', mimetype='image/vnd.microsoft.icon')


def get_title(path_to_file, lines_to_skip):
    with open(path_to_file, 'r') as f:
        for _ in range(lines_to_skip):
            next(f)
        s = next(f)
        l = s.split('"') # [id=, id, url=, url, title=, title]
        next(f)
        s = next(f)
        snippet = ''
        while not s.startswith('</doc>'):
            if not snippet:
                snippet = s.strip()
            s = next(f)
        print('snip =', snippet)
    return (l[5], snippet, int(l[1]))


@app.route('/search')
def search():
    global current_res, current_query
    start = time.time()
    query = request.args.get('query')
    print('raw query:', query)
    print(tok_to_int.get(normalize(query), 0))
    try:
        offset = int(request.args.get('offset', 0))
    except:
        return "Wrong offset value"
    if query is None:
        return "Wrong request"
    if len(query) > 500:
        return "Too long request"
    q_toks, boolean = parse_req(query)
    if len(q_toks) == 0:
        return 'Empty request'
    print('toks =', q_toks)
    prepared = str(int(boolean)) + prepare_query(q_toks)
    print('prep =', prepared)
    if len(prepared) == 1:
        return 'Wrong request format'
    print('User req', prepared)
    if prepared != current_query:
        current_query = prepared
        send_req_to_engine(prepared)
        get_response_from_engine()
    search_result = []
    for i in range(50):
        if offset + i >= len(current_res):
            break
        cur_str = article_titles[current_res[offset + i]]
        path_to_file = '../Articles/ver3/{}/wiki_{}'.format(cur_str[:2], 
                                                            cur_str[2:4])
        lines_to_skip = int(cur_str[4:])
        title, snippet, art_id = get_title(path_to_file, lines_to_skip)
        search_result.append((title, snippet, art_id))
    return render_template("serp.html", query=query, 
                                         search_result=search_result,
                                         total=len(current_res),
                                         offset=offset,
                                         tim=time.time() - start)


if __name__ == "__main__":
    if not os.path.exists(path_py_cpp):
        os.mkfifo(path_py_cpp)
    article_titles.append('notValidId')
    with open('../Index/article_titles.txt', 'r') as f:
        for line in f:
            # wid, title = line.split(' ', 1) 
            # article_titles.append((title, int(wid)))
            article_titles.append(line.strip())
    with open('../Index/token_dict.txt', 'r') as f:
        cur = 1
        for token in f:
            tok_to_int[token.strip()] = cur
            cur += 1
    print('READ ALL, START APP...')
    app.run(debug=True)
