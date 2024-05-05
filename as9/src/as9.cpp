#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include "delegate.hpp"
#include <BufferedInput.hpp>
#include "ECS.hpp"

cs381::Delegate<void()> Forward;
cs381::Delegate<void()> Back;
cs381::Delegate<void()> Right;
cs381::Delegate<void()> Left;
cs381::Delegate<void()> Stop;
cs381::Delegate<void()> Start;

template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

struct RenderingComponent{
    raylib::Model* model;
};

struct TransformComponent{
    raylib::Vector3 position;
    raylib::Degree heading;
};

struct VelocityComponent{
    float speed;
    raylib::Vector3 velocity;
};

struct Physics2D{
    raylib::Degree targetHeading;
    float targetSpeed ,acceleration, turningRate;
};

struct EnemyComponent{
    bool isEnemy;
};

struct ProximityComponent{
    bool inRange;
};

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    //model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

void KinematicsSystem(cs381::Scene<>& scene, float dt){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(scene.HasComponent<VelocityComponent>(e) && scene.HasComponent<TransformComponent>(e)){
            auto& velocity= scene.GetComponent<VelocityComponent>(e);
            auto& transform= scene.GetComponent<TransformComponent>(e);

            raylib::Vector3 tempVelocity = {velocity.speed * cos(transform.heading.RadianValue()), 0, -velocity.speed * sin(transform.heading.RadianValue())};
            transform.position += tempVelocity * dt;
            velocity.velocity = tempVelocity;  
        }
    }
};

void Physics2DSystem(cs381::Scene<>& scene, float dt){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(scene.HasComponent<Physics2D>(e) && scene.HasComponent<VelocityComponent>(e) && scene.HasComponent<TransformComponent>(e)){
            auto& velocity= scene.GetComponent<VelocityComponent>(e);
            auto& physics= scene.GetComponent<Physics2D>(e);
            auto& transform= scene.GetComponent<TransformComponent>(e);

            if(velocity.speed < physics.targetSpeed){
                velocity.speed += physics.acceleration * dt;
            }
            if(velocity.speed > physics.targetSpeed){
                velocity.speed -= physics.acceleration * dt;
            }
            if(transform.heading < physics.targetHeading){
                transform.heading += physics.turningRate * dt;
            }
            if(transform.heading > physics.targetHeading){
                transform.heading -= physics.turningRate * dt;
            }
        }
    }
}

void ProximitySystem(cs381::Scene<>& scene, cs381::Entity& user, cs381::Entity& e){

    auto& player= scene.GetComponent<TransformComponent>(user);
    auto& entity= scene.GetComponent<TransformComponent>(e);

    auto playerX= player.position.GetX();
    auto playerZ= player.position.GetZ();

    auto entityX= entity.position.GetX();
    auto entityZ= entity.position.GetZ();

    auto distance= abs(playerX - entityX) + abs(playerZ - entityZ);
    
    if(distance <= 15){
        scene.GetComponent<ProximityComponent>(user).inRange= true;
        scene.GetComponent<ProximityComponent>(e).inRange= true;
    }
    else{
        scene.GetComponent<ProximityComponent>(user).inRange= false;
        scene.GetComponent<ProximityComponent>(e).inRange= false;
    }
}

void GameActiveSystem(cs381::Scene<>& scene, cs381::Entity& user, cs381::Entity& e, bool& won, bool& lost){
    if(scene.GetComponent<ProximityComponent>(user).inRange && scene.GetComponent<ProximityComponent>(e).inRange && !scene.GetComponent<EnemyComponent>(e).isEnemy){
        won= true;
    }
    if(scene.GetComponent<ProximityComponent>(user).inRange && scene.GetComponent<ProximityComponent>(e).inRange && scene.GetComponent<EnemyComponent>(e).isEnemy){
        lost= true;
    }
}

void DrawSystem(cs381::Scene<>& scene){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(scene.HasComponent<RenderingComponent>(e) && scene.HasComponent<TransformComponent>(e)){
            auto& rendering= scene.GetComponent<RenderingComponent>(e);
            auto& transform= scene.GetComponent<TransformComponent>(e);

            auto position= transform.position;
            auto heading= transform.heading;

            DrawBoundedModel(*rendering.model, [&position, &heading](raylib::Transform t) -> raylib::Transform{
            return t.Translate(position).RotateY(heading);
            });     
        }    
    }
}

