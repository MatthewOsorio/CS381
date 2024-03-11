#include "raylib-cpp.hpp"
#include <iostream>

void PingButton(){
    std::cout<< "Ping!"<< std::endl;
}

#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "VolumeControl.h"

int main (){
    raylib:: Window window (800, 540, "CS381 - AS5");

    auto guiState = InitGuiVolumeControl();

    while(!window.ShouldClose()){
        window.BeginDrawing();
        {
            GuiVolumeControl(&guiState);
        }
        window.EndDrawing();
    }

    return 0;
}