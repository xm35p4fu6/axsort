#include <iostream>
#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/string_sorter/sais.hpp>
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

int main()
{
    std::string str;
    std::vector<int> sa;

    get_string_data(str);
    get_answer(str, sa);
    print_answer(str, sa);
}
