#include "raylib-cpp.hpp"
#include "skybox.hpp"
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

    
    raylib::Model amongus("meshes/among_us_character.glb");
    amongus.transform= raylib::Transform(amongus.transform).Scale(1,1,1);
    
    raylib::Model plane("meshes/PolyPlane.glb");
    plane.transform= raylib::Transform(plane.transform).Scale(1,1,1);
    float speed= 5;
    raylib::Vector3 position = {0, 0, 0};
    raylib::Degree heading = 60;

    while(!window.ShouldClose()){
        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                DrawBoundedModel(plane, [&position, &heading](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position).RotateY(heading);
                });

                DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({0,0,-50});
                });

                DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({100 ,0, 70});
                });

                DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({-100 ,0, 70});
                });

                raylib::Vector3 velocity = {speed* cos(heading.RadianValue()), 0 , -speed *sin(heading.RadianValue())};
                position += velocity * window.GetFrameTime();
            
                if(IsKeyPressed(KEY_W)){
                    position.z += 1;
                    heading += 1;
                    speed +=1;
                }
                if(IsKeyPressed(KEY_S)){
                    position.z -=1;
                    heading -= 1;
                    speed +=1;
                }
                if(IsKeyPressed(KEY_A)){
                    position.x +=1;
                    heading += 1;
                    speed +=1;
                }
                if(IsKeyPressed(KEY_D)){
                    position.x -=1;
                    heading -= 1;
                    speed += 1;
                }
                if(IsKeyPressed(KEY_Q)){
                    position.y +=1;
                    speed += 1;
                }
                if(IsKeyPressed(KEY_E)){
                    position.y -=1;
                    speed += 1;
                }

            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}