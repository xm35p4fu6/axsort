#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <tuple>
#include <utility>

using namespace std;

class Three_char_unit
{
  public:

    Three_char_unit(int s1, int idx):s1(s1), idx(idx){}
    Three_char_unit(int s1, int s2, int s3, int idx):s1(s1), s2(s2), s3(s3), idx(idx){}
    bool operator<(const Three_char_unit& b)
    {
        if(s1 != b.s1)
        {
            return s1 < b.s1;
        }
        else if(s2 != b.s2)
        {
            return s2 < b.s2;
        }
        else
            return s3 < b.s3;
    }

    bool operator==(const Three_char_unit& b)
    {
        if( s1 == b.s1 && s2 == b.s2 && s3 == b.s3 )
            return true;
        return false;
    }

    friend ostream& operator<<(ostream& o, const Three_char_unit& b)
    {
        o<<b.s1<<b.s2<<b.s3;
        return o;
    }

    int s1, s2, s3;
    int idx;
};


void append_remaining_idx(vector<Three_char_unit>& from, int beg1, vector<int>& to, int beg2)
{
    for(; beg1 < from.size(); ++beg1)
    {
        to[beg2] = from[beg1].idx;
        ++beg2;
    }
}

vector<int> DC3(vector<int>& str)
{
    int orig_len = str.size();
    str.push_back(0);
    str.push_back(0);

    vector<Three_char_unit> b0, b2, orig_b12;

    for(int i = 0; i < orig_len; ++i)
    {
        if(i % 3 == 0)
            b0.emplace_back(str[i] , i);
        else if( i % 3 == 1)
            orig_b12.emplace_back( str[i], str[i+1], str[i+2], i);
        else
            b2.emplace_back(str[i], str[i+1], str[i+2], i);
    }
    

    copy(begin(b2), end(b2), back_inserter(orig_b12));
    auto sorted_b12 = orig_b12;

    // radix sort would be better
    sort(begin(sorted_b12), end(sorted_b12));
    

    bool is_even = false;
    for(int i = 1; i < sorted_b12.size(); ++i)
    {
        if(sorted_b12[i-1] == sorted_b12[i])
        {
            is_even = true;
            break;
        }
    }

    vector<int> orig_rank(orig_len);
    orig_rank.push_back(0);
    orig_rank.push_back(0);

    if(is_even)
    {
        
        orig_rank[ sorted_b12[0].idx ] = 1;
        for (int i = 1, j = 1; i < sorted_b12.size(); ++i)
        {
            if( sorted_b12[i] == sorted_b12[i-1])
                orig_rank[ sorted_b12[i].idx ] = j;
            else 
                orig_rank[ sorted_b12[i].idx ] = ++j;
        }
        vector<int> str2;
        for(auto unit: orig_b12)
        {
            str2.push_back(orig_rank[ unit.idx ]);
        }
        str2.push_back(0);
        
        
        vector<int> SA2 = DC3(str2);
        assert(SA2[0] == str2.size() -1);
        
        SA2.erase(begin(SA2));

        
        for(int i = 0; i < SA2.size(); ++i)
        {
            orig_rank[ orig_b12[ SA2[i] ].idx ] = i;
            sorted_b12[i] = orig_b12[ SA2[i] ];
        }
    }
    else
    {
        
        for(int i = 0; i < sorted_b12.size(); ++i)
        {
            orig_rank[ sorted_b12[ i ].idx ] = i;
        }
    }

    for(auto& unit: b0)
    {
        unit.s2 = orig_rank[ unit.idx + 1 ];
    }

    sort(begin(b0), end(b0));

    vector<int> SA(orig_len);

    // merge
    for(int i = 0, j = 0, sa_idx = 0; ;)
    {
        if(i != b0.size() && j != sorted_b12.size())
        {
            if(sorted_b12[j].idx % 3 == 1)
            {
                
                auto s0 = make_pair(b0[i].s1, b0[i].s2);
                auto s1 = make_pair(sorted_b12[j].s1, 
                                    orig_rank[ sorted_b12[j].idx + 1 ] 
                                   );
                
                
                assert(s0 != s1);
                if(s0 < s1)
                {
                    SA[sa_idx] = b0[i++].idx;
                }
                else
                {
                    SA[sa_idx] = sorted_b12[j++].idx;
                }
                sa_idx++;

            }
            else
            {
                auto s0 = make_tuple(b0[i].s1, 
                                       str[ b0[i].idx+1 ],
                                       orig_rank[ b0[i].idx + 2 ]
                                      );
            
                auto s2 = make_tuple(sorted_b12[j].s1, 
                                       str[ sorted_b12[j].idx+1 ],
                                       orig_rank[ sorted_b12[j].idx + 2 ]
                                      );
                
                
                assert(s0 != s2);
                if(s0 < s2)
                {
                    SA[sa_idx] = b0[i++].idx;
                }
                else
                {
                    SA[sa_idx] = sorted_b12[j++].idx;
                }
                sa_idx++;

            }
        }
        else if(i == b0.size())
        {
            append_remaining_idx( sorted_b12, j, SA, sa_idx);
            break;
        }
        else if(j == sorted_b12.size())
        {
            append_remaining_idx( b0, i, SA, sa_idx);
            break;
        }
    }
    str.erase(str.end()-2, str.end());
    return SA;
}