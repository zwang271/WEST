all: west test_rest benchmark_rest benchmark_west west_lib clean

test_rest: test_rest.o utils.o grammar.o nnf_grammar.o reg.o rest.o
	g++ test_rest.o utils.o rest.o grammar.o nnf_grammar.o reg.o -o test_rest -std=c++17

west: main.o utils.o reg.o nnf_grammar.o grammar.o rest.o
	g++  main.o utils.o reg.o nnf_grammar.o grammar.o rest.o -o west -std=c++17

west_lib: 
	g++ west_lib.cpp utils.cpp reg.cpp nnf_grammar.cpp grammar.cpp rest.cpp -o ./gui/west_lib -std=c++17

west_lib.o: west_lib.cpp
	g++ -c west_lib.cpp -std=c++17

benchmark_west: simulation_main.o utils.o grammar.o nnf_grammar.o reg.o
	g++ simulation_main.o utils.o grammar.o nnf_grammar.o reg.o -o benchmark_west -std=c++17

benchmark_rest: rest_experiments.o utils.o grammar.o nnf_grammar.o reg.o rest.o
	g++ rest_experiments.o utils.o rest.o grammar.o nnf_grammar.o reg.o -o benchmark_rest -std=c++17

simulation_main.o: simulation_main.cpp
	g++ -c simulation_main.cpp -std=c++17

rest_experiments.o: rest_experiments.cpp
	g++ -c rest_experiments.cpp -std=c++17

test_rest.o: test_rest.cpp
	g++ -c test_rest.cpp -std=c++17

rest.o: rest.cpp
	g++ -c rest.cpp -std=c++17
  
main.o: main.cpp
	g++ -c main.cpp -std=c++17
  
utils.o: utils.cpp utils.h
	g++ -c utils.cpp -std=c++17
  
reg.o: reg.cpp reg.h
	g++ -c reg.cpp -std=c++17
  
nnf_grammar.o: nnf_grammar.cpp nnf_grammar.h
	g++ -c nnf_grammar.cpp -std=c++17
  
grammar.o: grammar.cpp grammar.h
	g++ -c grammar.cpp -std=c++17
  
clean: 
	rm -f main.o utils.o reg.o nnf_grammar.o grammar.o 
