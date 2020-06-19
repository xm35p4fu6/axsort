#pragma once
#include "suffix_array/DC3.hpp"

class Seeds
{
  public:
    Seeds() = default;
    Seeds(string& str)
    {
    }

	string read_reverse(string& read) {
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


};