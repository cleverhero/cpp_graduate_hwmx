#include <iostream>
#include <iterator>

#include "matrix.h"


using hwmx::Matrix;


int main() {
    size_t n;
    std::cin >> n;

    auto m = Matrix<double>{ 
        n, n, 
        std::istream_iterator<double>(std::cin),
        std::istream_iterator<double>() 
    };

	std::cout << m.det() << std::endl;

    return 0;
}