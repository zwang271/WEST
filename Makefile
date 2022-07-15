output: main.o utils.o reg.o nnf_grammar.o grammar.o
  g++  main.o utils.o reg.o nnf_grammar.o grammar.o -o output
  
main.o: main.cpp
  g++ -c main.cpp 
  
utils.o: utils.cpp utils.h
  g++ -c utils.cpp
  
reg.o: reg.cpp reg.h
  g++ -c reg.cpp
  
nnf_grammar.o: nnf_grammar.cpp nnf_grammar.h
  g++ -c nnf_grammar.cpp
  
grammar.o: grammar.cpp grammar.h
  g++ -c grammar.cpp
  
clean: 
  rm *.o output
