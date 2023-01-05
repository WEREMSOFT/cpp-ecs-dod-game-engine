build:
	g++ -Wall -O0 -g -std=c++17 -I"./libs" src/*.cpp src/Logger/*.cpp src/Game/*.cpp -lSDL2 -llua5.3 -lSDL2_ttf -lSDL2_mixer -lSDL2_image -o bin/Main.bin

run: build
	./bin/Main.bin

clean:
	rm ./bin/Main.bin