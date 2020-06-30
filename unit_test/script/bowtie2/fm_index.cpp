#include <AXSORT/aligner/aligner.hpp>
#include <AXSORT/aligner/seeds.hpp>
#include <AXSORT/test/gtest.hpp>
#include <AXSORT/test/data_dir.hpp>
#include <AXSORT/string_sorter/sais.hpp>

#include <iostream>
#include <string_view>
#include <numeric>
#include <vector>
#include <iterator>
#include <iomanip>

using namespace std;

void pre_process()
{
}

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
	ofstream fasta_database_file;
	fasta_database_file.open((axsort::test::data_dir() /"database.fasta").string());
	
    for(string q: querys)
    {
        cout<<"search for \""<<q<<"\""<<endl;

        auto v = a.query(q);
        cout<<v.size()<<endl;
        for(int i: v)
		{
			if ((i - q.length() + 1) > 0)
			{
				cout<<i<<": "<<str.substr(i - q.length() + 1, 2 * q.length()-1)<<endl;				
			}else
			{
				cout<<i<<": "<<str.substr(i, q.length())<<endl;								
			}			
			if (v.size()>0)
			{
				fasta_database_file << ">" << i << "\n";
				if ((i - q.length() + 1) > 0)
				{
					fasta_database_file <<str.substr(i - q.length() + 1, 2 * q.length()-1) <<"\n";					
				}else
				{
					fasta_database_file <<str.substr(i, q.length()) <<"\n";					
				}			}
		}
        cout<<endl;
    }
	fasta_database_file.close();
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

template<typename T>
void get_strings(T& strs)
{
    {
        std::ifstream fin(sequence());
        assert(fin.is_open());
        std::copy(std::istream_iterator<std::string>(fin), 
                std::istream_iterator<std::string>(),
                std::back_inserter(strs));
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

std::vector<std::string> strs;
std::vector<std::vector<uint32_t>> anss, ress;

TEST(BWT, check_reverse)
{
	std::ifstream ifs(sequence());
    string str, read, reverse;
	read = "GAA";
	reverse = read;
	
	Seeds s(reverse);

	ofstream fasta_query_file;
	fasta_query_file.open((axsort::test::data_dir() /"query.fasta").string());
	fasta_query_file << ">query\n";
	fasta_query_file << read << "\n";

    int c = 0;
	vector<int> sa(str.size());
    while(getline(ifs, str))
    {
        cout<<str<<endl;
		
		get_strings(strs);
		get_result(strs, ress);
		
		for (int i = 0; i < ress.size(); i++) { 
			for (int j = 0; j < ress[i].size(); j++){
				sa.push_back(ress[i][j]);
			}
		} 
		
        Aligner a(str);
        vector<string> querys;
		
		reverse = s.read_reverse(reverse);
		querys.push_back(read);
		querys.push_back(reverse);

        auto v = string_to_vector(str);

        for(auto q: querys)
        {
            auto ans = a.query(q);
            auto ep = query(sa, str, q);
            EXPECT_EQ(ep, ans);
        }
        print_result(a, str, querys);
            
    }
}
/*
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
        vector<string> querys = { "QDDRRNA"}; 
        
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
*/