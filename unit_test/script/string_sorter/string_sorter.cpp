#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/test/gtest.hpp>
#include <AXSORT/string_sorter/sais.hpp>
#include <iostream>
#include <string_view>
#include <numeric>
#include <vector>
#include <iterator>
#include <iomanip>
#include <gperftools/profiler.h>

std::string reference_str;

std::string get_data(std::string filename)
{
    return (axsort::test::data_dir() / filename).string();
}

template<typename T>
void get_strings(T& strs)
{
    {
        std::ifstream fin(get_data("chr.txt"));
        assert(fin.is_open());
        std::copy(std::istream_iterator<std::string>(fin), 
                std::istream_iterator<std::string>(),
                std::back_inserter(strs));
    }
    /*
    {
        std::ifstream fin(get_data("b_string"));
        assert(fin.is_open());
        std::copy(std::istream_iterator<std::string>(fin), 
                std::istream_iterator<std::string>(),
                std::back_inserter(strs));
    }
    {
        std::ifstream fin(get_data("string"));
        assert(fin.is_open());
        std::copy(std::istream_iterator<std::string>(fin), 
                std::istream_iterator<std::string>(),
                std::back_inserter(strs));
    }
    {
        std::ifstream fin(get_data("special_str"));
        assert(fin.is_open());
        std::copy(std::istream_iterator<std::string>(fin), 
                std::istream_iterator<std::string>(),
                std::back_inserter(strs));
    }
    */
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
    std::cerr << "--- ";
    AXSORT::string_sorter::sais sorter;
    std::cerr << "--- ";

    ress.resize(strs.size());
    std::cerr << "--- ";
    for(size_t i=0; i<strs.size(); ++i)
    {
    std::cerr << "--- ";
        ress[i].resize(strs[i].size());
    std::cerr << strs[i].size() << " " << ress[i].size() << "\n";
        sorter.sort(strs[i], ress[i]);
    std::cerr << "--- ";
    }
    std::cerr << "--- ";
}

std::vector<std::string> strs;
std::vector<std::vector<uint32_t>> anss, ress;

void pre_process()
{
    std::cerr << "start getting reference string ... ";
    get_strings(strs);
    std::cerr << " done (";
    for(auto& s : strs) std::cerr << s.length() << " ";
    std::cerr << "\n";
}


TEST(sais, check_answers)   // multi-testcase
{
    //std::vector<std::string_view> strs_view(strs.begin(), strs.end());

    //get_answers(strs_view, anss);
    //print_answer(strs_view, anss);

    std::cerr << strs.size() << ":";
    for(auto& s : strs) std::cerr << s.length() << " ";
    std::cerr << "\n";

    ProfilerStart("sais.prof"); // 指定所生成的profile文件名
    get_result(strs, ress);
    ProfilerStop();           // 结束profiling
    
    //EXPECT_EQ(ress, anss);
}
