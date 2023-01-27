all: west clean

west: main.o utils.o reg.o nnf_grammar.o grammar.o rest.o
	g++  main.o utils.o reg.o nnf_grammar.o grammar.o rest.o -o west -std=c++17

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
	rm -f main.o utils.o reg.o nnf_grammar.o grammar.o -std=c++17
