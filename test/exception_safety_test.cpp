#include <gtest/gtest.h>
#include <iostream>

#include "matrix.h"

static bool check_exc = false;

static int ctr_counter = 0;
static int copy_ctr_counter = 0;

static int entity_counter = 0;


class myint_error : public std::runtime_error {
public:
    myint_error(char const* const message) throw()
        : std::runtime_error(message) {};
};


class MyInt
{
    int val;

public:
    MyInt() : val(0) {
        if (check_exc) {
            ctr_counter++;
            if (ctr_counter % 5 == 0)
                throw myint_error{"ctr exception"};
        }

        entity_counter++;
    }

    MyInt(int val) : val(val) {
        if (check_exc) {
            ctr_counter++;
            if (ctr_counter % 5 == 0)
                throw myint_error{"ctr exception"};
        }

        entity_counter++;
    }

    MyInt(const MyInt& rhs) : val(rhs.val) {
        if (check_exc) {
            copy_ctr_counter++;
            if (copy_ctr_counter % 5 == 0)
                throw myint_error{"copy exception"};
        }

        entity_counter++;
    }

    operator double() const noexcept {
        return static_cast<double>(val);
    }

    ~MyInt() {
        entity_counter--;
    }
};


TEST(HwmxTests, Test_ES) {
    {
        ctr_counter = 0; copy_ctr_counter = 0; entity_counter = 0;
        check_exc = false;

        hwmx::Matrix<MyInt> m{2, 5};
        hwmx::Matrix<MyInt> m_tmp{2, 5};

        check_exc = true;
        try {
            hwmx::Matrix<MyInt> m2{2, 5};
        }
        catch (const myint_error& exc) {}

        try {
            hwmx::Matrix<MyInt> m2 = m;
        }
        catch (const myint_error& exc) {}

        try {
            m_tmp = m;
        }
        catch (const myint_error& exc) {}
    }

    ASSERT_EQ(entity_counter, 0);
}


TEST(HwmxTests, Test_ES_lazy) {
    {
        ctr_counter = 0; copy_ctr_counter = 0; entity_counter = 0;
        check_exc = false;

        hwmx::Matrix<MyInt, true> m{2, 5};
        hwmx::Matrix<MyInt, true> m_tmp{2, 5};

        check_exc = true;
        try {
            hwmx::Matrix<MyInt, true> m2{2, 5};
        }
        catch (const myint_error& exc) {}

        try {
            hwmx::Matrix<MyInt, true> m2 = m;
            m2[0][0] = 10;
        }
        catch (const myint_error& exc) {}

        try {
            m_tmp = m;
            m_tmp[0][0] = 10;
        }
        catch (const myint_error& exc) {}
    }

    ASSERT_EQ(entity_counter, 0);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}