#include "raylib-cpp.hpp"

int main(){
    raylib::Window window(700, 550, "CS381 - Assingment 1");
    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );
    raylib::Mesh shipMesh();
    raylib::Model ship = LoadModelFromMesh(raylib::Mesh ship());
    while(!window.ShouldClose()){
        window.BeginDrawing();
            camera.BeginMode();
                ship.Draw(
                    raylib::Vector3(0,0,0),
                    100,
                    BLUE
                );

            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}