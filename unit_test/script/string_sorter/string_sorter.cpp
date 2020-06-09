#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>
#include <AXSORT/string_sorter/sais.hpp>
#include <iostream>
#include <string_view>
#include <numeric>
#include <vector>
#include <iterator>
#include <iomanip>

std::string get_data(std::string filename)
{
    return (axsort::test::data_dir() / filename).string();
}

template<typename T>
void get_strings(T& strs)
{
    std::ifstream fin(get_data("string"));
    assert(fin.is_open());
    std::copy(std::istream_iterator<std::string>(fin), 
              std::istream_iterator<std::string>(),
              std::back_inserter(strs));
}

template<typename T1, typename T2>
void get_answers(T1 strs, T2& answers)
{
    answers.clear();
    std::for_each(strs.begin(), strs.end(), [&](auto s)
            {
                answers.emplace_back(s.length());
                auto& ans = answers.back();
                std::iota(ans.begin(), ans.end(), 0);
                std::sort(ans.begin(), ans.end(), [&](int a, int b)
                        {
                            return s.substr(a) < s.substr(b);
                        });
            });
}

template<typename T1, typename T2>
void print_answer(T1 strs, T2& answers)
{
    for(size_t i=0; i<strs.size(); ++i)
    {
        for(auto c : strs[i]) std::cout << std::setw(2) << c << " ";
        std::cout << "\n";
        for(auto c : answers[i]) std::cout << std::setw(2) << c << " ";
        std::cout << "\n";
    }
}

template<typename T1, typename T2>
void get_result(T1& strs, T2& ress)
{
    AXSORT::string_sorter::sais sorter;

    ress.resize(strs.size());
    for(size_t i=0; i<strs.size(); ++i)
    {
        ress[i].resize(strs[i].size());
        sorter.sort(strs[i], ress[i]);
    }
}

TEST(sais, check_answers)   // multi-testcase
{
    std::vector<std::string> strs;
    std::vector<std::vector<int>> anss, ress;

    get_strings(strs);
    std::vector<std::string_view> strs_view(strs.begin(), strs.end());

    get_answers(strs_view, anss);
    print_answer(strs_view, anss);

    get_result(strs, ress);
    
    EXPECT_EQ(ress, anss);
}
