import random
import sys


def print_m(matrix):
    for line in matrix:
        for el in line:
            print(el, end=' ')
        print(end=' ')


def init_matrix(size, det=42):
    assert size > 0

    matrix = [[0] * size for i in range(size)]
    matrix[0][0] = det
    for i in range(1, size):
        matrix[i][i] = 1

    return matrix


def random_ops(matrix):
    size = len(matrix)
    if size == 1:
        return

    x = random.randint(0, size - 1)
    y = random.randint(0, size - 1)
    if (x == y):
        y = x - 1 if x > 0 else x + 1

    if bool(random.getrandbits(1)):
        mult = random.randint(1, 3)
    else:
        mult = round(random.random(), 3)

    if bool(random.getrandbits(1)):
        mult *= -1

    for i in range(size):
        matrix[x][i] += matrix[y][i] * mult


def generate_test(size, det=42):
    matrix = init_matrix(size, det)
    
    for i in range(10*size):
        random_ops(matrix)

    return matrix
    
size, det = int(sys.argv[1]), int(sys.argv[2])
print(size, end=' ')
print_m(generate_test(size, det))