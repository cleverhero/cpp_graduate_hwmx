#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <vector>

#include "matrix.h"
#include "matrix_chain.h"
#include "config.h"


using std::filesystem::path;
using Config::RESOURCES_PATH;

using lli = long long int;

template<typename T>
using MatrixUP = std::unique_ptr<hwmx::Matrix<T>>;


TEST(HwmxTests, Test_Test) {
    path resources(RESOURCES_PATH);
    std::fstream testf, answerf;
    testf.open(resources / "matrix_chain/test.in", std::ios::in);
    answerf.open(resources / "matrix_chain/test.out", std::ios::in);

     if (!testf || !answerf) {
        std::cout << "NO SUCH FILE!!" << std::endl;
        ASSERT_TRUE(false);
    }

    size_t count; testf >> count;
    hwmx::MatrixChain<lli> chain{};
    std::vector<MatrixUP<lli>> matrices;

    for (int i = 0; i < count; i++) {
        int rows, cols;
        testf >> rows >> cols;
        matrices.push_back(
            std::make_unique<hwmx::Matrix<lli>>(rows, cols, std::istream_iterator<lli>(testf))
        );
        chain.add(&*matrices[i]);
    }

    auto res = chain.optimal_multiply();
    
    int rows, cols;
    answerf >> rows >> cols;

    ASSERT_TRUE(rows == res.rows());
    ASSERT_TRUE(cols == res.cols());

    ASSERT_TRUE(
        std::mismatch(res.cbegin(), res.cend(), std::istream_iterator<lli>(answerf)).first == res.cend()
    );


    testf.close();
    answerf.close();
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}