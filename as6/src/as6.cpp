// Matthew Osorio
// AS6
// April 22, 2024

#include "raylib-cpp.hpp"
#include "skybox.hpp"
#include "delegate.hpp"
#include <iostream>
#include <memory>
#include <BufferedInput.hpp>
#include <vector>

cs381::Delegate<void()> Forward;
cs381::Delegate<void()> Back;
cs381::Delegate<void()> Right;
cs381::Delegate<void()> Left;
cs381::Delegate<void()> Stop;

enum entities{e10, e1, e2, e3, e4, e5, e6, e7, e8, e9};

template<typename T>
concept Transformer = requires(T t, raylib::Transform m){
    { t.operator()(m) } -> std::convertible_to<raylib::Transform>;
};

void DrawModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    model.transform = backupTransform;
}

void DrawBoundedModel(raylib::Model& model, Transformer auto transformer) {
    raylib::Transform backupTransform= model.transform;
    model.transform = transformer(backupTransform);
    model.Draw({});
    model.GetTransformedBoundingBox().Draw();
    model.transform = backupTransform;
}

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
    raylib::Degree heading =0;
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
        std::unique_ptr<Component> c = std::make_unique<T>(*this, std::forward<Ts>(args)...);
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
        return {};
    }

    void tick(float dt) {
        for(auto& c: components)
        c->tick(dt);
    }

    void setup() {
        for(auto& c: components)
        c->setup();
    }

    //Got from Josh's file
    TransformComponent& Transform() { return *GetComponent<TransformComponent>(); }
};

struct PhysicsComponent: public Component {
    int acceleration, turningacceleration, maxSpeed, speed, targetSpeed;
    raylib::Degree targetHeading;

    PhysicsComponent(Entity& e, int ac, int tc, int ms): Component(e), acceleration(std::move(ac)), turningacceleration(std::move(tc)), maxSpeed(std::move(ms)) {}

    void setup () override{
        speed= 0;
        targetSpeed= 0;

        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return ;
        auto& transform = ref -> get();

        targetHeading= transform.heading; 
    }
    
    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return ;
        auto& transform = ref -> get(); 

        raylib::Vector3 velocity = {speed* cos(transform.heading.RadianValue()), 0 , -speed *sin(transform.heading.RadianValue())};
        transform.position += velocity * dt;

        if(targetSpeed > speed){
            speed += acceleration * dt;
        }
        if(targetSpeed < speed){
            speed -= acceleration * dt;
        }
        if(targetHeading > transform.heading){
            transform.heading += turningacceleration * dt;
        }
        if(targetHeading < transform.heading){
            transform.heading -= turningacceleration * dt;
        }
    }
};

struct RenderingComponent: public Component {
    raylib::Model model;
    bool selected= false;

    RenderingComponent(Entity& e, raylib::Model&& m): Component(e), model(std::move(m)) {}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return;
        auto& transform = ref -> get();

        auto position= transform.position;
        auto heading= transform.heading;

        if(selected){
            DrawBoundedModel(model, [&position, &heading](raylib::Transform t) -> raylib::Transform{
            return t.Translate(position).RotateY(heading);
            });
        }
        else{
            DrawModel(model, [&position, &heading](raylib::Transform t) -> raylib::Transform{
            return t.Translate(position).RotateY(heading);
            });
        }
    }
};

struct InputComponent: public Component {
    raylib::BufferedInput* input;
    bool selected= false;

    InputComponent(Entity& e, raylib::BufferedInput* in): Component(e), input(std::move(in)) { }

    void setup() override{
            
        (*input)["forward"].AddPressedCallback([]{
            Forward();    
        });

        (*input)["back"].AddPressedCallback([]{
            Back();    
        });

        (*input)["right"].AddPressedCallback([]{
            Right();    
        });

        (*input)["left"].AddPressedCallback([]{
            Left();    
        });
        (*input)["stop"].AddPressedCallback([]{
            Stop();    
        });
    }
};

