#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include "ECS.hpp"

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

void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    model.transform = backupTransform;
}

struct Rendering {
    raylib::Model* model;
    bool drawBoundingBox = false;
};

struct Transform{

};

struct Physics {

};



void DrawSystem(cs381::Scene<>& scene){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(!scene.HasComponent<Rendering>(e)) continue;
        //if(!scene.HasComponent<Physics>(e)) continue;

        auto& rendering= scene.GetComponent<Rendering>(e);
        //auto& physics= scene.GetComponent<Physics>(e);

        auto transformer= [](raylib::Transform t) -> raylib::Transform {
            return t;
        };

        if(rendering.drawBoundingBox)
            DrawBoundedModel(*rendering.model, transformer);
        else DrawModel(*rendering.model, transformer);
    }
}

int main(){
    raylib::Window window(700, 550, "CS381 - Assingment 8");
    raylib::Camera camera(
        raylib::Vector3(0, 120, -750),
        raylib::Vector3(0, 0, 0),
        raylib::Vector3::Up(),
        45,
        CAMERA_PERSPECTIVE
    );

    cs381::SkyBox skybox("textures/skybox.png");

    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50,25);
    raylib::Model ground = ((raylib::Mesh*)&mesh) -> LoadModelFrom();
    raylib::Texture grass("textures/grass.jpg");
    grass.SetFilter(TEXTURE_FILTER_BILINEAR);
    grass.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = grass;

    cs381::Scene scene;
    raylib::Model plane("meshes/PolyPlane.glb");

    auto e = scene.CreateEntity();
    scene.AddComponent<Rendering>(e) = {&plane, true};
    auto& model =scene.GetComponent<Rendering>(e).model;
    model->transform= raylib::Transform(model->transform).Scale(5, 5,5 );

    while(!window.ShouldClose()){
        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                DrawSystem(scene);

            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}