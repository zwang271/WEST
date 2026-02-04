CXX ?= g++
CXXFLAGS ?= -std=c++17

.PHONY: all clean string_west

all: string_west

../../bin/west_string: string_grammar.cpp string_nnf_grammar.cpp string_reg.cpp string_rest.cpp string_utils.cpp string_west.cpp string_grammar.h string_nnf_grammar.h string_reg.h string_rest.h string_utils.h
	mkdir -p ../../bin
	$(CXX) string_grammar.cpp string_nnf_grammar.cpp string_reg.cpp string_rest.cpp string_utils.cpp string_west.cpp -o ../../bin/west_string $(CXXFLAGS)

string_west: ../../bin/west_string

clean:
	rm -f ../../bin/west_string