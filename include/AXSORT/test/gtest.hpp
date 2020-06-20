#pragma once
#include <gtest/gtest.h>

void pre_process();

int main(int argc, char* argv[])
{
    pre_process();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
