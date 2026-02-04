CXX ?= g++
CXXFLAGS ?= -std=c++17
LDFLAGS ?= -lstdc++fs

# Check if filesystem library is needed (older compilers)
FILESYSTEM_LIB := $(shell echo 'int main(){}' | $(CXX) -std=c++17 -x c++ - -o /tmp/test_fs 2>&1 | grep -q 'filesystem' && echo '-lstdc++fs' || echo '')

.PHONY: all clean west

all: west

../../bin/west: west.cpp reg.cpp reg.h utils.cpp utils.h parser.cpp parser.h
	mkdir -p ../../bin
	$(CXX) west.cpp reg.cpp utils.cpp parser.cpp -o ../../bin/west $(CXXFLAGS) $(FILESYSTEM_LIB)

west: ../../bin/west

clean:
	rm -f ../../bin/west