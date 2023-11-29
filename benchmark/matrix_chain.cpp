#include <ctime>
#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

#include "matrix.h"
#include "matrix_chain.h"


template<typename T>
using MatrixUP = std::unique_ptr<hwmx::Matrix<T>>;


int main(int argc, char** argv) {
    int N = 20, K = 1, MAX_SIZE = 500;
    if (argc > 1)
        N = atoi(argv[1]);
    if (argc > 2)
        K = atoi(argv[2]);

    std::srand(unsigned(std::time(nullptr)));
    std::vector<int> dims(N + 1);
    std::generate(dims.begin(), dims.end(), [MAX_SIZE]() { return std::rand() % MAX_SIZE; });

    hwmx::MatrixChain<int> chain{};
    std::vector<MatrixUP<int>> matrices;

    for (int i = 0; i < N; i++) {
        matrices.push_back(
            std::make_unique<hwmx::Matrix<int>>(dims[i], dims[i + 1], 1)
        );
        chain.add(&*matrices[i]);
    }

    double t0 = clock();

    for (int i = 0; i < K; i++)
        chain.sequential_multiply();

    double dt = clock() - t0;
    std::cout << "Sequential multiply: " << dt / CLOCKS_PER_SEC << std::endl;

    t0 = clock();

    for (int i = 0; i < K; i++)
        chain.optimal_multiply();

    dt = clock() - t0;
    std::cout << "Optimal multiply: " << dt / CLOCKS_PER_SEC << std::endl;
}