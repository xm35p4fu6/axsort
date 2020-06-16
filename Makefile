link_library = -lboost_filesystem -lboost_system -ldl -lgtest -lpthread
include_dirs = ../include

source_library = ../unit_test/script
bowtie2_library = ../unit_test/script/bowtie2
simd_library = ../include/AXSORT/aligner
Cflag= -Wall -std=c++17 -I $(include_dirs) $(link_library)

CXXFLAGS = -std=c++11 -march=native -Wall -I $(include_dirs) $(link_library)

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -fno-omit-frame-pointer
else
	CXXFLAGS += -O3
endif

all:
	echo "hahaha"

# TARGET test:  for convenience in build/
test: test.cpp
	g++ -std=c++17 test.cpp $(Cflag) -o $@


# TARGET sample: example.  
# $< = first val after ':', that is, $(source_library)/sample_test.cpp
sample: $(source_library)/sample_test.cpp
	g++ $< $(Cflag) -o $@


# TARGET string_sorter: Xman's command. 
# $@ = TARGET, that is, string_sorter
string_sorter: $(source_library)/string_sorter/string_sorter.cpp
	g++ $(source_library)/$@/$@.cpp $(Cflag) -o $@


# TARGET seed_extraction
seed_extraction: $(bowtie2_library)/seed_extraction.cpp
	g++ $< $(Cflag) -o $@

# TARGET fm_index
fm_index: $(bowtie2_library)/fm_index.cpp
	g++ $< $(Cflag) -o $@

# TARGERT simd_aligner
simd_aligner: $(simd_library)/opal.o $(simd_library)/ScoreMatrix.o $(bowtie2_library)/simd_aligner.cpp
	g++ $(CXXFLAGS) $(simd_library)/opal.o $(simd_library)/ScoreMatrix.o $(bowtie2_library)/simd_aligner.cpp -o $@
	
ScoreMatrix.o: $(simd_library)/ScoreMatrix.cpp $(simd_library)/ScoreMatrix.hpp
	g++ $(CXXFLAGS) -c $(simd_library)/ScoreMatrix.cpp

opal.o: $(simd_library)/opal.cpp $(simd_library)/opal.h
	g++ $(CXXFLAGS) -c $(simd_library)/opal.cpp	