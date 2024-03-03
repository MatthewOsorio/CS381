//Matthew Osorio
//Assingment 4: Custom Ad Hoc Game
//3 March 2024

#include "raylib-cpp.hpp"
#include "skybox.hpp"

//function responsible for transforming the models
template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

//function responsible for drawing the models
void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    //model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

//function responsible for drawing the rocks on the scence
void drawRocks(raylib::Model& rocks){

    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({100,0, 100});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-100,0, -50});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({170,0, -150});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-210,0, -5});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({50,0, -300});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-375,0, -220});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-301,0, -410});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({350,0, -200});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-10,0, -285});
    });
    DrawBoundedModel(rocks, [](raylib::Transform t) -> raylib::Transform{
        return t.Translate({-100,0, -350});
    });
}

//resposible for creating and implementing all entities/variables and running the game itself
int main(){
    //creating and initializing all the entities/variables
    raylib::Window window(700, 550, "CS381 - Assingment 4");
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
    raylib::Texture water("textures/water.jpeg");
    water.SetFilter(TEXTURE_FILTER_BILINEAR);
    water.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;
    
    raylib::Model boat ("meshes/boat.glb");
    boat.transform= raylib::Transform(boat.transform).Scale(10 ,10, 10).RotateY(30);

    raylib::Model rock ("meshes/stylized_rocks.glb");
    rock.transform= raylib::Transform(rock.transform).Scale(10 ,10, 10);

    raylib::Model amongus("meshes/among_us_character.glb");
    amongus.transform = raylib::Transform(amongus.transform).Scale(0.5,0.5,0.5);

    raylib::Model dock("meshes/old_bridge.glb");
    dock.transform = raylib::Transform(dock.transform).Scale(0.25 ,0.25 , 2);

    float speed = 0, targetSpeed= 0, acceleration=20;
    raylib::Vector3 boatPos = {-350, 5, 0};
    raylib::Degree heading = 0, targetHeading=0;

    //running the game window
    while(!window.ShouldClose()){
        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                //draw skybox and grond plane
                skybox.Draw();
                ground.Draw({0,0,0});
                
                //draw the boat at the right of the screen
                DrawBoundedModel(boat, [&boatPos, &heading](raylib::Transform t) -> raylib::Transform{
                    return t.Translate(boatPos).RotateY(heading);
                });  
                
                //fuction call to draw rocks
                drawRocks(rock);

                //draw the dock/destination
                DrawBoundedModel(dock, [](raylib::Transform t) -> raylib::Transform{
                    return t.Translate({250,0,0});
                });  

                //if left shift is down the acceleration becomes double
                if(IsKeyDown(KEY_LEFT_SHIFT)){
                    acceleration=40;
                }
                else
                {
                    acceleration=20;
                }
                //checkpoint 1 for boat, if boat reaches checkpoint draw among us character
                if(boatPos.x > -300){
                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({-300,1, 25});
                    }); 

                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({-25 ,1, -40});
                    });
                }

                //checkpoint 2 for boat, if boat reaches checkpoint draw among us character
                if(boatPos.x > -200){
                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({-100,1, -140});
                    }); 

                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({20 ,1, -210});
                    });
                }

                //checkpoint 3 for boat, if boat reaches checkpoint draw among us character
                if(boatPos.x > 0){
                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({25,1, -350});
                    }); 

                    DrawBoundedModel(amongus, [](raylib::Transform t) -> raylib::Transform{
                        return t.Translate({100 ,1, -200});
                    });
                    
                }

                //this part is responsible for making the boat move
                raylib::Vector3 velocity = {speed* cos(heading.RadianValue()), 0 , -speed *sin(heading.RadianValue())};
                boatPos += velocity * window.GetFrameTime();

                //checks if boat reaches the destination, if it does the boat stops
                if(boatPos.x > 200){
                    targetSpeed=0;
                    speed=0;
                    boatPos.x=200;
                }

                //recieve input and update the target speed/ target heading of the boat
                if(IsKeyPressed(KEY_W)){
                    targetSpeed +=10;
                }
                if(IsKeyPressed(KEY_S))
                {
                    targetSpeed -=10;
                }
                if(IsKeyPressed(KEY_A)){
                    targetHeading += 10;
                }
                if(IsKeyPressed(KEY_D)){
                    targetHeading -= 10;
                }
                //adjust the speed/heading of the boat according to the target speed and target heading
                if(targetSpeed < speed){
                    speed -= acceleration * window.GetFrameTime();
                }
                if(targetSpeed > speed){
                    speed += acceleration * window.GetFrameTime();
                }
                if(targetHeading < heading){
                    heading -= acceleration * window.GetFrameTime();
                }
                if(targetHeading > heading){
                    heading += acceleration * window.GetFrameTime();
                }

            camera.EndMode();
        window.EndDrawing();
        //end of game
    }
    return 0;
}