void Input(cs381::Scene<>& scene, cs381::Entity& selected, int value, char action){
    if(scene.HasComponent<Physics2D>(selected)){
        auto& speed= scene.GetComponent<VelocityComponent>(selected);
        auto& physics= scene.GetComponent<Physics2D>(selected);

        switch(action){
            case 's':
                if(value > 0){
                    physics.targetSpeed += 10;
                }
                else{
                    physics.targetSpeed -= 10;
                }
                break;
            case 'h':
                if(value > 0){
                    physics.targetHeading += 10;
                }
                else{
                    physics.targetHeading -= 10;
                }
                break;
            default:
                break;
        }
    }
    else{
        return;
    }
}

int main(){
    raylib:: Window window (1200, 900, "CS381 - AS9");

    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 300),
        raylib::Vector3::Up(),
        45.0f,
        CAMERA_PERSPECTIVE
    );

    cs381::SkyBox skybox("textures/skybox.png");
    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50,25);
    raylib::Model ground = ((raylib::Mesh*)&mesh) -> LoadModelFrom();
    raylib::Texture ashpalt("textures/Asphalt-Mixing-Plant.jpg");
    ashpalt.SetFilter(TEXTURE_FILTER_BILINEAR);
    ashpalt.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = ashpalt;
    
    cs381::Scene scene;
    raylib::BufferedInput inputs;
    raylib::AudioDevice audio;
    raylib::Text startText;
    int pos= 1, neg=-1;
    char speed = 's', heading = 'h';
    bool gameWon= false, gameOver= false, startGame= false;

    raylib::Model rambo("meshes/john_rambo.glb");
    raylib::Model girl("meshes/little_girl.glb");
    raylib::Model monster("meshes/among_us_character.glb");
    raylib::Model package("meshes/drugs.glb");
    raylib::Music music("audio/gta4_theme_song.mp3");
    PlayMusicStream(music);
    
    auto user= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(user) = {raylib::Vector3(-250, 20, 0), raylib::Degree (0)};
    scene.AddComponent<RenderingComponent>(user) = {&rambo};
    scene.GetComponent<RenderingComponent>(user).model->transform = raylib::Transform (scene.GetComponent<RenderingComponent>(user).model->transform).Scale(20.0, 20.0, 20.0).RotateX(raylib::Degree(180)).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(user) = {raylib::Degree(0), 0.0, 20.0, 20.0};
    scene.AddComponent<VelocityComponent>(user) = {0.0, raylib::Vector3::Zero()};
    scene.AddComponent<ProximityComponent>(user) = {false};
    scene.AddComponent<EnemyComponent>(user) = {false};
    
    auto hostage= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(hostage) = {raylib::Vector3(250, 0, 0), raylib::Degree (-90)};
    scene.AddComponent<RenderingComponent>(hostage) = {&girl};
    scene.GetComponent<RenderingComponent>(hostage).model->transform = raylib::Transform (scene.GetComponent<RenderingComponent>(hostage).model->transform).Scale(30.0, 30.0, 30.0);
    scene.AddComponent<ProximityComponent>(hostage) = {false};
    scene.AddComponent<EnemyComponent>(hostage) = {false};

    auto monster1= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster1) = {raylib::Vector3(100, 0 ,0), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster1) = {&monster};
    scene.GetComponent<RenderingComponent>(monster1).model->transform= raylib::Transform (scene.GetComponent<RenderingComponent>(monster1).model->transform).Scale(0.25 , 0.25 , 0.25);
    scene.AddComponent<ProximityComponent>(monster1) = {false};
    scene.AddComponent<EnemyComponent>(monster1) = {true};

    auto monster2= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster2) = {raylib::Vector3(60 ,0, -200), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster2) = {&monster};
    scene.AddComponent<ProximityComponent>(monster2) = {false};
    scene.AddComponent<EnemyComponent>(monster2) = {true};

    auto monster3= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster3) = {raylib::Vector3(-90 ,0, -220), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster3) = {&monster};
    scene.AddComponent<ProximityComponent>(monster3) = {false};
    scene.AddComponent<EnemyComponent>(monster3) = {true};

    auto monster4= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster4) = {raylib::Vector3(-10,0, -285), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster4) = {&monster};
    scene.AddComponent<ProximityComponent>(monster4) = {false};
    scene.AddComponent<EnemyComponent>(monster4) = {true};

    auto monster5= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster5) = {raylib::Vector3(-30 ,0, -150), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster5) = {&monster};
    scene.AddComponent<ProximityComponent>(monster5) = {false};
    scene.AddComponent<EnemyComponent>(monster5) = {true};

    auto monster6= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster6) = {raylib::Vector3(-80 ,0, 205), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster6) = {&monster};
    scene.AddComponent<ProximityComponent>(monster6) = {false};
    scene.AddComponent<EnemyComponent>(monster6) = {true};

    auto monster7= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster7) = {raylib::Vector3(-150 ,0, 30), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster7) = {&monster};
    scene.AddComponent<ProximityComponent>(monster7) = {false};
    scene.AddComponent<EnemyComponent>(monster7) = {true};

    auto monster8= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster8) = {raylib::Vector3(150 ,0, 75), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster8) = {&monster};
    scene.AddComponent<ProximityComponent>(monster8) = {false};
    scene.AddComponent<EnemyComponent>(monster8) = {true};

    auto monster10= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster10) = {raylib::Vector3(160 ,0, -100), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster10) = {&monster};
    scene.AddComponent<ProximityComponent>(monster10) = {false};
    scene.AddComponent<EnemyComponent>(monster10) = {true};

    auto monster11= scene.CreateEntity();
    scene.AddComponent<TransformComponent>(monster11) = {raylib::Vector3(10 ,0, 10), raylib::Degree(0)};
    scene.AddComponent<RenderingComponent>(monster11) = {&monster};
    scene.AddComponent<ProximityComponent>(monster11) = {false};
    scene.AddComponent<EnemyComponent>(monster11) = {true};

    Forward += [&](){
        Input(scene, user, pos, speed);
    };

    Back += [&](){
        Input(scene, user, neg, speed);
    };

    Right += [&]{
        Input(scene, user, pos, heading);
    };

    Left += [&]{
        Input(scene, user, neg, heading);
    };

    Stop += [&]{
        scene.GetComponent<Physics2D>(user).targetSpeed = 0;
    };

    Start += [&]{
        startGame= true;
    };

    inputs["forward"] = raylib::Action::key(KEY_W).AddPressedCallback([]{
        Forward();
    }).move();

    inputs["back"] = raylib::Action::key(KEY_S).AddPressedCallback([]{
        Back();
    }).move();

    inputs["right"] = raylib::Action::key(KEY_D).AddPressedCallback([]{
        Right();
    }).move();

    inputs["left"] = raylib::Action::key(KEY_A).AddPressedCallback([]{
        Left();
    }).move();

    inputs["stop"] = raylib::Action::key(KEY_SPACE).AddPressedCallback([]{
        Stop();
    }).move();

    inputs["start"] = raylib::Action::key(KEY_ENTER).AddPressedCallback([]{
        Start();
    }).move();

    while(!window.ShouldClose()){
        inputs.PollEvents();

        if(!startGame){
            window.BeginDrawing();
            window.ClearBackground(raylib::Color::Black());

            startText.Draw("SAVE THE GIRL AND AVOID THE MONSTERS \n PRESS ENTER TO PLAY", window.GetWidth()/6, window.GetHeight()/2, 30.0, raylib::Color::Red());

            window.EndDrawing();
        }
        else if(gameWon){
            window.BeginDrawing();
            window.ClearBackground(raylib::Color::Black());

            startText.Draw("GOOD JOB YOU WON \n HOSTAGE HAS BEEN SAVED", window.GetWidth()/6, window.GetHeight()/2, 30.0, raylib::Color::Red());

            window.EndDrawing();
        }
        else if(gameOver){
            window.BeginDrawing();
            window.ClearBackground(raylib::Color::Black());

            startText.Draw("GOT TOO CLOSE TO MONSTERS \n TRY AGAIN", window.GetWidth()/6, window.GetHeight()/2, 30.0, raylib::Color::Red());

            window.EndDrawing();
        }
        else{
            UpdateMusicStream(music);
            Physics2DSystem(scene, window.GetFrameTime());
            KinematicsSystem(scene, window.GetFrameTime());
            
            for(cs381::Entity e= 0; e < scene.entityMasks.size() && !gameWon &&!gameOver; e++){
                if(e == user) continue;
                ProximitySystem(scene, user, e);
                GameActiveSystem(scene, user, e, gameWon, gameOver);
            }
                window.ClearBackground(raylib::Color::Black());
                window.BeginDrawing();

                    camera.BeginMode();
                        skybox.Draw();
                        ground.Draw({0,0,0}); 

                        DrawSystem(scene);  

                    camera.EndMode();
                window.EndDrawing();
            }
    }
}