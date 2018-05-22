import os
import sys
sys.path.append('../')
from tok import is_valid_symbol, normalize
from flask import Flask, request, render_template, send_from_directory

app = Flask(__name__)
article_titles = []
tok_to_int = {}

def parse_req(s):
    l = []
    cur = ''
    boolean = False
    for c in s:
        if is_valid_symbol(c):
            cur += c
        else:
            if cur:
                cur = normalize(cur)
                cur = tok_to_int.get(cur, 0)
                l.append('.' + str(cur))
            cur = ''
            if c in '&|':
                boolean = True
                if l and l[-1] != c:
                    l.append(c)
            elif c in '!()':
                boolean = True
                l.append(c)
            elif c == ' ':
                if l and l[-1] != ' ':
                    l.append(' ')
    if cur:
        cur = normalize(cur)
        cur = tok_to_int.get(cur, 0)
        l.append('.' + str(cur))
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
    i = 0
    while i < len(l1):
        if l1[i] == '!':
            if i + 1 < len(l1) and len(l1[i + 1]) > 1:
                l2.append('!' + l1[i + 1])
                i += 2
            else:
                return ''
        else:
            l2.append(l1[i])
            i += 1
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
    return list(range(1, 51))


def send_req_to_engine(req):
    pass

@app.route('/')
def index():
    return render_template("index.html")


@app.route('/favicon.ico')
def favicon():
    return send_from_directory(os.path.join(app.root_path, 'static'),
                          'favicon.ico', mimetype='image/vnd.microsoft.icon')


@app.route('/search')
def search():
    query = request.args.get('query')
    if query is None:
        return "Wrong request"
    if len(query) > 500:
        return "Too long request"
    q_toks, boolean = parse_req(query)
    if len(q_toks) == 0:
        return 'Empty request'

    prepared = prepare_query(q_toks)
    if not prepared:
        return 'Wrong request format'
    send_req_to_engine(prepared)
    res = get_response_from_engine()
    search_result = []
    for i in range(50):
        if i >= len(res):
            break
        search_result.append(article_titles[res[i] - 1])
    return render_template("search.html", query=query, 
                                         search_result=search_result)


if __name__ == "__main__":
    with open('../Index/article_titles.txt', 'r') as f:
        for line in f:
            wid, title = line.split(' ', 1) 
            article_titles.append((title, int(wid)))
    with open('../Index/token_dict.txt', 'r') as f:
        cur = 1
        for token in f:
            tok_to_int[token] = cur
            cur += 1
    print('READ ALL, START APP...')
    app.run(debug=True)