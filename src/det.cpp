#include <iostream>
#include <iterator>

#include "matrix.h"


using hwmx::Matrix;


int main() {
    // size_t n;
    // std::cin >> n;

    // auto m = Matrix<double>{ 
    //     n, n, 
    //     std::istream_iterator<double>(std::cin),
    //     std::istream_iterator<double>() 
    // };

	// std::cout << m.det() << std::endl;


    size_t n = 5;
    const auto m = Matrix<double>::eye(n);

    // auto i = m.begin(); // ERROR
    auto ci = m.cbegin();
    // *ci = 5; // ERROR

    auto m2 = Matrix<double>::eye(n);

    auto i = m2.begin();
    *i = 5;

    m2.print();


    // auto r = m[2];
    

    // m.print();
    

    // auto m2 = m;
    // m2[0][1] = 100;

    // std::cout << std::endl;
    // m.print();


    return 0;
}