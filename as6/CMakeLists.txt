cmake_minimum_required(VERSION 3.18)
project(as6 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as6 ${sources})
target_link_libraries(as6 PUBLIC raylib raygui raylib_cpp raylib::buffered)
set_property(TARGET as6 PROPERTY CXX_STANDARD 20)

configure_file(meshes/PolyPlane.glb meshes/PolyPlane.glb COPYONLY)
configure_file(meshes/SmitHouston_Tug.glb meshes/SmitHouston_Tug.glb COPYONLY)
configure_file(meshes/boat.glb meshes/boat.glb COPYONLY)
configure_file(meshes/ddg51.glb meshes/ddg51.glb COPYONLY)
configure_file(meshes/Container_ShipLarge.glb meshes/Container_ShipLarge.glb COPYONLY)
configure_file(meshes/boat2.glb meshes/boat2.glb COPYONLY)
configure_file(textures/water.jpeg textures/water.jpeg COPYONLY)
configure_file(textures/skybox.png textures/skybox.png COPYONLY)
configure_file(audio/windsound.mp3  audio/windsound.mp3 COPYONLY)


include(includeable.cmake)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.330.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.330.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.330.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.330.fs)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/skybox.vs generated/skybox.100.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/skybox.fs generated/skybox.100.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/cubemap.vs generated/cubemap.100.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/cubemap.fs generated/cubemap.100.fs)