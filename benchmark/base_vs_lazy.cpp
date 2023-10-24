#include <ctime>
#include <iostream>
#include <algorithm>

#include "matrix.h"


bool random_bool() { return rand() > (RAND_MAX / 2); }


template<typename M>
M::elements_type random_change_and_det(M matrix) {
    if (random_bool())
        matrix[0][0] = 10;

    return std::reduce(matrix.cbegin(), matrix.cend());
}


template<typename M>
float benchmark(int n, int k) {
    M matrix = M::eye(n);

    auto t0 = clock();

    for (int i = 0; i < k; i++)
        random_change_and_det(matrix);

    return clock() - t0;
}


int main(int argc, char** argv) {
    int N = 200, K = 500;
    if (argc > 1)
        N = atoi(argv[1]);
    if (argc > 2)
        K = atoi(argv[2]);

    auto dt = benchmark<hwmx::Matrix<int>>(N, K);
    std::cout << "NonLazy matrix: " << dt / CLOCKS_PER_SEC << std::endl;

    auto lazy_dt = benchmark<hwmx::LazyMatrix<int>>(N, K);
    std::cout << "Lazy matrix: " << lazy_dt / CLOCKS_PER_SEC << std::endl;
}