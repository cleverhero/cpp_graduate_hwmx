import numpy as np

dims = [
    [3, 5], [5, 60], [60, 4], [4, 3], [3, 50], [50, 2], [2, 3], [3, 6],
    [6, 5], [5, 60], [60, 4], [4, 3], [3, 50], [50, 2], [2, 3], [3, 12],
    [12, 5], [5, 60], [60, 4], [4, 30], [30, 50], [50, 2], [2, 3], [3, 4]
]


ftest = open('./test.in', 'w')
fans = open('./test.out', 'w')


print(len(dims), file=ftest)


result = None
for rows, cols in dims:
    mx = np.random.randint(-1, 1, size=(rows, cols))
    print(rows, cols, file=ftest, end=' ')
    print(' '.join(str(el) for el in mx.flatten()), file=ftest)

    if result is None:
        result = mx
    else:
        result = np.matmul(result, mx)


res_rows, res_cols = result.shape
print(res_rows, res_cols, file=fans, end=' ')
print(' '.join(str(el) for el in result.flatten()), file=fans)


ftest.close()
fans.close()
