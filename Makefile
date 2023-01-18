COMPILER_FLAGS = -Wall -Wfatal-errors -O0 -DDEBUG=2 -g 
INCLUDE_PATH = -I"./libs"
CC = g++
LANG_STD = -std=c++17
SRC_FILES = src/*.cpp src/Logger/*.cpp src/Game/*.cpp src/ECS/*.cpp src/AssetStore/*.cpp
LINKER_FLAGS = -lSDL2 -llua5.3 -lSDL2_ttf -lSDL2_mixer -lSDL2_image
TARGET = bin/Main.bin
build:
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATH) $(LANG_STD) $(SRC_FILES) $(LINKER_FLAGS) -o $(TARGET)

run: build copy_assets
	./bin/Main.bin

copy_assets:
	@cp -rf ./assets ./bin/assets

clean:
	rm ./bin/Main.bin -rf
	rm ./bin/assets -rf
