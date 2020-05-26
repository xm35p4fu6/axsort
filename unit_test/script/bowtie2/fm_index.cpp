#include <AXSORT/aligner/aligner.hpp>
#include <AXSORT/test/gtest.hpp>
#include <AXSORT/test/data_dir.hpp>

#include <fstream>
#include <string>

using namespace std;

string article()
{
    return (axsort::test::data_dir() / "article.txt").string();
}

string sequence()
{
    return (axsort::test::data_dir() / "sequence.txt").string();
}

void print_result(Aligner& a, string& str, vector<string>& querys)
{
    
    for(string q: querys)
    {
        cout<<"search for \""<<q<<"\""<<endl;

        auto v = a.query(q);
        
        cout<<v.size()<<endl;
        for(int i: v)
            cout<<i<<": "<<str.substr(i, 20)<<endl;
        cout<<endl;
    }
}

vector<int> string_to_vector(string& str)
{   
    vector<int> s(str.length());
    for(int i = 0; i<str.length(); i++)
        s[i] = str[i];
    return s;
}

vector<int> query(vector<int>& SA, string& str, string& q)
{
    vector<int> res;
    int l = str.length();
    int ll = q.length();

    for(auto i: SA)
    {
        bool diff = false;

        for(int d = 0; d<l; d++)
        {
            if(i+d==l || d == ll)
                break;
            else if( str[i+d] != q[d])
            {
                diff = true;
                break;
            }
        }
        if(!diff)
            res.push_back(i);
    }
    return res;
}


TEST(BWT, file1_normal)
{
    std::ifstream ifs(article());
    string str;
    
    int c = 0;

    while(getline(ifs, str))
    {
        cout<<"**************** CASE "<<c++<<" *********************"<<endl;
        cout<<str<<endl;

        str.push_back(0);
        Aligner a(str);
        vector<string> querys = { "t", "th", "the"};
        
        auto v = string_to_vector(str);
        auto sa = DC3(v);

        for(auto q: querys)
        {
            auto ans = a.query(q);
            auto ep = query(sa, str, q);
            EXPECT_EQ(ep, ans);
        }
        print_result(a, str, querys);
            
    }
}

TEST(BWT, file1_boundary_case)
{
    std::ifstream ifs(article());
    string str;
    
    int c = 0;

    while(getline(ifs, str))
    {
        cout<<"**************** CASE "<<c++<<" *********************"<<endl;
        cout<<str<<endl;

        str.push_back(0);
        cout<<str.length()<<endl;
        Aligner a(str);
        vector<string> querys = {  ".", "s.", "Th", "ere" }; 
        
        auto v = string_to_vector(str);
        auto sa = DC3(v);

        for(auto q: querys)
        {
            auto ans = a.query(q);
            auto ep = query(sa, str, q);
            EXPECT_EQ(ep, ans);
        }
        print_result(a, str, querys);
            
    }
}

TEST(BWT, file2)
{
    std::ifstream ifs(sequence());
    string str;
    
    int c = 0;

    while(getline(ifs, str))
    {
        cout<<"**************** CASE "<<c++<<" *********************"<<endl;
        cout<<str<<endl;

        str.push_back(0);
        Aligner a(str);
        vector<string> querys = { "RQQ", "QDDRRNA", "CQ", 
        "DAGCANEACDCCDNAECCGNRACACQAQQNRERCEGQGGNCQARADDQRDQNCNANRGACCQEANG",
         "AN", "CCRDG", "NEEQ" }; 
        
        auto v = string_to_vector(str);
        auto sa = DC3(v);
        for(auto q: querys)
        {
            auto ans = a.query(q);
            auto ep = query(sa, str, q);
            EXPECT_EQ(ep, ans);
        }
        print_result(a, str, querys);
            
    }
}