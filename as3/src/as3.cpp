#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include <iostream>

enum planes{p3, p1, p2};

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
    
    raylib::Model plane1("meshes/PolyPlane.glb");
    plane1.transform= raylib::Transform(plane1.transform).Scale(1,1,1);

    raylib::Model plane2("meshes/PolyPlane.glb");
    plane2.transform= raylib::Transform(plane2.transform).Scale(1,1,1);

    raylib::Model plane3("meshes/PolyPlane.glb");
    plane3.transform= raylib::Transform(plane3.transform).Scale(1,1,1);

    float acceleration = 30;

    float speed1= 1, targetSpeed1 =1;
    raylib::Vector3 position1 = {0, 0, 0};
    raylib::Degree heading1 = 0, targetHeading1=0;

    raylib::Vector3 position2 = {50, 0, 0};
    raylib::Degree heading2=0, targetHeading2 = 0;
    float speed2=1, targetSpeed2= 1;

    raylib::Vector3 position3 = {-50, 0, 0};
    raylib::Degree heading3=0, targetHeading3 = 0;
    float speed3=1, targetSpeed3= 1;

    const raylib::Vector3 zero = {0,0,0};
    int counter=0;

    while(!window.ShouldClose()){
        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                DrawBoundedModel(plane1, [&position1, &heading1](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position1).RotateY(heading1);
                });

                DrawBoundedModel(plane2, [&position2, &heading2](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position2).RotateY(heading2);
                });

                DrawBoundedModel(plane3, [&position3, &heading3](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position3).RotateY(heading3);
                });

                raylib::Vector3 velocity = {speed1* cos(heading1.RadianValue()), 0 , -speed1 *sin(heading1.RadianValue())};
                position1 += velocity * window.GetFrameTime();

                raylib::Vector3 velocity2 = {speed2* cos(heading2.RadianValue()), 0 , -speed2 *sin(heading2.RadianValue())};
                position2 += velocity2 * window.GetFrameTime();

                raylib::Vector3 velocity3 = {speed3* cos(heading3.RadianValue()), 0 , -speed3 *sin(heading3.RadianValue())};
                position3 += velocity3 * window.GetFrameTime();


                if(IsKeyPressed(KEY_TAB)){
                    counter++;
                }

                switch((counter %3 )){
                    case p3:
                        plane3.GetTransformedBoundingBox().Draw();

                        if(IsKeyPressed(KEY_W)){
                            targetSpeed3 += 10;
                        }
                        if(IsKeyPressed(KEY_S)){
                            targetSpeed3 -= 10;
                        }
                        if(IsKeyPressed(KEY_A)){
                            targetHeading3 +=10;
                        }
                        if(IsKeyPressed(KEY_D)){
                            targetHeading3 -=10;
                        }

                        if(targetSpeed3 > speed3){
                            speed3 += acceleration * window.GetFrameTime();
                        }
                        if(targetSpeed3 < speed3){
                            speed3 -= acceleration * window.GetFrameTime();
                        }
                        
                        if(targetHeading3 > heading3){
                            heading3 += acceleration * window.GetFrameTime();
                        }
                        if(targetHeading3 < heading3){
                            heading3 -= acceleration * window.GetFrameTime();
                        }

                        if(IsKeyPressed(KEY_SPACE)){
                            targetSpeed3=0;
                        }
                        break;
                    case p1:
                        plane1.GetTransformedBoundingBox().Draw();
                        if(IsKeyPressed(KEY_W)){
                            targetSpeed1 += 10;
                        }
                        if(IsKeyPressed(KEY_S)){
                            targetSpeed1 -= 10;
                        }
                        if(IsKeyPressed(KEY_A)){
                            targetHeading1 +=10;
                        }
                        if(IsKeyPressed(KEY_D)){
                            targetHeading1 -=10;
                        }

                        if(targetSpeed1 > speed1){
                            speed1 += acceleration * window.GetFrameTime();
                        }
                        if(targetSpeed1 < speed1){
                            speed1 -= acceleration * window.GetFrameTime();
                        }
                        
                        if(targetHeading1 > heading1){
                            heading1 += acceleration * window.GetFrameTime();
                        }
                        if(targetHeading1 < heading1){
                            heading1 -= acceleration * window.GetFrameTime();
                        }

                        if(IsKeyPressed(KEY_SPACE)){
                            targetSpeed1=0;
                        }
                        break;
                    case p2:
                        plane2.GetTransformedBoundingBox().Draw();

                        if(IsKeyPressed(KEY_W)){
                            targetSpeed2 += 10;
                        }
                        if(IsKeyPressed(KEY_S)){
                            targetSpeed2 -= 10;
                        }
                        if(IsKeyPressed(KEY_A)){
                            targetHeading2 +=10;
                        }
                        if(IsKeyPressed(KEY_D)){
                            targetHeading2 -=10;
                        }

                        if(targetSpeed2 > speed2){
                            speed2 += acceleration * window.GetFrameTime();
                        }
                        if(targetSpeed2 < speed2){
                            speed2 -= acceleration * window.GetFrameTime();
                        }
                        
                        if(targetHeading2 > heading2){
                            heading2 += acceleration * window.GetFrameTime();
                        }
                        if(targetHeading2 < heading2){
                            heading2 -= acceleration * window.GetFrameTime();
                        }

                        if(IsKeyPressed(KEY_SPACE)){
                            targetSpeed2=0;
                        }
                        break;
                    default:
                        break;
                }
            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}