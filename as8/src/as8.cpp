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
cs381::Delegate<void ()> Select;

enum{s1, s2, s3, s4, s5, p1, p2, p3, p4, p5};

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

struct RenderingComponent{
    raylib::Model* model;
    bool drawBoundingBox;
};

struct TransfromComponent{
    raylib::Vector3 position;
    raylib::Transform transform;
};

struct VelocityComponent{
    float speed, targetSpeed, maxSpeed;
    raylib::Vector3 velocity;
};

struct Physics2D{
    raylib::Degree heading, targetHeading;
    float turningRate, acceleration;
};
struct Physics3D{
    raylib::Quaternion rotation;
};

void kinematicsSystem2D(cs381::Scene<>& scene, float dt){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(!scene.HasComponent<VelocityComponent>(e) && !scene.HasComponent<Physics2D>(e)) continue;

        auto& velocity= scene.GetComponent<VelocityComponent>(e);
        auto& transform= scene.GetComponent<TransfromComponent>(e);
        auto& physics=scene.GetComponent<Physics2D>(e);

        raylib::Vector3 tempVelocity = {velocity.speed * cos(physics.heading.RadianValue()), 0, -velocity.speed * sin(physics.heading.RadianValue())};
        transform.position += tempVelocity * dt;
        velocity.velocity = tempVelocity;
    }
};
/*
void kinematicsSystem3D(cs381::Scene<>& scene, float dt){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(!scene.HasComponent<VelocityComponent>(e) && !scene.HasComponent<Physics3D>(e)) continue;

        auto& velocity= scene.GetComponent<VelocityComponent>(e);
        auto& transform= scene.GetComponent<TransfromComponent>(e);
        auto& physics=scene.GetComponent<Physics3D>(e);

        auto position= transform.position;
        auto [axis, angle]= physics.rotation.ToAxisAngle();
        auto tempAngle= angle;

        raylib::Vector3 tempVelocity = {velocity.speed * cos(tempAngle), 0, -velocity.speed * sin(tempAngle)};
        transform.position += tempVelocity * dt;
        velocity.velocity = tempVelocity;
    }
};
*/
void Physics2DSystem(cs381::Scene<>& scene, float dt){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(scene.HasComponent<Physics2D>(e) && scene.HasComponent<VelocityComponent>(e)){
            auto& velocity= scene.GetComponent<VelocityComponent>(e);
            auto& physics= scene.GetComponent<Physics2D>(e);
            auto& transform= scene.GetComponent<TransfromComponent>(e);

            if(velocity.speed < velocity.targetSpeed){
                velocity.speed += physics.acceleration * dt;
            }
            if(velocity.speed > velocity.targetSpeed){
                velocity.speed -= physics.acceleration * dt;
            }
            if(physics.heading < physics.targetHeading){
                physics.heading += physics.turningRate * dt;
            }
            if(physics.heading > physics.targetHeading){
                physics.heading -= physics.turningRate * dt;
            }
        }
    }
}

