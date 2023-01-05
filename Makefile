COMPILER_FLAGS = -Wall -Wfatal-errors -O0 -g 
INCLUDE_PATH = -I"./libs"
CC = g++
LANG_STD = -std=c++17
SRC_FILES = src/*.cpp src/Logger/*.cpp src/Game/*.cpp
LINKER_FLAGS = -lSDL2 -llua5.3 -lSDL2_ttf -lSDL2_mixer -lSDL2_image
TARGET = bin/Main.bin
build:
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATH) $(LANG_STD) $(SRC_FILES) $(LINKER_FLAGS) -o $(TARGET)

run: build
	./bin/Main.bin

clean:
	rm ./bin/Main.bin
