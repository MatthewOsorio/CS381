#include "raylib-cpp.hpp"
#include <memory>
#include <iostream>
#include "delegate.hpp"
#include <BufferedInput.hpp>
#include "skybox.hpp"
#include <vector>

cs381::Delegate<void()> Forward;
cs381::Delegate<void()> Back;
cs381::Delegate<void()> Right;
cs381::Delegate<void()> Left;

enum entities{e1, e2, e3, e4, e5, e6, e7, e8, e9, e10};

struct Component {
    struct Entity* object;

    Component(struct Entity& e) : object(&e) {}

    virtual void setup () {}
    virtual void cleanup () {}
    virtual void tick (float dt) {}
};

struct TransformComponent: public Component {
    using Component::Component;
    raylib::Vector3 position = {0,0,0};
    raylib::Quaternion rotation = raylib::Quaternion::Identity();
};

struct Entity{
    std::vector <std::unique_ptr <Component>> components = {};

    Entity() {AddComponent<TransformComponent>();}
    Entity(const Entity&) = delete;
    Entity(Entity&& other) : components(std::move(other.components)) {
        for(auto& c: components)
            c->object = this; 
    }

   template <std::derived_from<Component> T, typename... Ts>
   size_t AddComponent(Ts... args) {
        auto c = std::make_unique<T>(*this, std::forward<Ts>(args)...);
        components.push_back(std::move(c));
        return components.size() -1;
   }
    
    template<std:: derived_from<Component> T>
    std::optional<std::reference_wrapper<T>> GetComponent () {
        if constexpr(std::is_same_v<T,TransformComponent>){
            T* cast = dynamic_cast<T*>(components[0].get());
            if(cast) return *cast;
        }
        for(auto & c: components) {
            T* cast = dynamic_cast<T*>(c.get());
            if(cast) return *cast;
        }

        return std::nullopt;
    }

   void tick(float dt) {
        for(auto& c: components)
            c->tick(dt);
   }
};
struct PhysicsComponent: public Component {
    int acceleration, turningacceleration, speed, targetSpeed;
    raylib::Degree heading;
    raylib::Degree targetHeading;

    PhysicsComponent(Entity& e, int ac, int tc): Component(e), acceleration(std::move(ac)), turningacceleration(std::move(tc)) {
        speed=0;
        targetSpeed=0;
        heading=0;
        targetHeading=0;
    }

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return ;
        auto& transform = ref -> get(); 

        raylib::Vector3 velocity = {speed* cos(heading.RadianValue()), 0 , -speed *sin(heading.RadianValue())};
        transform.position += velocity * dt;

        if(targetSpeed > speed){
            speed += acceleration * dt;
        }
        if(targetSpeed < speed){
            speed -= acceleration * dt;
        }
        if(targetHeading > heading){
            heading += turningacceleration * dt;
        }
        if(targetHeading < heading){
            heading -= turningacceleration * dt;
        }
    }
};

struct InputComponent: public Component {
    raylib::BufferedInput input;
    bool selected;

    InputComponent(Entity&e): Component(e) { }

    void setup() override{
        auto ref = object->GetComponent<PhysicsComponent>();
        if(!ref) return;
        auto& physics = ref -> get();
        int speed = physics.targetSpeed;

        input["forward"] = raylib::Action::key(KEY_SPACE).SetPressedCallback([]{
        
        }).move();
    }
};

struct RenderingComponent: public Component {
    raylib::Model model;

    RenderingComponent(Entity& e, raylib::Model&& model): Component(e), model(std::move(model)) {}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return;
        auto& transform = ref -> get();

        auto ref2 = object->GetComponent<InputComponent>();
        if(!ref2) return;
        auto& s = ref2 -> get();

        auto [axis, angle] = transform.rotation.ToAxisAngle();
        model.Draw(transform.position, axis, angle);

        if(s.selected){
            auto [axis, angle] = transform.rotation.ToAxisAngle();
            model.Draw(transform.position, axis, angle);
            model.GetTransformedBoundingBox().Draw();
        }

    }
};

