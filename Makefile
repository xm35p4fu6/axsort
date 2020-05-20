link_library = -lboost_filesystem -lboost_system -ldl -lgtest -lpthread
include_dirs = ../include

source_library = ../unit_test/script
Cflag= -Wall -std=c++17 -I $(include_dirs) $(link_library)

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
