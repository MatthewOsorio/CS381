#include "raylib-cpp.hpp"

int main (){
    int windowWidth= 700, windowHeight= 550, rotation=0;
    float spacing= 2.0, fontSize= 50.0;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(240);

    raylib::Window window (windowWidth, windowHeight, "CS381 - Assingment 0");
    raylib::Text text;

    //creating Vector2 objects for the positon of text and the origin for the rotaton
    Vector2 position;
    Vector2 origin;

    while(!window.ShouldClose()){
        window.BeginDrawing();
        window.ClearBackground(RAYWHITE);

        //setting position for text
        position.x= (float)(window.GetWidth()/2);
        position.y= (float)(window.GetHeight()/2);

        //setting the origin in which the text rotatest around
        origin.x= ((MeasureTextEx(GetFontDefault(), "Leslie", fontSize, spacing).x)/2);
        origin.y= ((MeasureTextEx(GetFontDefault(), "Leslie", fontSize, spacing).y)/2);

        //creating lines to visualize the center
        DrawLine(window.GetWidth()/2, 0, window.GetWidth()/2, window.GetHeight(), BLACK);
        DrawLine(0, window.GetHeight()/2, window.GetWidth(), window.GetHeight()/2, BLACK);

        //create text
        text.Draw(GetFontDefault(), "Leslie", position, origin, (float)rotation, (float)fontSize, spacing, PINK);
        //increment rotaton variable so the text can rotate
        rotation++;

        window.EndDrawing() ;
    }

    return 0;
}