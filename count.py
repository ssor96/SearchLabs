import sys
import os
from collections import defaultdict
from pathlib import Path
from struct import pack
import tok

token_count = defaultdict(int)
fc = defaultdict(int)
sc = defaultdict(int)

def parse_file(file_name):
    file_in = open(file_name, 'r')
    tuples = []
    for s in file_in:
        if s.startswith('<doc '):
            pass
        elif s.startswith('</doc>'):
            pass
        else:
            prev = ''
            not_first = False
            for w in tok.parse_str(s):
                token_count[w] += 1
                if not_first:
                    fc[prev] += 1
                    sc[w] += 1
                    tuples.append((prev, w))
                prev = w
                not_first = True
    file_in.close()
    output_file_name = os.path.join('Count/Parts', file_name)
    # path = Path(os.path.join('Count/Parts', file_name))
    # path.parent.mkdir(parents=True, exist_ok=True)
    tuples.sort()
    with open(output_file_name, 'wb') as out:
        pa, pb = tuples[0]
        cc = 0
        for a, b in tuples:
            if a == pa and b == pb:
                cc += 1
            else:
                # print(pa, pb, cc)
                if a < pa:
                    print(a, b, '!!!')
                    exit()
                out.write(pack('<III', pa, pb, cc))
                pa, pb, cc = a, b, 1
            # print(a, b)
        out.write(pack('<III', pa, pb, cc))
    # print()


def parse_dir(dir_name):
    # path = Path(os.path.join('Count/Parts', dir_name))
    # path.parent.mkdir(parents=True, exist_ok=True)
    for name in sorted(os.listdir(dir_name)):
        child_name = os.path.join(dir_name, name)
        if os.path.isdir(child_name):
            parse_dir(child_name)
        else:
            parse_file(child_name)
            print(child_name)


if __name__ == '__main__':
    parse_dir(sys.argv[1])
    f2 = open('col_stat', 'wb')
    with open('pairs_dict.txt', 'w') as f:
        for token, tid in sorted(tok.tok_to_int.items(), key = lambda x:x[1]):
            f.write(token + '\n')
            f2.write(pack('<III', token_count[tid], fc[tid], sc[tid]))
    f2.close()