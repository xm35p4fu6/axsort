#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include <ctime>
#include <string>
#include <climits>
#include<iostream>

#include <AXSORT/aligner/opal.h>
#include <AXSORT/aligner/ScoreMatrix.hpp>

#include <AXSORT/aligner/aligner.hpp>
#include <AXSORT/aligner/seeds.hpp>


#include <fstream>

using namespace std;

//-------------FM-index functions----------------------------------//

void pre_process()
{
}

string sequence()
{
    return ("../unit_test/data/sequence.txt");
}

vector<int> print_result(Aligner& a, string& str, vector<string>& querys)
{
	ofstream fasta_database_file;
	fasta_database_file.open("../unit_test/data/database.fasta");
	vector<int> indexes;
	
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
				indexes.push_back(i);
				if ((i - q.length() + 1) > 0)
				{
					fasta_database_file <<str.substr(i - q.length() + 1, 2 * q.length()-1) <<"\n";					
				}else
				{
					fasta_database_file <<str.substr(i, q.length()) <<"\n";					
				}
			}
		}
        cout<<endl;
    }
	fasta_database_file.close();
	return indexes;
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
//-----------------------------------------------------------//

bool readFastaSequences(FILE* &file, unsigned char* alphabet, int alphabetLength, vector< vector<unsigned char> >* seqs);
void printAlignment(const unsigned char* query, const int queryLength,
                    const unsigned char* target, const int targetLength,
                    const OpalSearchResult result, const unsigned char* alphabet, vector<int> indexes, int i);

