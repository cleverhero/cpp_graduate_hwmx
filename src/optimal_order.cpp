#include <iostream>
#include <iterator>
#include <vector>
#include <memory>

#include "matrix.h"
#include "matrix_chain.h"


using hwmx::Matrix;

template<typename T>
using MatrixUP = std::unique_ptr<hwmx::Matrix<T>>;


int main() {
    size_t n; std::cin >> n;

    std::vector<int> dims(n);
    for (int i = 0; i < n; i++) std::cin >> dims[i];
    hwmx::MatrixChain<int> chain{};
    std::vector<MatrixUP<int>> matrices;

    for (int i = 0; i < n - 1; i++) {
        matrices.push_back(
            std::make_unique<hwmx::Matrix<int>>(dims[i], dims[i + 1])
        );
        chain.add(&*matrices[i]);
    }
    
    chain.print_order();
    return 0;
}