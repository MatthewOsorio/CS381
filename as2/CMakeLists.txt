cmake_minimum_required(VERSION 3.18)
project(as2 LANGUAGES CXX)

add_subdirectory(raylib-cpp)

file(GLOB sources "src/*.cpp" "src/*.c")
add_executable(as2 ${sources})
target_link_libraries(as2 PUBLIC raylib raylib_cpp)
set_property(TARGET as2 PROPERTY CXX_STANDARD 20)

configure_file(meshes/PolyPlane.glb meshes/PolyPlane.glb COPYONLY)
configure_file(meshes/SmitHouston_Tug.glb meshes/SmitHouston_Tug.glb COPYONLY)
configure_file(meshes/john_rambo.glb meshes/john_rambo.glb COPYONLY)
configure_file(meshes/among_us_character.glb meshes/among_us_character.glb COPYONLY)
configure_file(textures/grass.jpg textures/grass.jpg COPYONLY)
configure_file(textures/skybox.png textures/skybox.png COPYONLY)

include(includeable.cmake)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.vs generated/skybox.330.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/skybox.fs generated/skybox.330.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.vs generated/cubemap.330.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl330/cubemap.fs generated/cubemap.330.fs)

make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/skybox.vs generated/skybox.100.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/skybox.fs generated/skybox.100.fs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/cubemap.vs generated/cubemap.100.vs)
make_includeable(raylib-cpp/raylib/examples/models/resources/shaders/glsl100/cubemap.fs generated/cubemap.100.fs)