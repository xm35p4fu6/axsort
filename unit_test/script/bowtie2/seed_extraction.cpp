#include <iostream>
#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>
#include <string>

using namespace std;

string input_read()
{
    return (axsort::test::data_dir() / "input_read").string();
}

string read_reverse(string read) {
    reverse(read.begin(), read.end());
    for(uint i = 0; i < read.length(); i++) {
        switch (read[i]) {
            case 'A':
                read[i] = 'T';
                break;
            case 'C':
                read[i] = 'G';
                break;
            case 'G':
                read[i] = 'C';
                break;
            case 'T':
                read[i] = 'A';
                break;
            default:
                cout << "Only AGCT is supported!" << endl;
        }
    }
    return read;
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
	
	reverse = read_reverse(read);

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