build:
	g++ -Wall -std=c++17 src/*.cpp -lSDL2 -llua5.3 -o bin/Main.bin

run: build
	./bin/Main.bin

clean:
	rm ./bin/Main.bin
