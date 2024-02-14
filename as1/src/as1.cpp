#include "raylib-cpp.hpp"

template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
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

    raylib::Model plane("meshes/PolyPlane.glb");
    plane.transform= raylib::Transform(plane.transform).Scale(3,3,3);

    raylib::Model ship("meshes/SmitHouston_Tug.glb");
    ship.transform= raylib::Transform(ship.transform).RotateXYZ(raylib::Degree(90),0, raylib::Degree(90));

    raylib::Model rambo("meshes/john_rambo.glb");
    rambo.transform= raylib::Transform(rambo.transform).Scale(150, 50, 150).RotateX(raylib::Degree(180)).RotateY(raylib::Degree(180));

    SetTargetFPS(60);

    while(!window.ShouldClose()){
        int rotation=0;
        window.BeginDrawing();
            camera.BeginMode();
                DrawBoundedModel(plane, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({0,0,0});
                });
                DrawBoundedModel(plane, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({-100,100,0}).Scale(1,-1,1).RotateZ(raylib::Degree(180));
                });
                DrawBoundedModel(ship, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({-200,0,0});
                });
                DrawBoundedModel(ship, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({200,0,0}).RotateZ(raylib::Degree(180));
                });
                DrawBoundedModel(ship, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({100,100,0}).Scale(1,2,1).RotateZ(raylib::Degree(270));
                });
                DrawBoundedModel(rambo, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({-75,0,-200}).RotateX(raylib::Degree(25));
                });
            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}