#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include <iostream>

struct PolyPlane {
    raylib::Vector3 position;
    raylib::Degree heading;
    float speed;

    PolyPlane(){
        position= 0;
        heading= 0;
        speed= 0;
    }

    PolyPlane(raylib::Vector3 newPos, raylib::Degree newHeading, float newSpeed){
        position= newPos;
        heading= newHeading;
        speed= newSpeed;
    }
};

template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
   // model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

int main(){
    raylib::Window window(700, 550, "CS381 - Assingment 3");
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
    raylib::Model plane2("meshes/PolyPlane.glb");
    raylib::Model plane3("meshes/PolyPlane.glb");

    raylib::Vector3 position1= {0,0,0};
    raylib::Degree heading1= 0;
    float speed1=0;

    raylib::Vector3 position2= {50,0,0};
    raylib::Degree heading2= 0;
    float speed2=0;

    raylib::Vector3 position3= {100,0,0};
    raylib::Degree heading3= 0;
    float speed3=0;

    int counter=0;
    int planeSelected;
    while(!window.ShouldClose()){
        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});
                
                //plane1
                DrawBoundedModel(plane1, [&position1, &heading1](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position1).RotateY(heading1);
                });
                raylib::Vector3 velocity1 = {speed1* cos(heading1.RadianValue()), 0 , -speed1 *sin(heading1.RadianValue())};
                position1 += velocity1 * window.GetFrameTime();

                //plane2
                DrawBoundedModel(plane2, [&position2, &heading2](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position2).RotateY(heading2);
                });
                raylib::Vector3 velocity2 = {speed2* cos(heading2.RadianValue()), 0 , -speed2 *sin(heading2.RadianValue())};
                position2 += velocity2 * window.GetFrameTime();

                //plane3
                DrawBoundedModel(plane3, [&position3, &heading3](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(position3).RotateY(heading3);
                });
                raylib::Vector3 velocity3 = {speed3* cos(heading3.RadianValue()), 0 , -speed3 *sin(heading3.RadianValue())};
                position3 += velocity3 * window.GetFrameTime();
            
                if(IsKeyPressed(KEY_TAB)){
                    counter++;
                    std::cout<<"Time counter has been pressed: "<<counter<< std::endl;
                }
                
                planeSelected= (counter % 3);
                
                int i =0;
                switch(planeSelected){
                    case 1: //plane 1 selected
                        //plane1.GetTransformedBoundingBox().Draw();
                        //speed1 +=1;
                        break;
                    case 2: //plane 2 seleceted
                        //plane2.GetTransformedBoundingBox().Draw();
                        //speed2 +=2;
                        break;
                    case 0: //plane 3 selected
                        //plane3.GetTransformedBoundingBox().Draw();
                        //speed3 += 3;
                        break;
                }
            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}