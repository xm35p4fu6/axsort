#include <iostream>
#include <AXSORT/test/data_dir.hpp>

std::string sample_data()
{
    return (axsort::test::data_dir() / "sample_data").string();
}

void test1() {
    std::ifstream fin(sample_data());
    int a, b;

    fin >> a >> b;

    assert(a == b);
    std::cout << "succ" << std::endl;
}

int main() {
    test1();
}
