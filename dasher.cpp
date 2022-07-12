#include "raylib.h"
#include <stdio.h>

struct AnimData{
    Rectangle rec; //texture sprite
    Vector2 pos; // texture position
    float running_time; //seconds with the same frame
    float update_time; //seconds until frame change
    int frame; //displayed frame
    int max_frames; //number of frames of the animation
};

bool is_on_air(AnimData data, int window_heigth){
    return data.pos.y + data.rec.height < window_heigth;
}

AnimData update_anim_data(AnimData data, float delta_time){
    data.running_time += delta_time;
    if (data.running_time >= data.update_time){
        data.frame = (data.frame + 1) % data.max_frames;
        data.running_time = 0.0;
        data.rec.x = data.frame*data.rec.width;
    }
    return data;
}

int main(){
    //window dimensions
    const int window_dimensions[2]{1000,700}; //width, height

    //rectangle dimensions, position and velocity
    /*const int rectangle_width = 50;
    const int rectangle_height = 80;
    int rectangle_pos_y = window_dimensions[1] - rectangle_height;*/
    int scarfy_velocity = 0;
    const int nebula_velocity = -250; //pixels/second
    const int background_vel = -200; //pixels/second

    //gravity value
    const int gravity = 1000; //(pixels/second)/second
    const int jump_velocity = -600; //pixels/second
    const int target_fps = 60;

    InitWindow(window_dimensions[0],window_dimensions[1],"Dapper Dasher");
    SetTargetFPS(target_fps);

    //animations textures and data
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData{
        {0,0,static_cast<int>(scarfy.height),scarfy.width/6},
        {(float)window_dimensions[0]/2 - scarfy.width/12,window_dimensions[1]-scarfy.height},
        0.0,
        1.0/12.0,
        0,
        6};
    Texture2D nebula = LoadTexture("textures/nebula.png");
    
    //init the nebulae array
    AnimData nebulae[10];
    for(int i = 0; i < sizeof(nebulae)/sizeof(AnimData); i++){
        nebulae[i].rec.x = 0;
        nebulae[i].rec.y = (i+1)*nebula.height/8;
        nebulae[i].rec.height = nebula.height/8;
        nebulae[i].rec.width = nebula.width/8;
        nebulae[i].pos.x = 400*i+window_dimensions[0];
        nebulae[i].pos.y = window_dimensions[1] - nebula.height/8;
        nebulae[i].frame = 0;
        nebulae[i].running_time = 0.0;
        nebulae[i].update_time = 1.0/24.0;
        nebulae[i].max_frames = 8;
    }

    float finishLine = nebulae[(sizeof(nebulae)/sizeof(AnimData))-1].pos.x;

    Texture2D background = LoadTexture("textures/far-buildings.png");
    const float background_scale = 7.0;
    int background_repetition = 2 + (window_dimensions[0] / (background.width * background_scale));
    AnimData backgrounds[background_repetition];
    for (int i = 0; i < background_repetition; i++){
        backgrounds[i] = {
            {0,0,static_cast<float>(background.height),static_cast<float>(background.width)},
            {i*(background.width*background_scale),0},
            0.0,
            0,
            0,
            0 
        };
    }
    Texture2D background2 = LoadTexture("textures/back-buildings.png");
    int background2_repetition = 2 + (window_dimensions[0] / background2.width );
    AnimData backgrounds2[background2_repetition];
    const float background2_scale = 3.5;
    for (int i = 0; i < background2_repetition; i++){
        backgrounds2[i] = {
            {0,0,static_cast<float>(background2.height),static_cast<float>(background2.width)},
            {i*(background2.width)*background2_scale,window_dimensions[1]-background2.height*background2_scale},
            0.0,
            0,
            0,
            0 
        };
    }

    Texture2D background3 = LoadTexture("textures/foreground.png");
    int background3_repetition = 2 + (window_dimensions[0] / (background3.width));
    const float background3_scale = 2;
    AnimData backgrounds3[background3_repetition];
    for (int i = 0; i < background3_repetition; i++){
        backgrounds3[i] = {
            {0,0,static_cast<float>(background3.height),static_cast<float>(background3.width)},
            {i*(background3.width)*background3_scale,window_dimensions[1]-background3.height*background3_scale},
            0.0,
            0,
            0,
            0 
        };
    }

    bool collision = false;

    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        float delta_time = GetFrameTime();
        float nebula_pad = 50;
        Rectangle scarfy_rectangle, nebula_rectangle;
        
        for (AnimData nebula: nebulae){
            scarfy_rectangle = {scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height};
            nebula_rectangle = {nebula.pos.x+nebula_pad, nebula.pos.y+nebula_pad, nebula.rec.width-2*nebula_pad, nebula.rec.height-2*nebula_pad};
            if(CheckCollisionRecs(nebula_rectangle,scarfy_rectangle)){
                collision = true;
            }
        }

        if (collision){
            DrawText("Game Over",scarfyData.pos.x,scarfyData.pos.y, 25, RED);
        }
        else{
            if (scarfyData.pos.x > finishLine){
                DrawText("Victory",scarfyData.pos.x,scarfyData.pos.y, 25, GREEN);  
            }else{
                for (int i = 0; i < background_repetition; i++){
                    DrawTextureEx(background,backgrounds[i].pos,0,background_scale,WHITE);
                }
                for (int i = 0; i < background2_repetition; i++){
                    DrawTextureEx(background2,backgrounds2[i].pos,0,background2_scale,WHITE);
                }
                for (int i = 0; i < background3_repetition; i++){
                    DrawTextureEx(background3,backgrounds3[i].pos,0,background3_scale,WHITE);
                }
                for(int i = 0; i < sizeof(nebulae)/sizeof(AnimData); i++){
                    nebulae[i] = update_anim_data(nebulae[i],delta_time);
                    DrawTextureRec(nebula,nebulae[i].rec,nebulae[i].pos,WHITE);
                }
                if (!is_on_air(scarfyData, window_dimensions[1])){
                    scarfyData = update_anim_data(scarfyData, delta_time);
                }
                DrawTextureRec(scarfy,scarfyData.rec,scarfyData.pos,WHITE);   

                //update velocity and position
                for(int i = 0; i < sizeof(nebulae)/sizeof(AnimData); i++){
                    nebulae[i].pos.x += nebula_velocity * delta_time;
                    /*if (nebulae[i].pos.x <= 0-nebulae[0].rec.width){
                        nebulae[i].pos.x = window_dimensions[0];
                    }*/
                }
                scarfyData.pos.y += scarfy_velocity * delta_time;
                if (!is_on_air(scarfyData,window_dimensions[1])){ //on the ground
                    scarfyData.pos.y = window_dimensions[1]-scarfyData.rec.height;
                    scarfy_velocity = 0;        
                }
                else{ //on the air
                    scarfy_velocity+=gravity * delta_time;  
                }

            for (int i = 0; i < background_repetition; i++){
                backgrounds[i].pos.x += background_vel * delta_time * 0.25;
                if (backgrounds[i].pos.x + (background.width * background_scale) < 0){
                    backgrounds[i].pos.x = window_dimensions[0];
                }
            }
            for (int i = 0; i < background2_repetition; i++){
                backgrounds2[i].pos.x += background_vel * delta_time * 0.5;
                if (backgrounds2[i].pos.x + (background2.width * background2_scale) < 0){
                    backgrounds2[i].pos.x = window_dimensions[0];
                }
            }
            for (int i = 0; i < background3_repetition; i++){
                backgrounds3[i].pos.x += background_vel * delta_time;
                if (backgrounds3[i].pos.x + (background3.width * background3_scale) < 0){
                    backgrounds3[i].pos.x = window_dimensions[0];
                }
            }

            finishLine += nebula_velocity * delta_time;

                if (IsKeyPressed(KEY_SPACE) && !is_on_air(scarfyData,window_dimensions[1])){
                    scarfy_velocity += jump_velocity;
                }
            }    
        }
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(background2);
    UnloadTexture(background3);
    CloseWindow();
}