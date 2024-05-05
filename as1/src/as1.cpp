#include "raylib-cpp.hpp"

template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    //model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

int main(){
    raylib::Window window(700, 550, "CS381 - Assingment 1");
    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    raylib::Model little_girl("meshes/little_girl.glb");
    little_girl.transform= raylib::Transform(little_girl.transform).Scale(90.0, 90.0, 90.0).RotateY(raylib::Degree(180));

    raylib::Model rambo("meshes/john_rambo.glb");
    rambo.transform= raylib::Transform(rambo.transform).Scale(150, 150, 150).RotateX(raylib::Degree(180)).RotateY(raylib::Degree(-180));

    SetTargetFPS(60);

    while(!window.ShouldClose()){
        int rotation=0;
        window.BeginDrawing();
            camera.BeginMode();
                /*
                DrawBoundedModel(little_girl, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({0,0,0});
                });
                */
                DrawBoundedModel(little_girl, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({100,0,-100});
                });
            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}