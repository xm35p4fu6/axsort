#include <iostream>
#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>

std::string sample_data()
{
    return (axsort::test::data_dir() / "sample_data").string();
}

TEST(TestClassName, TestName)
{
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;

    EXPECT_EQ(a, b);
}

TEST(TestClassName, TestName2)
{
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;

    EXPECT_EQ(a, b);
}

TEST(TestClassName2, TestName)
{
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;

    EXPECT_EQ(a, b);
}

TEST(TestClassName2, TestName2)
{
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;

    EXPECT_EQ(a, b);
}
TEST(TestClassName2, TestName_FAIL)
{
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;
    ++b;

    EXPECT_EQ(a, b);
}
