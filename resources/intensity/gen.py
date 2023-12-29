from random import randint, random


def gen_edge(N):
    l, r = randint(1, N), randint(1, N)
    res, val = round(random() * 100, 3), round(random() * 1000, 3)

    # if (random() > 0.9):
    #     res = 0.0

    if (random() > 0.2):
        return f"{l} -- {r}, {res};"
    else:
        return f"{l} -- {r}, {res}; {val}V"


N, M = 100, 100
with open('gen_test.in', 'w') as f:
    str = "\n".join(gen_edge(N) for i in range(M))
    print(str, file=f)
    print(str)
    print()

import os
# res = os.popen("./intensity < ../resources/intensity/test2.in").read()
res = os.popen("./intensity < gen_test.in").read()

print(res)