#include <iostream>
#include <limits>
#include <sol/sol.hpp>

#include "Game/Game.h"

void TestLua()
{
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.script_file("assets/scripts/myscript.lua");

	// this is how a get a variable from the lua script
	int someVariable = lua["some_variable"];
	printf("the value on C++ is %d\n", someVariable);
}

int main(int argc, char **argv)
{
	// Game game;

	// game.Initialize();
	// game.Run();
	// game.Destroy();

	TestLua();

	return 0;
}
