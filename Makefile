link_library = -lboost_filesystem -lboost_system -ldl -lgtest -lpthread -lprofiler
include_dirs = ../include

source_library = ../unit_test/script
Cflag = -Wall -std=c++2a -I $(include_dirs) $(link_library) -g
Cflag_O2 = -Wall -std=c++2a -I $(include_dirs) $(link_library) -O2
CC = g++-10

all:
	echo "hahaha"

# TARGET test:  for convenience in build/
test: test.cpp
	$(CC) -std=c++2a test.cpp $(Cflag) -o $@


# TARGET sample: example.  
# $< = first val after ':', that is, $(source_library)/sample_test.cpp
sample: $(source_library)/sample_test.cpp
	$(CC) $< $(Cflag) -o $@


# TARGET string_sorter: Xman's command. 
# $@ = TARGET, that is, string_sorter
string_sorter: $(source_library)/string_sorter/string_sorter.cpp
	$(CC) $(source_library)/$@/$@.cpp $(Cflag) -o $@
	$(CC) $(source_library)/$@/$@.cpp $(Cflag_O2) -o $@_O2


fastq: $(source_library)/format/fastq_test.cpp
	$(CC) $(source_library)/format/fastq_test.cpp $(Cflag) -o $@

.PHONY: string_sorter