int main () {
    raylib:: Window window (1200, 900, "CS381 - AS6");

    raylib::Camera camera(
        raylib::Vector3(0, 120, -500),
        raylib::Vector3(0, 0, 300),
        raylib::Vector3::Up(),
        45.0f,
        CAMERA_PERSPECTIVE
    );

    raylib::AudioDevice audio;
    raylib::Music wind("audio/windsound.mp3");
    PlayMusicStream(wind);

    raylib::BufferedInput inputs;
    inputs["forward"] = raylib::Action::key(KEY_W).move();
    inputs["back"] = raylib::Action::key(KEY_S).move();
    inputs["right"] = raylib::Action::key(KEY_D).move();
    inputs["left"] = raylib::Action::key(KEY_A).move();
    inputs["stop"] = raylib::Action::key(KEY_SPACE).move();

    cs381::SkyBox skybox("textures/skybox.png");
    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50,25);
    raylib::Model ground = ((raylib::Mesh*)&mesh) -> LoadModelFrom();
    raylib::Texture water("textures/water.jpeg");
    water.SetFilter(TEXTURE_FILTER_BILINEAR);
    water.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

    std::vector<Entity> entities;  

    Entity& ship1= entities.emplace_back();
    ship1.AddComponent<RenderingComponent>(raylib::Model ("meshes/SmitHouston_Tug.glb"));
    ship1.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {200,0,0};
    ship1.GetComponent<TransformComponent>()->get().heading= raylib::Degree {90};
    ship1.AddComponent<PhysicsComponent>(20, 20, 30);
    ship1.AddComponent<InputComponent>(&inputs);

    Forward += [&ship1](){
        if(ship1.GetComponent<RenderingComponent>()->get().selected){
            ship1.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;
        }
    };

    Back += [&ship1](){
        if(ship1.GetComponent<RenderingComponent>()->get().selected){
            ship1.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&ship1](){
        if(ship1.GetComponent<RenderingComponent>()->get().selected){
            ship1.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&ship1](){
        if(ship1.GetComponent<RenderingComponent>()->get().selected){
            ship1.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&ship1](){
        if(ship1.GetComponent<RenderingComponent>()->get().selected){
            ship1.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& ship2= entities.emplace_back();
    ship2.AddComponent<RenderingComponent>(raylib::Model ("meshes/ddg51.glb"));
    ship2.GetComponent<RenderingComponent>()->get().model.transform= raylib::Transform(ship2.GetComponent<RenderingComponent>()->get().model.transform).RotateX(raylib::Degree (90));
    ship2.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {-200,20,0};
    ship2.GetComponent<TransformComponent>()->get().heading= raylib::Degree {90};
    ship2.AddComponent<PhysicsComponent>(7, 7, 20);
    ship2.AddComponent<InputComponent>(&inputs);

    Forward += [&ship2](){
        if(ship2.GetComponent<RenderingComponent>()->get().selected){
            ship2.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&ship2](){
        if(ship2.GetComponent<RenderingComponent>()->get().selected){
            ship2.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&ship2](){
        if(ship2.GetComponent<RenderingComponent>()->get().selected){
            ship2.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&ship2](){
        if(ship2.GetComponent<RenderingComponent>()->get().selected){
            ship2.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&ship2](){
        if(ship2.GetComponent<RenderingComponent>()->get().selected){
            ship2.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& ship3= entities.emplace_back();
    ship3.AddComponent<RenderingComponent>(raylib::Model ("meshes/Container_ShipLarge.glb"));
    ship3.GetComponent<RenderingComponent>()->get().model.transform= raylib::Transform(ship3.GetComponent<RenderingComponent>()->get().model.transform).RotateX(raylib::Degree (90)).Scale(0.005, 0.005, 0.005);
    ship3.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {-100, 20, 100};
    ship3.GetComponent<TransformComponent>()->get().heading= raylib::Degree {-90};
    ship3.AddComponent<PhysicsComponent>(5, 5, 15);
    ship3.AddComponent<InputComponent>(&inputs);

    Forward += [&ship3](){
        if(ship3.GetComponent<RenderingComponent>()->get().selected){
            ship3.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&ship3](){
        if(ship3.GetComponent<RenderingComponent>()->get().selected){
            ship3.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&ship3](){
        if(ship3.GetComponent<RenderingComponent>()->get().selected){
            ship3.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&ship3](){
        if(ship3.GetComponent<RenderingComponent>()->get().selected){
            ship3.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&ship3](){
        if(ship3.GetComponent<RenderingComponent>()->get().selected){
            ship3.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& ship4= entities.emplace_back();
    ship4.AddComponent<RenderingComponent>(raylib::Model ("meshes/u-_boat.glb"));
    ship4.GetComponent<RenderingComponent>()->get().model.transform= raylib::Transform(ship4.GetComponent<RenderingComponent>()->get().model.transform).Scale(3.0, 3.0, 3.0).RotateX(raylib::Degree (-90));
    ship4.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {25, 20, -100};
    ship4.GetComponent<TransformComponent>()->get().heading= raylib::Degree {90};
    ship4.AddComponent<PhysicsComponent>(30, 30, 35);
    ship4.AddComponent<InputComponent>(&inputs);

    Forward += [&ship4](){
        if(ship4.GetComponent<RenderingComponent>()->get().selected){
            ship4.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&ship4](){
        if(ship4.GetComponent<RenderingComponent>()->get().selected){
            ship4.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&ship4](){
        if(ship4.GetComponent<RenderingComponent>()->get().selected){
            ship4.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&ship4](){
        if(ship4.GetComponent<RenderingComponent>()->get().selected){
            ship4.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&ship4](){
        if(ship4.GetComponent<RenderingComponent>()->get().selected){
            ship4.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& ship5= entities.emplace_back();
    ship5.AddComponent<RenderingComponent>(raylib::Model ("meshes/boat.glb"));
    ship5.GetComponent<RenderingComponent>()->get().model.transform= raylib::Transform(ship5.GetComponent<RenderingComponent>()->get().model.transform).Scale(7.0, 7.0, 7.0);
    ship5.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {75, 20, 75};
    ship5.GetComponent<TransformComponent>()->get().heading= raylib::Degree {-90};
    ship5.AddComponent<PhysicsComponent>(12, 12, 25);
    ship5.AddComponent<InputComponent>(&inputs);

    Forward += [&ship5](){
        if(ship5.GetComponent<RenderingComponent>()->get().selected){
            ship5.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&ship5](){
        if(ship5.GetComponent<RenderingComponent>()->get().selected){
            ship5.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&ship5](){
        if(ship5.GetComponent<RenderingComponent>()->get().selected){
            ship5.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&ship5](){
        if(ship5.GetComponent<RenderingComponent>()->get().selected){
            ship5.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&ship5](){
        if(ship5.GetComponent<RenderingComponent>()->get().selected){
            ship5.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& plane1= entities.emplace_back();
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {200, 50 ,0};
    plane1.AddComponent<PhysicsComponent>(15, 15, 30);
    plane1.AddComponent<InputComponent>(&inputs);

    Forward += [&plane1](){
        if(plane1.GetComponent<RenderingComponent>()->get().selected){
            plane1.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&plane1](){
        if(plane1.GetComponent<RenderingComponent>()->get().selected){
            plane1.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&plane1](){
        if(plane1.GetComponent<RenderingComponent>()->get().selected){
            plane1.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&plane1](){
        if(plane1.GetComponent<RenderingComponent>()->get().selected){
            plane1.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&plane1](){
        if(plane1.GetComponent<RenderingComponent>()->get().selected){
            plane1.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& plane2= entities.emplace_back();
    plane2.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane2.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {-200, 50, 0};
    plane2.AddComponent<PhysicsComponent>(15, 15, 30);
    plane2.AddComponent<InputComponent>(&inputs);

    Forward += [&plane2](){
        if(plane2.GetComponent<RenderingComponent>()->get().selected){
            plane2.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&plane2](){
        if(plane2.GetComponent<RenderingComponent>()->get().selected){
            plane2.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&plane2](){
        if(plane2.GetComponent<RenderingComponent>()->get().selected){
            plane2.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&plane2](){
        if(plane2.GetComponent<RenderingComponent>()->get().selected){
            plane2.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&plane2](){
        if(plane2.GetComponent<RenderingComponent>()->get().selected){
            plane2.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& plane3= entities.emplace_back();
    plane3.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane3.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {-100, 50, 100};
    plane3.AddComponent<PhysicsComponent>(15, 15, 30);
    plane3.AddComponent<InputComponent>(&inputs);

    Forward += [&plane3](){
        if(plane3.GetComponent<RenderingComponent>()->get().selected){
            plane3.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };
    Back += [&plane3](){
        if(plane3.GetComponent<RenderingComponent>()->get().selected){
            plane3.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&plane3](){
        if(plane3.GetComponent<RenderingComponent>()->get().selected){
            plane3.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&plane3](){
        if(plane3.GetComponent<RenderingComponent>()->get().selected){
            plane3.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&plane3](){
        if(plane3.GetComponent<RenderingComponent>()->get().selected){
            plane3.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& plane4= entities.emplace_back();
    plane4.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane4.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {25, 50, -100};
    plane4.AddComponent<PhysicsComponent>(15, 15, 30);
    plane4.AddComponent<InputComponent>(&inputs);

    Forward += [&plane4](){
        if(plane4.GetComponent<RenderingComponent>()->get().selected){
            plane4.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&plane4](){
        if(plane4.GetComponent<RenderingComponent>()->get().selected){
            plane4.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&plane4](){
        if(plane4.GetComponent<RenderingComponent>()->get().selected){
            plane4.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&plane4](){
        if(plane4.GetComponent<RenderingComponent>()->get().selected){
            plane4.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&plane4](){
        if(plane4.GetComponent<RenderingComponent>()->get().selected){
            plane4.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    Entity& plane5= entities.emplace_back();
    plane5.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane5.GetComponent<TransformComponent>()->get().position= raylib::Vector3 {75, 50, 75};
    plane5.AddComponent<PhysicsComponent>(15, 15, 30);
    plane5.AddComponent<InputComponent>(&inputs);

    Forward += [&plane5](){
        if(plane5.GetComponent<RenderingComponent>()->get().selected){
            plane5.GetComponent<PhysicsComponent>()->get().targetSpeed += 5;       
        }
    };

    Back += [&plane5](){
        if(plane5.GetComponent<RenderingComponent>()->get().selected){
            plane5.GetComponent<PhysicsComponent>()->get().targetSpeed -= 5;
        }
    };

    Right += [&plane5](){
        if(plane5.GetComponent<RenderingComponent>()->get().selected){
            plane5.GetComponent<PhysicsComponent>()->get().targetHeading -= 5;
        }
    };

    Left += [&plane5](){
        if(plane5.GetComponent<RenderingComponent>()->get().selected){
            plane5.GetComponent<PhysicsComponent>()->get().targetHeading += 5;
        }
    };

    Stop += [&plane5](){
        if(plane5.GetComponent<RenderingComponent>()->get().selected){
            plane5.GetComponent<PhysicsComponent>()->get().targetSpeed = 0;
        }
    };

    for(Entity& e: entities) e.setup();
    
    int counter=0;

    while(!window.ShouldClose()){
        UpdateMusicStream(wind);

        if(IsKeyPressed(KEY_TAB)){
            counter++;
        }

        switch(counter % 10){
            case e10:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e1:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e2:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e3:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e4:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e5:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e6:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e7:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e8:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= true;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= false;
                break;
            case e9:
                entities[e10].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected= false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected= true;
                break;
        }

        inputs.PollEvents();
        
        window.BeginDrawing();
            window.ClearBackground(BLACK);
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                for(Entity& e: entities) e.tick(window.GetFrameTime());

            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}