int main(int argc, char * const argv[]) 
{
	//----------------------------- FM-Index part -----------------------------//
	vector<int> indexes;
	ifstream ifs(sequence());
    string str, read, reverse;
	read = "GAA";
	reverse = read;
	
	Seeds s(reverse);

	ofstream fasta_query_file;
	fasta_query_file.open("../unit_test/data/query.fasta");
	fasta_query_file << ">query\n";
	fasta_query_file << read << "\n";
	fasta_query_file.close();

    int c = 0;

    while(getline(ifs, str))
    {
        cout<<"*************************************"<<endl;
        cout<<str<<endl;

        str.push_back(0);
        Aligner a(str);
        vector<string> querys;
		
		reverse = s.read_reverse(reverse);
		querys.push_back(read);
		querys.push_back(reverse);

        auto v = string_to_vector(str);
        auto sa = DC3(v);

        indexes = print_result(a, str, querys);
    }	
    //-------------------------------------------------------------------------//
	
    int gapOpen = 3;
    int gapExt = 1;
    ScoreMatrix scoreMatrix;

    //----------------------------- PARSE COMMAND LINE ------------------------//
    string scoreMatrixName = "Blosum50";
    bool scoreMatrixFileGiven = false;
    char scoreMatrixFilepath[512];
    bool silent = false;
    char mode[16] = "SW";
    int searchType = 2;//OPAL_SEARCH_SCORE;
    int option;
	/*
    while ((option = getopt(argc, argv, "a:o:e:m:f:x:s")) >= 0) {
        switch (option) {
        case 'a': strcpy(mode, optarg); break;
        case 'o': gapOpen = atoi(optarg); break;
        case 'e': gapExt = atoi(optarg); break;
        case 'm': scoreMatrixName = string(optarg); break;
        case 'f': scoreMatrixFileGiven = true; strcpy(scoreMatrixFilepath, optarg); break;
        case 's': silent = true; break;
        case 'x': searchType = atoi(optarg); break;
        }
    }*/

    //-------------------------------------------------------------------------//

    // Set score matrix by name
    if (scoreMatrixName == "Blosum50")
        scoreMatrix = ScoreMatrix::getBlosum50();
    else {
        fprintf(stderr, "Given score matrix name is not valid\n");
        exit(1);
    }
    // Set score matrix by filepath
    if (scoreMatrixFileGiven) {
        scoreMatrix = ScoreMatrix(scoreMatrixFilepath);
    }

    unsigned char* alphabet = scoreMatrix.getAlphabet();
    int alphabetLength = scoreMatrix.getAlphabetLength();

    // Detect mode
    int modeCode = OPAL_MODE_SW;

    // Build query
    char* queryFilepath = "../unit_test/data/query.fasta";//argv[optind];
    FILE* queryFile = fopen(queryFilepath, "r");
    if (queryFile == 0) {
        printf("Error: There is no file with name %s\n", queryFilepath);
        return 1;
    }
    vector< vector<unsigned char> >* querySequences = new vector< vector<unsigned char> >();
    printf("Reading query fasta file...\n");
    readFastaSequences(queryFile, alphabet, alphabetLength, querySequences);
    unsigned char* query = (*querySequences)[0].data();
    int queryLength = (*querySequences)[0].size();
    printf("Read query sequence, %d residues.\n", queryLength);
    fclose(queryFile);


    // Build db
    char* dbFilepath = "../unit_test/data/database.fasta";//argv[optind+1];
    FILE* dbFile = fopen(dbFilepath, "r");
    if (dbFile == 0) {
        printf("Error: There is no file with name %s\n", dbFilepath);
        return 1;
    }

    double cpuTime = 0;
    bool wholeDbRead = false;
    int dbTotalNumResidues = 0;  // Sum of lengths of all database sequences.
    int dbTotalLength = 0;  // Number of sequences in the database.
    while (!wholeDbRead) {
        vector< vector<unsigned char> >* dbSequences = new vector< vector<unsigned char> >();
        printf("\nReading database fasta file...\n");
        // Chunk of database is read and processed (if database is not huge, there will be only one chunk).
        // We do this because if database is huge, it may not fit into memory.
        wholeDbRead = readFastaSequences(dbFile, alphabet, alphabetLength, dbSequences);
        int dbLength = dbSequences->size();
        unsigned char** db = new unsigned char*[dbLength];
        int* dbSeqLengths = new int[dbLength];
        int dbNumResidues = 0;
        for (unsigned int i = 0; i < dbSequences->size(); i++) {
            db[i] = (*dbSequences)[i].data();
            dbSeqLengths[i] = (*dbSequences)[i].size();
            dbNumResidues += dbSeqLengths[i];
        }
        printf("Read %d database sequences, %d residues total.\n", dbLength, dbNumResidues);

        dbTotalNumResidues += dbNumResidues;
        dbTotalLength += dbLength;
        if (wholeDbRead) {
            printf("Whole database read: %d database sequences, %d residues in total.\n",
                   dbTotalLength, dbTotalNumResidues);
        }

        // ----------------------------- MAIN CALCULATION ----------------------------- //
        OpalSearchResult** results = new OpalSearchResult*[dbLength];
        for (int i = 0; i < dbLength; i++) {
            results[i] = new OpalSearchResult;
            opalInitSearchResult(results[i]);
        }
        printf("\nComparing query to database...");
        fflush(stdout);
        clock_t start = clock();
        int resultCode = opalSearchDatabase(query, queryLength, db, dbLength, dbSeqLengths,
                                             gapOpen, gapExt, scoreMatrix.getMatrix(), alphabetLength,
                                             results, searchType, modeCode, OPAL_OVERFLOW_BUCKETS);
        if (resultCode) {
            printf("\nDatabase search failed with error code: %d\n", resultCode);
        }
        clock_t finish = clock();
        cpuTime += ((double)(finish-start))/CLOCKS_PER_SEC;
        // ---------------------------------------------------------------------------- //
        printf("\nFinished!\n");

        if (!silent) {
            //printf("\n#<i>: <score> (<query start>, <target start>) (<query end>, <target end>)\n");
            for (int i = 0; i < dbLength; i++) {
				printf("#%d:", dbTotalLength - dbLength + i);//printf("#%d: %d", dbTotalLength - dbLength + i, results[i]->score);
				/*
                if (results[i]->startLocationQuery >= 0) {
                    printf(" (%d, %d)", results[i]->startLocationQuery, results[i]->startLocationTarget);
                } else {
                    printf(" (?, ?)");
                }
                if (results[i]->endLocationQuery >= 0) {
                    printf(" (%d, %d)", results[i]->endLocationQuery, results[i]->endLocationTarget);
                } else {
                    printf(" (?, ?)");
                }*/
                printf("\n");

                if (results[i]->alignment) {
                    printAlignment(query, queryLength, db[i], dbSeqLengths[i], *results[i], alphabet, indexes, i);
                }
            }
        }

        for (int i = 0; i < dbLength; i++) {
            if (results[i]->alignment) {
                free(results[i]->alignment);
            }
            delete (results[i]);
        }
        delete[] results;
        delete[] db;
        delete[] dbSeqLengths;
        delete dbSequences;
    }

    printf("\nCpu time of searching: %.6lf\n", cpuTime);
    if (searchType != OPAL_SEARCH_ALIGNMENT) {
        printf("GCUPS (giga cell updates per second): %.2lf\n",
               dbTotalNumResidues / 1000000000.0 * queryLength / cpuTime);
    }


    fclose(dbFile);
    // Free allocated space
    delete querySequences;

    return 0;
}