int main () {
    raylib:: Window window (800, 450, "CS381 - AS6");
    std::vector<Entity> entities;  
    raylib::BufferedInput inputs;

    Entity& plane1= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(20,20);
    plane1.AddComponent<InputComponent>();

    Entity& plane2= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(20,20);
    plane1.AddComponent<InputComponent>();

    Entity& plane3= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(20,20);
    plane1.AddComponent<InputComponent>();

    Entity& plane4= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(20,20);
    plane1.AddComponent<InputComponent>();
    
    Entity& plane5= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(20,20);
    plane1.AddComponent<InputComponent>();

    Entity& ship1 = entities.emplace_back();
    ship1.AddComponent<RenderingComponent>(raylib::Model ("meshes/SmitHouston_Tug.glb"));
    ship1.AddComponent<PhysicsComponent>(3, 3);
    ship1.AddComponent<InputComponent>();

    Entity& ship2 = entities.emplace_back();
    ship2.AddComponent<RenderingComponent>(raylib::Model ("meshes/OrientExplorer.glb"));
    ship2.AddComponent<PhysicsComponent>(6, 6);
    ship2.AddComponent<InputComponent>();

    Entity& ship3 = entities.emplace_back();
    ship3.AddComponent<RenderingComponent>(raylib::Model ("meshes/ddg51.glb"));
    ship3.AddComponent<PhysicsComponent>(1,1);
    ship3.AddComponent<InputComponent>();

    Entity& ship4 = entities.emplace_back();
    ship4.AddComponent<RenderingComponent>(raylib::Model ("meshes/Container_ShipLarge.glb"));
    ship4.AddComponent<PhysicsComponent>(8 ,8);
    ship4.AddComponent<InputComponent>();

    Entity& ship5 = entities.emplace_back();
    ship4.AddComponent<RenderingComponent>(raylib::Model ("meshes/CargoG_HOSBrigadoon.glb"));
    ship4.AddComponent<PhysicsComponent>(2 ,2);
    ship4.AddComponent<InputComponent>();
    
    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 300),
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
    
    int counter=0;

    while(!window.ShouldClose()){
        inputs.PollEvents();
        for(Entity& e: entities) e.tick(window.GetFrameTime());
        window.ClearBackground(raylib::Color::Black());

        if(IsKeyPressed(KEY_TAB)){
            counter++;
        }

        window.BeginDrawing();
        camera.BeginMode();

        skybox.Draw();
        ground.Draw({0,0,0});

        switch(counter % 10){
            case e1:
                auto temp = entities[e1].GetComponent<InputComponent>();
                auto& inputE = temp -> get();

                auto temp2 = entities[e1].GetComponent<PhysicsComponent>();
                auto& physicsE = temp2 -> get();

                inputE.selected = true;
                int targetSpeedE1= physicsE.targetSpeed;
                int targetHeadingE1 = physicsE.targetHeading;

                Forward += [&targetSpeedE1](){
                    targetSpeedE1 += 10;
                };

                Back += [&targetSpeedE1](){
                    targetSpeedE1 -= 10;
                };

                Right += [&targetHeadingE1](){
                    targetHeadingE1 -= 10;
                };

                Left += [&targetHeadingE1](){
                    targetHeadingE1 += 10;
                };

                physicsE.targetSpeed = targetSpeedE1;
                physicsE.targetHeading = targetHeadingE1;

                break;
            case e2:
                auto temp3 = entities[e2].GetComponent<InputComponent>();
                auto& inputE2 = temp3 -> get();

                auto temp4 = entities[e2].GetComponent<PhysicsComponent>();
                auto& physicsE2 = temp4 -> get();

                inputE2.selected = true;
                int targetSpeedE2= physicsE2.targetSpeed;
                int targetHeadingE2 = physicsE2.targetHeading;

                Forward += [&targetSpeedE2](){
                    targetSpeedE2 += 10;
                };

                Back += [&targetSpeedE2](){
                    targetSpeedE2 -= 10;
                };

                Right += [&targetHeadingE2](){
                    targetHeadingE2 -= 10;
                };

                Left += [&targetHeadingE2](){
                    targetHeadingE2 += 10;
                };

                physicsE2.targetSpeed = targetSpeedE2;
                physicsE2.targetHeading = targetHeadingE2;
                break;
            case e3:
                auto temp5 = entities[e3].GetComponent<InputComponent>();
                auto& inputE3 = temp5 -> get();

                auto temp6 = entities[e3].GetComponent<PhysicsComponent>();
                auto& physicsE3 = temp6 -> get();

                inputE3.selected = true;
                int targetSpeedE3= physicsE3.targetSpeed;
                int targetHeadingE3 = physicsE3.targetHeading;

                Forward += [&targetSpeedE3](){
                    targetSpeedE3 += 10;
                };

                Back += [&targetSpeedE3](){
                    targetSpeedE3 -= 10;
                };

                Right += [&targetHeadingE3](){
                    targetHeadingE3 -= 10;
                };

                Left += [&targetHeadingE3](){
                    targetHeadingE3 += 10;
                };

                physicsE3.targetSpeed = targetSpeedE3;
                physicsE3.targetHeading = targetHeadingE3;
                break;
            case e4:
                auto temp7 = entities[e4].GetComponent<InputComponent>();
                auto& inputE4 = temp7 -> get();

                auto temp8 = entities[e4].GetComponent<PhysicsComponent>();
                auto& physicsE4 = temp8 -> get();

                inputE4.selected = true;
                int targetSpeedE4= physicsE4.targetSpeed;
                int targetHeadingE4 = physicsE4.targetHeading;

                Forward += [&targetSpeedE4](){
                    targetSpeedE4 += 10;
                };

                Back += [&targetSpeedE4](){
                    targetSpeedE4 -= 10;
                };

                Right += [&targetHeadingE4](){
                    targetHeadingE4 -= 10;
                };

                Left += [&targetHeadingE4](){
                    targetHeadingE4 += 10;
                };

                physicsE4.targetSpeed = targetSpeedE4;
                physicsE4.targetHeading = targetHeadingE4;

                break;
            case e5:
                auto temp9 = entities[e5].GetComponent<InputComponent>();
                auto& inputE5 = temp9 -> get();

                auto temp10 = entities[e5].GetComponent<PhysicsComponent>();
                auto& physicsE5 = temp10 -> get();

                inputE5.selected = true;
                int targetSpeedE5= physicsE5.targetSpeed;
                int targetHeadingE5 = physicsE5.targetHeading;

                Forward += [&targetSpeedE5](){
                    targetSpeedE5 += 10;
                };

                Back += [&targetSpeedE5](){
                    targetSpeedE5 -= 10;
                };

                Right += [&targetHeadingE5](){
                    targetHeadingE5 -= 10;
                };

                Left += [&targetHeadingE5](){
                    targetHeadingE5 += 10;
                };

                physicsE5.targetSpeed = targetSpeedE5;
                physicsE5.targetHeading = targetHeadingE5;
                break;
            case e6:
                auto temp12 = entities[e6].GetComponent<InputComponent>();
                auto& inputE6 = temp12 -> get();

                auto temp13 = entities[e6].GetComponent<PhysicsComponent>();
                auto& physicsE6 = temp13 -> get();

                inputE6.selected = true;
                int targetSpeedE6= physicsE6.targetSpeed;
                int targetHeadingE6 = physicsE6.targetHeading;

                Forward += [&targetSpeedE6](){
                    targetSpeedE6 += 10;
                };

                Back += [&targetSpeedE6](){
                    targetSpeedE6 -= 10;
                };

                Right += [&targetHeadingE6](){
                    targetHeadingE6 -= 10;
                };

                Left += [&targetHeadingE6](){
                    targetHeadingE6 += 10;
                };

                physicsE6.targetSpeed = targetSpeedE6;
                physicsE6.targetHeading = targetHeadingE6;
            case e7:
                auto temp14 = entities[e7].GetComponent<InputComponent>();
                auto& inputE7 = temp14 -> get();

                auto temp15 = entities[e7].GetComponent<PhysicsComponent>();
                auto& physicsE7 = temp15 -> get();

                inputE7.selected = true;
                int targetSpeedE7= physicsE7.targetSpeed;
                int targetHeadingE7 = physicsE7.targetHeading;

                Forward += [&targetSpeedE7](){
                    targetSpeedE7 += 10;
                };

                Back += [&targetSpeedE7](){
                    targetSpeedE7 -= 10;
                };

                Right += [&targetHeadingE7](){
                    targetHeadingE7 -= 10;
                };

                Left += [&targetHeadingE7](){
                    targetHeadingE7 += 10;
                };

                physicsE7.targetSpeed = targetSpeedE7;
                physicsE7.targetHeading = targetHeadingE7;
                break;
            case e8:
                auto temp16 = entities[e8].GetComponent<InputComponent>();
                auto& inputE8 = temp16 -> get();

                auto temp17 = entities[e8].GetComponent<PhysicsComponent>();
                auto& physicsE8 = temp17 -> get();

                inputE8.selected = true;
                int targetSpeedE8= physicsE8.targetSpeed;
                int targetHeadingE8 = physicsE8.targetHeading;

                Forward += [&targetSpeedE8](){
                    targetSpeedE8 += 10;
                };

                Back += [&targetSpeedE8](){
                    targetSpeedE8 -= 10;
                };

                Right += [&targetHeadingE8](){
                    targetHeadingE8 -= 10;
                };

                Left += [&targetHeadingE8](){
                    targetHeadingE8 += 10;
                };

                physicsE8.targetSpeed = targetSpeedE8;
                physicsE8.targetHeading = targetHeadingE8;
                break;
            case e9:
                auto temp18 = entities[e9].GetComponent<InputComponent>();
                auto& inputE9 = temp -> get();

                auto temp19 = entities[e9].GetComponent<PhysicsComponent>();
                auto& physicsE9 = temp19 -> get();

                inputE9.selected = true;
                int targetSpeedE9= physicsE9.targetSpeed;
                int targetHeadingE9 = physicsE9.targetHeading;

                Forward += [&targetSpeedE9](){
                    targetSpeedE9 += 10;
                };

                Back += [&targetSpeedE9](){
                    targetSpeedE9 -= 10;
                };

                Right += [&targetHeadingE9](){
                    targetHeadingE9 -= 10;
                };

                Left += [&targetHeadingE9](){
                    targetHeadingE9 += 10;
                };

                physicsE9.targetSpeed = targetSpeedE9;
                physicsE9.targetHeading = targetHeadingE9;

                break;
        }

    }
    return 0;
}