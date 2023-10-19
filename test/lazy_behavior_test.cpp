#include <gtest/gtest.h>
#include <iostream>

#include "matrix.h"


static int entity_counter = 0;


class MyInt
{
    int val;

public:
    MyInt() : val(0) {
        entity_counter++;
    }

    MyInt(int val) : val(val) {
        entity_counter++;
    }

    MyInt(const MyInt& rhs) : val(rhs.val) {
        entity_counter++;
    }

    operator double() const noexcept {
        return static_cast<double>(val);
    }

    ~MyInt() {
        entity_counter--;
    }
};


TEST(HwmxTests, Test_lazy_behavior) {
    {
        entity_counter = 0;

        hwmx::Matrix<MyInt, true> m{2, 5};
        ASSERT_EQ(entity_counter, 10);
        
        hwmx::Matrix<MyInt, true> m2{m};
        ASSERT_EQ(entity_counter, 10);

        m2[0][0] = 10;
        ASSERT_EQ(entity_counter, 20);
    }

    ASSERT_EQ(entity_counter, 0);
}

TEST(HwmxTests, Test_lazy_behavior2) {
    {
        entity_counter = 0;

        hwmx::Matrix<MyInt, true> m{2, 5};
        ASSERT_EQ(entity_counter, 10);
        
        hwmx::Matrix<MyInt, true> m2{m};
        ASSERT_EQ(entity_counter, 10);

        m[0][0] = 10;
        ASSERT_EQ(m[0][0], 10);
        ASSERT_EQ(m2[0][0], 0);
    }

    ASSERT_EQ(entity_counter, 0);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}