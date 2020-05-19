link_library = -lboost_filesystem -lboost_system -ldl
include_dirs = ../include

source_library = ../unit_test/script
Cflag= -Wall -std=c++17 -I $(include_dirs) $(link_library)

all:

sample:
	g++ $(source_library)/sample_test.cpp $(Cflag)

