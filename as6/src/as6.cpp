// Matthew Osorio
// AS6
// April 12, 2024

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

enum entities{e0, e1, e2, e3, e4, e5, e6, e7, e8, e9};

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

    void updatePosition (raylib::Vector3 newPosition) {
        position = newPosition;
    }
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
    raylib::Degree heading, targetHeading;

    PhysicsComponent(Entity& e, int ac, int tc, int ms): Component(e), acceleration(std::move(ac)), turningacceleration(std::move(tc)), maxSpeed(std::move(ms)) {}

    void setup () override{
        speed= 0;
        targetSpeed= 0;
        heading= 0;
        targetHeading= 0;
        std::cout << "test" << std::endl;
    }

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return ;
        auto& transform = ref -> get(); 

        raylib::Vector3 velocity = {speed* cos(heading.RadianValue()), 0 , -speed *sin(heading.RadianValue())};
        transform.updatePosition((transform.position + velocity * dt));

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

    void updateSpeed(int newSpeed){
        targetSpeed = newSpeed;
    }
};

struct InputComponent: public Component {
    bool selected;
    raylib::BufferedInput inputs;

    InputComponent(Entity&e): Component(e) { }

    void setup() override{

        inputs["forward"] = raylib::Action::key(KEY_W).AddPressedCallback([]{
            Forward();
        }).move();
    }
};

struct RenderingComponent: public Component {
    raylib::Model model;
    raylib::Vector3 scale {1.0f, 1.0f, 1.0f};
    bool selected;

    RenderingComponent(Entity& e, raylib::Model&& m): Component(e), model(std::move(m)) {}

    void tick(float dt) override {
        auto ref = object->GetComponent<TransformComponent>();
        if(!ref) return;
        auto& transform = ref -> get();

        auto[axis, angle] = transform.rotation.ToAxisAngle();
      
        model.Draw(transform.position, axis, angle, scale);
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

    cs381::SkyBox skybox("textures/skybox.png");
    auto mesh = raylib::Mesh::Plane(10'000, 10'000, 50, 50,25);
    raylib::Model ground = ((raylib::Mesh*)&mesh) -> LoadModelFrom();
    raylib::Texture water("textures/water.jpeg");
    water.SetFilter(TEXTURE_FILTER_BILINEAR);
    water.SetWrap(TEXTURE_WRAP_REPEAT);
    ground.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water;

    std::vector<Entity> entities;  

    Entity& ship0= entities.emplace_back();
    ship0.AddComponent<RenderingComponent>(raylib::Model ("meshes/SmitHouston_Tug.glb"));
    ship0.Transform().updatePosition(raylib::Vector3 {150, 0, 0});
    ship0.Transform().rotation.Slerp(raylib::Quaternion {0.59, 0, 0.2, 1.0}, 1.2);
    ship0.AddComponent<PhysicsComponent>(12, 12, 30);
    ship0.AddComponent<InputComponent>();

    Entity& ship1= entities.emplace_back();
    ship1.AddComponent<RenderingComponent>(raylib::Model ("meshes/Container_ShipLarge.glb"));
    ship1.Transform().updatePosition(raylib::Vector3 {-150, 10, 0});
    ship1.GetComponent<RenderingComponent>()->get().scale = raylib::Vector3 {0.01, 0.01, 0.01};
    ship1.AddComponent<PhysicsComponent>(15 , 15, 45);
    ship1.AddComponent<InputComponent>();

    /*
    Entity& ship2= entities.emplace_back();
    ship2.AddComponent<RenderingComponent>(raylib::Model ("meshes/ddg51.glb"));
    ship2.Transform().updatePosition(raylib::Vector3 {75, 0, 0});
    ship2.AddComponent<PhysicsComponent>(10 , 10 , 20);
    ship2.AddComponent<InputComponent>();

    Entity& ship3= entities.emplace_back();
    ship3.AddComponent<RenderingComponent>(raylib::Model ("meshes/u-_boat.glb"));
    ship3.Transform().updatePosition(raylib::Vector3 {-75, 50, 0});
    ship3.GetComponent<RenderingComponent>()->get().scale = raylib::Vector3 {-3.0, -3.0, -3.0};
    ship3.AddComponent<PhysicsComponent>(7, 7, 10);
    ship3.AddComponent<InputComponent>();

    Entity& ship4= entities.emplace_back();
    ship4.AddComponent<RenderingComponent>(raylib::Model ("meshes/boat2.glb"));
    ship4.AddComponent<PhysicsComponent>(4 ,4 , 8);
    ship4.AddComponent<InputComponent>();

    Entity& plane0= entities.emplace_back();
    plane0.Transform().updatePosition(raylib::Vector3 {0 ,50, 0});
    plane0.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane0.AddComponent<PhysicsComponent>(10, 10, 20);
    plane0.AddComponent<InputComponent>();

    Entity& plane1= entities.emplace_back();
    plane1.Transform().updatePosition(raylib::Vector3 {150 ,50, 0});
    plane1.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane1.AddComponent<PhysicsComponent>(10, 10, 20);
    plane1.AddComponent<InputComponent>();

    Entity& plane2= entities.emplace_back();
    plane2.Transform().updatePosition(raylib::Vector3 {75 ,50, 0});
    plane2.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane2.AddComponent<PhysicsComponent>(10, 10, 20);
    plane2.AddComponent<InputComponent>();

    Entity& plane3= entities.emplace_back();
    plane3.Transform().updatePosition(raylib::Vector3 {-75 ,50, 0});
    plane3.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane3.AddComponent<PhysicsComponent>(10, 10, 20);
    plane3.AddComponent<InputComponent>();

    Entity& plane4= entities.emplace_back();
    plane4.Transform().updatePosition(raylib::Vector3 {-150 ,50, 0});
    plane4.AddComponent<RenderingComponent>(raylib::Model ("meshes/PolyPlane.glb"));
    plane4.AddComponent<PhysicsComponent>(10, 10, 20);
    plane4.AddComponent<InputComponent>();
    
    */
    raylib::Model plane("meshes/PolyPlane.glb");
    Entity& plane0= entities.emplace_back();
    plane0.AddComponent<RenderingComponent>(&plane);
    plane0.AddComponent<PhysicsComponent>(10, 10, 20);
    plane0.AddComponent<InputComponent>();


    for(Entity& e: entities) e.setup();
    /*
    Forward += [&entities](){
        for(Entity& e :entities){
            e.GetComponent<PhysicsComponent>()->get().targetSpeed += 10;
        }
    };

    Forward += [](){
        std::cout<<"forward"<<std::endl;
    };
    */
    int counter=0;
    while(!window.ShouldClose()){
        //plane0.GetComponent<InputComponent>()->get().inputs.PollEvents();

        if(IsKeyPressed(KEY_TAB)){
            counter++;
        }

        window.BeginDrawing();
            window.ClearBackground(BLACK);
            camera.BeginMode();
                skybox.Draw();
                ground.Draw({0,0,0});

                for(Entity& e: entities) e.tick(window.GetFrameTime());

                        switch((counter % 10)){
            case e0:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e1:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e2:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e3:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e4:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e5:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e6:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e7:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e8:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = true;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = false;
                break;
            case e9:
                entities[e0].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e1].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e2].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e3].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e4].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e5].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e6].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e7].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e8].GetComponent<RenderingComponent>()->get().selected = false;
                entities[e9].GetComponent<RenderingComponent>()->get().selected = true;
                break;

            default:
                break;
        }

            camera.EndMode();
        window.EndDrawing();
    }
    return 0;
}