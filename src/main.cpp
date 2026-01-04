#include <iostream>
#include "IcePickRuntime.h"
#include "sol/sol.hpp"

int main(void)
{
    IcePick::Engine RuntimeEngine;
    if (!RuntimeEngine.Init()) {
        std::cout << "Engine failed to init.\n";
        RuntimeEngine.Terminate();
        return -1;
    }

    sol::state lua;

    // Open standard Lua libraries
    lua.open_libraries(sol::lib::base);

    // Run a simple Lua expression
    int result = lua.script("return 2 + 3");
    std::cout << "Lua says 2 + 3 = " << result << std::endl;

    // Expose a C++ function to Lua
    lua.set_function("cpp_add", [](int a, int b) {
        return a + b;
        });

    // Call C++ from Lua
    int result2 = lua.script("return cpp_add(10, 32)");
    std::cout << "Lua calling C++ says 10 + 32 = " << result2 << std::endl;
    lua.script("print('Lua is alive')");

    RuntimeEngine.Run();
    return 0;
}