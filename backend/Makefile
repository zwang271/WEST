all: clean west 

west:
	g++ west.cpp reg.cpp utils.cpp parser.cpp -o ./west -std=c++17

clean:
	rm ./west 2>/dev/null || true
	