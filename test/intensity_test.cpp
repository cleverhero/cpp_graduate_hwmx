#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "matrix.h"
#include "electrical_circuit.h"
#include "config.h"


using std::filesystem::path;
using Config::RESOURCES_PATH;


void test(std::string test_name) {
    path resources(RESOURCES_PATH);

    std::fstream testf, answerf;
    testf.open(resources / "intensity" / (test_name + ".in"), std::ios::in);
    answerf.open(resources / "intensity" / (test_name + ".out"), std::ios::in);

    if (!testf || !answerf) {
        std::cout << "NO SUCH FILE!!" << std::endl;
        ASSERT_TRUE(false);
    }

    std::stringstream result;
    hwmx::find_intensity_list(testf, result);

    ASSERT_TRUE(
        std::mismatch(
            std::istream_iterator<char>(answerf),
            std::istream_iterator<char>{}, 
            std::istream_iterator<char>(result)
        ).first == std::istream_iterator<char>{}
    );

    testf.close();
    answerf.close();
}


TEST(HwmxTests, Test_All) {
    size_t TEST_COUNT = 5;

    for (int i = 1; i <= TEST_COUNT; i++) {
        test("test" + std::to_string(i));
        std::cout << i << " passed;" << std::endl;
    }
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}