/** Reads sequences from fasta file. If it reads more than some amount of sequences, it will stop.
 * @param [in] file File pointer to database. It may not be the beginning of database.
 * @param [in] alphabet
 * @param [in] alphabetLength
 * @param [out] seqs Sequences will be stored here, each sequence as vector of indexes from alphabet.
 * @return true if reached end of file, otherwise false.
 */
bool readFastaSequences(FILE* &file, unsigned char* alphabet, int alphabetLength, vector< vector<unsigned char> >* seqs) {
    seqs->clear();

    unsigned char letterIdx[128]; //!< letterIdx[c] is index of letter c in alphabet
    for (int i = 0; i < alphabetLength; i++)
        if (alphabet[i] == '*') { // '*' represents all characters not in alphabet
            for (int j = 0; j < 128; j++)
                letterIdx[j] = i;
            break;
        }
    for (int i = 0; i < alphabetLength; i++)
        letterIdx[alphabet[i]] = i;

    long numResiduesRead = 0;
    bool inHeader = false;
    bool inSequence = false;
    int buffSize = 4096;
    unsigned char buffer[buffSize];
    while (!feof(file)) {
        int read = fread(buffer, sizeof(char), buffSize, file);
        for (int i = 0; i < read; ++i) {
            unsigned char c = buffer[i];
            if (inHeader) { // I do nothing if in header
                if (c == '\n')
                    inHeader = false;
            } else {
                if (c == '>') {
                    inHeader = true;
                    inSequence = false;
                } else {
                    if (c == '\r' || c == '\n')
                        continue;
                    // If starting new sequence, initialize it.
                    // Before that, check if we read more than 1GB of sequences,
                    // and if that is true, finish reading.
                    if (inSequence == false) {
                        if (seqs->size() > 0) {
                            numResiduesRead += seqs->back().size();
                        }
                        if (numResiduesRead > 1073741824L) {
                            fseek(file, i - read, SEEK_CUR);
                            return false;
                        }
                        inSequence = true;
                        seqs->push_back(vector<unsigned char>());
                    }

                    seqs->back().push_back(letterIdx[c]);
					
                }
            }
        }
    }

    return true;
}

void printAlignment(const unsigned char* query, const int queryLength,
                    const unsigned char* target, const int targetLength,
                    const OpalSearchResult result, const unsigned char* alphabet, vector<int> indexes, int i) {
    int tIdx = result.startLocationTarget;
    int qIdx = result.startLocationQuery;
    for (int start = 0; start < result.alignmentLength; start += 50) {
        // target
        printf("T: ");
        int startTIdx = tIdx;
        for (int j = start; j < start + 50 && j < result.alignmentLength; j++) {
            if (result.alignment[j] == OPAL_ALIGN_DEL)
                printf("_");
            else
                printf("%c", alphabet[target[tIdx++]]);
        }
        //printf(" (%d - %d)\n", max(startTIdx, 0), tIdx - 1);
		printf("\n");

        // query
        printf("Q: ");
        int startQIdx = qIdx;
        for (int j = start; j < start + 50 && j < result.alignmentLength; j++) {
            if (result.alignment[j] == OPAL_ALIGN_INS)
                printf("_");
            else
                printf("%c", alphabet[query[qIdx++]]);
        }
        printf(" (%d - %d)\n\n", max(startQIdx, 0) + indexes.at(i), qIdx - 1 + indexes.at(i));
    }
}