void input2D(cs381::Scene<>& scene, cs381::Entity& selected, int value, char action){

    if(scene.HasComponent<Physics2D>(selected)){
        auto& speed= scene.GetComponent<VelocityComponent>(selected);
        auto& physics= scene.GetComponent<Physics2D>(selected);

        switch(action){
            case 's':
                if(value > 0){
                    speed.targetSpeed += 5;
                    std::cout<<"forward"<<std::endl;
                }
                else{
                    speed.targetSpeed -= 5;
                    std::cout<<"back"<<std::endl;
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

void DrawSystem2D(cs381::Scene<>& scene){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(scene.HasComponent<RenderingComponent>(e) && scene.HasComponent<Physics2D>(e)){
            auto& rendering= scene.GetComponent<RenderingComponent>(e);
            auto& transform= scene.GetComponent<TransfromComponent>(e);
            auto& physics= scene.GetComponent<Physics2D>(e);

            auto position= transform.position;
            auto heading= physics.heading;

            if(rendering.drawBoundingBox){
                DrawBoundedModel(*rendering.model, [&position, &heading](raylib::Transform t) -> raylib::Transform{
                return t.Translate(position).RotateY(heading);
                });
            }
            else{
                DrawModel(*rendering.model, [&position, &heading](raylib::Transform t) -> raylib::Transform{
                return t.Translate(position).RotateY(heading);
                });
            } 
        }       
    }
}

void DrawSystem3D(cs381::Scene<>& scene){
    for(cs381::Entity e= 0; e < scene.entityMasks.size(); e++){
        if(!scene.HasComponent<Physics3D>(e)) continue;
        auto& rendering= scene.GetComponent<RenderingComponent>(e);
        auto& physics= scene.GetComponent<Physics3D>(e);
        auto& transform= scene.GetComponent<TransfromComponent>(e);

        auto position= transform.position;
        auto [axis, angle]= physics.rotation.ToAxisAngle();
        auto tempAngle= angle;

        if(rendering.drawBoundingBox){
            DrawBoundedModel(*rendering.model, [&position, &tempAngle](raylib::Transform t) -> raylib::Transform{
                return t.Translate(position).RotateY(tempAngle);
            });
        }
        else{
            DrawModel(*rendering.model, [&position, &tempAngle](raylib::Transform t) -> raylib::Transform{
                return t.Translate(position).RotateY(tempAngle);
            });
        }
    }
}

void selection(cs381::Scene<>& scene, cs381::Entity& selected){
    for(cs381::Entity e= 0; e< scene.entityMasks.size(); e++){
        if(!scene.HasComponent<RenderingComponent>(e)) continue;
        auto& rendering= scene.GetComponent<RenderingComponent>(e);
        
        if(selected == e ){
            rendering.drawBoundingBox= true;
        }
        else{
            rendering.drawBoundingBox= false;
        }
    }
}

int main(){
    raylib:: Window window (1200, 900, "CS381 - AS8");

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
    raylib::Texture water("textures/water.jpeg");
    water.SetFilter(TEXTURE_FILTER_BILINEAR);
    water.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

    cs381::Scene scene;
    raylib::Model ship1("meshes/SmitHouston_Tug.glb");
    raylib::Model ship2("meshes/boat.glb");
    raylib::Model ship3("meshes/ddg51.glb");
    raylib::Model ship4("meshes/Container_ShipLarge.glb");
    raylib::Model ship5("meshes/u-_boat.glb");
    raylib::Model plane("meshes/PolyPlane.glb");

    auto e1 = scene.CreateEntity();    
    scene.AddComponent<RenderingComponent>(e1) = {&ship1, false};
    scene.AddComponent<TransfromComponent>(e1)= {raylib::Vector3 (200, 0, 0), ship1.GetTransform()};
    scene.GetComponent<RenderingComponent>(e1).model->transform = raylib::Transform(scene.GetComponent<RenderingComponent>(e1).model->transform).Scale(1.5, 1.5, 1.5).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(e1)={raylib::Degree(0), raylib::Degree(0), 20.0, 20.0};
    scene.AddComponent<VelocityComponent>(e1)={0.0, 0.0, 0.0, raylib::Vector3::Zero()};

    auto e2= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e2) = {&ship2, false};
    scene.AddComponent<TransfromComponent>(e2) = {raylib::Vector3 (75, 20, 75), ship2.GetTransform()};
    scene.GetComponent<RenderingComponent>(e2).model->transform = raylib::Transform(scene.GetComponent<RenderingComponent>(e2).model->transform).Scale(7, 7, 7).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(e2)= {raylib::Degree(0), raylib::Degree(0), 12.0, 12.0};
    scene.AddComponent<VelocityComponent>(e2) = {0.0, 0.0, 25.0, raylib::Vector3::Zero()};
    
    auto e3= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e3) = {&ship3, false};
    scene.AddComponent<TransfromComponent>(e3) = {raylib::Vector3 (-200, 20, 0), ship3.GetTransform()};
    scene.GetComponent<RenderingComponent>(e3).model->transform = raylib::Transform(scene.GetComponent<RenderingComponent>(e3).model->transform).RotateX(raylib::Degree (90)).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(e3)= {raylib::Degree(0), raylib::Degree(0), 7.0, 7.0};
    scene.AddComponent<VelocityComponent>(e3) = {0.0, 0.0, 20.0, raylib::Vector3::Zero()};
    
    auto e4= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e4) = {&ship4, false};
    scene.AddComponent<TransfromComponent>(e4) = {raylib::Vector3 (-100, 20, 100), ship4.GetTransform()};
    scene.GetComponent<RenderingComponent>(e4).model->transform = raylib::Transform(scene.GetComponent<RenderingComponent>(e4).model->transform).RotateX(raylib::Degree (90)).Scale(0.005, 0.005, 0.005).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(e4)= {raylib::Degree(0), raylib::Degree(0), 5.0, 5.0};
    scene.AddComponent<VelocityComponent>(e4) = {0.0, 0.0, 15.0, raylib::Vector3::Zero()};
    
    auto e5= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e5) = {&ship5, false};
    scene.AddComponent<TransfromComponent>(e5) = {raylib::Vector3 (25, 20, -100), ship5.GetTransform()};
    scene.GetComponent<RenderingComponent>(e5).model->transform = raylib::Transform(scene.GetComponent<RenderingComponent>(e5).model->transform).RotateX(raylib::Degree (-90)).Scale(3.0, 3.0, 3.0).RotateY(raylib::Degree(90));
    scene.AddComponent<Physics2D>(e5)= {raylib::Degree(0), raylib::Degree(0), 30.0, 30.0};
    scene.AddComponent<VelocityComponent>(e5) = {0.0, 0.0, 35.0, raylib::Vector3::Zero()};

    auto e6= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e6)= {&plane, false};
    scene.AddComponent<TransfromComponent>(e6)= {raylib::Vector3 (25, 75, -100), plane.GetTransform()};
    scene.AddComponent<Physics3D>(e6) = {raylib::Quaternion::FromMatrix(scene.GetComponent<TransfromComponent>(e6).transform)};
    //scene.AddComponent<VelocityComponent>(e6) = {0.0, 0.0, 40.0, raylib::Vector3::Zero()};

    auto e7= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e7)= {&plane, false};
    scene.AddComponent<TransfromComponent>(e7)= {raylib::Vector3 (-100, 75, 100), plane.GetTransform()};
    scene.GetComponent<RenderingComponent>(e7).model->transform= raylib::Transform(scene.GetComponent<RenderingComponent>(e7).model->transform).RotateY(raylib::Degree (180));
    scene.AddComponent<Physics3D>(e7) = {raylib::Quaternion::FromMatrix(scene.GetComponent<TransfromComponent>(e7).transform)};
    //scene.AddComponent<VelocityComponent>(e6) = {0.0, 0.0, 40.0, raylib::Vector3::Zero()};

    auto e8= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e8)= {&plane, false};
    scene.AddComponent<TransfromComponent>(e8)= {raylib::Vector3 (-200, 75, 0), plane.GetTransform()};
    scene.AddComponent<Physics3D>(e8) = {raylib::Quaternion::FromMatrix(scene.GetComponent<TransfromComponent>(e8).transform)};
    //scene.AddComponent<VelocityComponent>(e6) = {0.0, 0.0, 40.0, raylib::Vector3::Zero()};
    
    auto e9= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e9)= {&plane, false};
    scene.AddComponent<TransfromComponent>(e9)= {raylib::Vector3 (75, 75, 75), plane.GetTransform()};
    scene.AddComponent<Physics3D>(e9) = {raylib::Quaternion::FromMatrix(scene.GetComponent<TransfromComponent>(e9).transform)};
    //scene.AddComponent<VelocityComponent>(e6) = {0.0, 0.0, 40.0, raylib::Vector3::Zero()};

    auto e10= scene.CreateEntity();
    scene.AddComponent<RenderingComponent>(e10)= {&plane, false};
    scene.AddComponent<TransfromComponent>(e10)= {raylib::Vector3 (200, 75, 0), plane.GetTransform()};
    scene.AddComponent<Physics3D>(e10) = {raylib::Quaternion::FromMatrix(scene.GetComponent<TransfromComponent>(e10).transform)};
    //scene.AddComponent<VelocityComponent>(e6) = {0.0, 0.0, 40.0, raylib::Vector3::Zero()};


    cs381::Entity selectedEntity;
    raylib::BufferedInput inputs;
    int pos= 1, neg=-1;
    char speed = 's', heading = 'h';
    int counter=0;

    Forward += [&](){
        input2D(scene, selectedEntity, pos, speed);
        //add 3D
    };

    Back += [&](){
        input2D(scene, selectedEntity, neg, speed);
        //add 3D
    };

    Right += [&]{
        input2D(scene, selectedEntity, pos, heading);
        //add 3D
    };

    Left += [&]{
        input2D(scene, selectedEntity, neg, heading);
        //add 3D
    };

    Stop += [&]{
        scene.GetComponent<VelocityComponent>(selectedEntity).targetSpeed = 0;
    };

    Select +=[&]{
        counter++;
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

    inputs["select"] = raylib::Action::key(KEY_TAB).AddPressedCallback([]{
        Select();
    }).move();

    while(!window.ShouldClose()){
        inputs.PollEvents();
        Physics2DSystem(scene, window.GetFrameTime());
        kinematicsSystem2D(scene, window.GetFrameTime());
        //kinematicsSystem3D(scene, window.GetFrameTime());

        switch(counter % 10){
            case s5:
                selectedEntity= e5;
                break;
            case s1:
                selectedEntity= e1;
                break;
            case s2:
                selectedEntity= e2;
                break;
            case s3:
                selectedEntity= e3;
                break;
            case s4:
                selectedEntity= e4;
                break;
            case p1:
                selectedEntity= e6;
                break;
            case p2:
                selectedEntity= e7;
                break;
            case p3:
                selectedEntity= e8;
                break;
            case p4:
                selectedEntity= e9;
                break;
            case p5: 
                selectedEntity= e10;
                break;
        }

        selection(scene, selectedEntity);

        window.ClearBackground(raylib::Color::Black());
        window.BeginDrawing();
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                DrawSystem2D(scene);
                DrawSystem3D(scene);

            camera.EndMode();
        window.EndDrawing();
    }

    return 0;
}