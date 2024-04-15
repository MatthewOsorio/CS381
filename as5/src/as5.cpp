#include "raylib-cpp.hpp"
#include <iostream>
#include "delegate.hpp"
#include <BufferedInput.hpp>

cs381::Delegate<void()> PingButton;

#include "VolumeControl.h"

int main (){
    raylib:: Window window (300, 350, "CS381 - AS5");
    raylib::AudioDevice audio;

    raylib::Music music("audio/nice_guys.mp3");
    PlayMusicStream(music);

    raylib::Sound ping("audio/ping.wav");
    
    PingButton += [&ping](){
        ping.Play();
    };

    raylib::Sound crowd("audio/crowd.wav");
    PlaySound(crowd);

    raylib::BufferedInput inputs;
    
    inputs["ping"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([]{
        PingButton();
    }).move();

    auto guiState = InitGuiVolumeControl();

    while(!window.ShouldClose()){
        inputs.PollEvents();
        UpdateMusicStream(music);
        SetMusicVolume(music, guiState.MusicSliderValue);
        SetSoundVolume(ping, guiState.SFXSliderValue);
        SetSoundVolume(crowd, guiState.DialogueSliderValue);

        window.BeginDrawing();
        {
            window.ClearBackground(raylib::Color::Black());
            GuiVolumeControl(&guiState);
        }
        window.EndDrawing();
    }

    return 0;
}

#define GUI_VOLUMECONTROL_IMPLEMENTATION
#include "VolumeControl.h"