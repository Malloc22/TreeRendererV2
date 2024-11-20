//----------------------------

#include "Core.hpp"

//----------------------------

int main() {

    Core core;
    if(!core.init({ 1280, 720 }))
        return -1;

    while(core.windowIsOpen()) {

        core.draw();
        core.update();
    }

    return 0;
}

//----------------------------
