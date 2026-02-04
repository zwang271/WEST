CXX ?= g++
CXXFLAGS ?= -std=c++17
LDFLAGS ?=

# Check if we need to link filesystem library
ifeq ($(shell $(CXX) -std=c++17 -E -x c++ - < /dev/null > /dev/null 2>&1 && echo ok),ok)
    # Check if filesystem needs explicit linking
    ifeq ($(shell echo '\#include<filesystem>' | $(CXX) -std=c++17 -E - > /dev/null 2>&1 && echo ok),ok)
        FILESYSTEM_LIB := $(shell echo 'int main(){}' | $(CXX) -std=c++17 -x c++ - -o /tmp/test_fs 2>&1 | grep -q 'filesystem' && echo '-lstdc++fs' || echo '')
    endif
endif

.PHONY: all clean interpret interpret_batch

all: interpret interpret_batch

../../bin/interpret: interpret.cpp evaluate_mltl.cpp evaluate_mltl.h utils.cpp utils.h
	mkdir -p ../../bin
	$(CXX) ./interpret.cpp ./evaluate_mltl.cpp ./utils.cpp -o ../../bin/interpret $(CXXFLAGS) $(FILESYSTEM_LIB) $(LDFLAGS)

../../bin/interpret_batch: interpret_batch.cpp evaluate_mltl.cpp evaluate_mltl.h utils.cpp utils.h
	mkdir -p ../../bin
	$(CXX) ./interpret_batch.cpp ./evaluate_mltl.cpp ./utils.cpp -o ../../bin/interpret_batch $(CXXFLAGS) $(FILESYSTEM_LIB) $(LDFLAGS)

interpret: ../../bin/interpret

interpret_batch: ../../bin/interpret_batch

clean:
	rm -f ../../bin/interpret ../../bin/interpret_batch