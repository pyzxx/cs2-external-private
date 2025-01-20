#include "render/render.h"
#include "render/overlay.h"
#include "cheat/sdk.h"
#include <thread>
#include <iostream>


int main()
{
 


    std::thread Readthread(&Sdk::ThreadLoop, &sdk);
    Readthread.detach();

    //funcitions for cheat
    std::thread CheatThread(&Sdk::LoopCheat, &sdk);
    CheatThread.detach();

	render.loopRender();

    return 0;
}

