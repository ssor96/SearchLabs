from struct import pack

lemm_td = open('Index/lemm_token_dict.txt', 'w')
lemm_table = open('Index/lemm_table', 'wb')
lst = 1
with open('Index/lemm.txt', 'r') as lemm:
    w = {}
    cur = 1
    for l in lemm:
        l = l.strip()
        if l.startswith('{'):
            en = 1
            while s[en] not in "[?|}]":
                en += 1
            l = l[1:en]
        if l not in w:
            w[l] = lst
            lst += 1
            lemm_td.write(l + '\n')
        lemm_table.write(pack('<I', w[l]))

lemm_td.close()
lemm_table.close()