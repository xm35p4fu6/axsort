#include <iostream>
#include <AXSORT/aligner/seeds.hpp>
#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>
#include <string>

using namespace std;

string input_read()
{
    return (axsort::test::data_dir() / "input_read").string();
}


// need to finish implementation
int32_t get_number_seed(string read, int32_t length, int32_t freq) {
	return 3;
}

TEST(ReadTest, CheckReadReverse)
{
	string reverse_mock, reverse;
	reverse_mock = "TACAGGCCTGGGTAAAATAAGGCTGAGAGCTACTGG";
	
    std::ifstream fin(input_read());
    string read;

    fin >> read;
	
	Seeds s(read);
	
	reverse = s.read_reverse(read);//read_reverse(read);

    EXPECT_EQ(reverse_mock, reverse);
}

TEST(ReadTest, CheckSeeds)
{
	int32_t seeds_number_mock, seeds_number;
	seeds_number_mock = 3;
	
    std::ifstream fin(input_read());
    string read;
	int32_t length, freq;

    fin >> read;
	fin >> length >> freq;
	
	seeds_number = get_number_seed(read, length, freq);

    EXPECT_EQ(seeds_number_mock, seeds_number);
}