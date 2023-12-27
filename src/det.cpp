#include <iostream>
#include <iterator>

#include "matrix.h"
#include "algorithm.h"


using hwmx::Matrix;


int main() {
    size_t n;
    std::cin >> n;

    auto m = Matrix<double>{ 
        n, n, 
        std::istream_iterator<double>(std::cin)
    };

	std::cout << hwmx::det(m) << std::endl;

    return 0;
}