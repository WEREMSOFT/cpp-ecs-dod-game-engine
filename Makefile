build:
	g++ -Wall -std=c++17 -I"./libs" src/*.cpp -lSDL2 -llua5.3 -lSDL2_ttf -lSDL2_mixer -lSDL2_image -o bin/Main.bin

run: build
	./bin/Main.bin

clean:
	rm ./bin/Main.bin
