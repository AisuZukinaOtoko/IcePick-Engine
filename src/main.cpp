#include <iostream>
#include "IcePickRuntime.h"

int main(void)
{
    IcePick::Engine RuntimeEngine;
    if (!RuntimeEngine.Init()) {
        std::cout << "Engine failed to init.\n";
        RuntimeEngine.Terminate();
        return -1;
    }
    RuntimeEngine.Run();
    return 0;
}