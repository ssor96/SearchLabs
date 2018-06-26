import sys
import os
from collections import defaultdict
from pathlib import Path
from struct import pack
import sys
sys.path.append('../')
import tok

token_count = defaultdict(int)
fc = defaultdict(int)
sc = defaultdict(int)


def parse_file(file_name, output_file_name):
    file_in = open(file_name, 'r')
    file_out = open(output_file_name, 'wb+')

    for s in file_in:
        if s.startswith('<doc '):
            prev = 0
        elif s.startswith('</doc>'):
            pass
        else:
            for tok_id in tok.parse_str(s):
                token_count[tok_id] += 1
                if prev:
                    fc[prev] += 1
                    sc[tok_id] += 1
                    file_out.write(pack('<II', prev, tok_id))
                prev = tok_id
    file_in.close()
    file_out.close()


def parse_dir(dir_name, out_name):
    if not os.path.isdir(dir_name):
        parse_file(dir_name, out_name)
        return
    path = Path(out_name)
    path.mkdir(parents=True, exist_ok=True)
    for name in os.listdir(dir_name):
        child_name = os.path.join(dir_name, name)
        child_out_name = os.path.join(out_name, name)
        parse_dir(child_name, child_out_name)


if __name__ == '__main__':
    parse_dir(sys.argv[1], 'Count/Parts')
    f2 = open('stat/col_stat', 'wb')
    with open('stat/pairs_dict.txt', 'w') as f:
        for token, tid in sorted(tok.tok_to_int.items(), key=lambda x:x[1]):
            f.write(token + '\n')
            f2.write(pack('<III', token_count[tid], fc[tid], sc[tid]))
    f2.close()
