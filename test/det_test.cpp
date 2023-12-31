#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "hwmx.h"
#include "matrix.h"
#include "algorithm.h"
#include "config.h"


using std::filesystem::path;
using Config::RESOURCES_PATH;


TEST(HwmxTests, Test_Big) {
    path resources(RESOURCES_PATH);
    std::fstream testf, answerf;
    testf.open(resources / "det/big_test.in", std::ios::in);
    answerf.open(resources /"det/big_test.out", std::ios::in);

    if (!testf || !answerf) {
        std::cout << "NO SUCH FILE!!" << std::endl;
        ASSERT_TRUE(false);
    }

    size_t size; testf >> size;
    double ans; answerf >> ans;
    auto m = hwmx::Matrix<double>{ size, size, std::istream_iterator<double>(testf) };

    ASSERT_FLOAT_EQ(hwmx::det(m), ans);

    testf.close();
    answerf.close();
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}