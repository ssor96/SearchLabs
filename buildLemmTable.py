from struct import pack
import re

lemm_td = open('Index/lemm_token_dict.txt', 'w')
lemm_table = open('Index/lemm_table', 'wb')
with open('Index/lemm.txt', 'r') as lemm:
    w = {}
    cur = 1
    for l in lemm:
        l = l.strip()
        if l.startswith('{'):
            en = re.search("[?|}]", l, 1).start(0)
            l = l[1:en]
        if l not in w:
            w[l] = len(w) + 1
            lemm_td.write(l + '\n')
        lemm_table.write(pack('<I', w[l]))

lemm_td.close()
lemm_table.close()