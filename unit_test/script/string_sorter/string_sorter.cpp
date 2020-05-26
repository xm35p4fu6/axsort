#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>
#include <AXSORT/string_sorter/sais.hpp>
#include <iostream>
#include <string_view>
#include <numeric>

std::string get_data(std::string filename)
{
    return (axsort::test::data_dir() / filename).string();
}

void get_string_data(std::string& s)
{
    std::ifstream fin(get_data("string0"));
    assert(fin.is_open());
    fin >> s;
}

void get_answer(std::string_view s, std::vector<int>& sa)
{
    sa.resize(s.length());
    std::iota(sa.begin(), sa.end(), 0);
    std::sort(sa.begin(), sa.end(), [&](int a, int b)
        {
           return s.substr(a) < s.substr(b); 
        });
}

void print_answer(std::string_view s, const std::vector<int>& v)
{
    std::cout << s << std::endl;
    for(int i : v) std::cout << i << " " << s.substr(i) << "\n";
}

template<typename SEQ, typename RES>
void get_result(SEQ&& str, RES&& res)
{
    AXSORT::string_sorter::sais sorter;
    sorter.sort(str, res);
}

TEST(sais, check_answer)
{
    std::string str;
    std::vector<int> sa, res;

    get_string_data(str);

    res.resize(sa.size()); // allocate memory for sais
    get_answer(str, sa);
    //print_answer(str, sa);

    get_result(str, res);
    
    EXPECT_EQ(res, sa);
}
