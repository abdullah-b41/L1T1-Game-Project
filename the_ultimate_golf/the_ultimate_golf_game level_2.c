#include "raylib.h"
#include "raymath.h"
#include <math.h>

//full global
int width = 1920;
int height = 1080;
float u = 1;
#define stroke_limit 25
#define max_speed 520

//colours
Color sand_light = {245,232,200,255};
Color wood = {139,94,52,255};
Color wood_dark = {92,60,32,255};
Color wood_light = {186,138,88,255};
Color sun_yellow = {255,204,64,255};
Color coral_red = {235,87,70,255};
Color sea = {63,180,207,255};
Color sea_deep = {26,127,168,255};

//pictures
Texture2D sand_texture;
Texture2D water_texture;
Texture2D foam_texture;
Texture2D crab_texture;
Texture2D palm_texture;
Texture2D splash_texture;

//ball and pot
Vector2 ball;
Vector2 speed;
float radius_ball;
Vector2 pot;
float radius_pot;
Vector2 start_position;
Vector2 last_shot_position;
int stroke = 0;
int game_state = 0;
int aiming = 0;
float message_timer = 0;
float animation_time = 0;
float hud_height;

//ground: sand is safe, everything else is water
Rectangle sand[10];
Rectangle soft_sand[4];
Rectangle wet_sand[2];

//tidal sand bars, safe only at low tide (tide clock: low 3s, rising 1s, high 3s, falling 1s)
Rectangle tidal[2];
Vector2 tidal_push[2];
float tide_timer = 0;

//rip current in shallow water (the old conveyor belt)
Rectangle ford;
Vector2 ford_push;

//whirlpools (the old magnet)
Vector2 whirlpool[2];
float whirlpool_field[2];
float whirlpool_pull;

//crabs (the old crane beams)
Rectangle crab[4];
float crab_speed[4];
float crab_left_limit[4];
float crab_right_limit[4];
float crab_angry[4];

//palm trees and beach balls
Vector2 palm[4];
float trunk_radius;
Vector2 beach_ball[4];
float beach_ball_radius;
float beach_ball_hit[4];

//splash
Vector2 splash_position;
float splash_timer = 0;


//a rectangle in design units (a 1920 x 1080 screen), made to fit the real screen
Rectangle make_rect(float x, float y, float w, float h)
{
    Rectangle rec = {x*u,y*u,w*u,h*u};
    return rec;
}

//a point in design units
Vector2 make_point(float x, float y)
{
    Vector2 point = {x*u,y*u};
    return point;
}


void reset_level()
{
    //sizes
    hud_height = 70*u;
    radius_ball = 7*u;
    radius_pot = 11*u;

    //sand islands and strips, the path goes up and down from left to right
    sand[0] = make_rect(60,760,340,270);     //start island
    sand[1] = make_rect(140,330,170,440);    //strip going up
    sand[2] = make_rect(60,110,520,230);     //top left island
    sand[3] = make_rect(450,640,200,390);    //bottom middle island
    sand[4] = make_rect(640,920,330,110);    //its narrow neck
    sand[5] = make_rect(860,330,110,600);    //strip going up (rip current in the middle)
    sand[6] = make_rect(800,110,600,230);    //top middle island
    sand[7] = make_rect(1310,330,70,380);    //narrow bridge going down
    sand[8] = make_rect(1100,700,760,330);   //bottom right island
    sand[9] = make_rect(1480,110,380,280);   //last island with the pot

    soft_sand[0] = make_rect(140,470,170,90);
    soft_sand[1] = make_rect(1230,820,220,150);
    soft_sand[2] = make_rect(1590,230,130,110);
    soft_sand[3] = make_rect(860,360,110,140);    //just past the rip current, so the ball stops before the island
    wet_sand[0] = make_rect(360,118,210,95);
    wet_sand[1] = make_rect(1600,940,250,80);

    //tidal bars, the rising tide pushes the ball back towards land
    tidal[0] = make_rect(470,330,100,320);
    tidal_push[0] = make_point(0,-400);
    tidal[1] = make_rect(1700,380,110,330);
    tidal_push[1] = make_point(0,400);
    tide_timer = 0;

    //rip current across the strip, pushing to the right
    ford = make_rect(860,520,110,220);
    ford_push = make_point(260,0);

    //whirlpools
    whirlpool[0] = make_point(730,820);
    whirlpool_field[0] = 125*u;
    whirlpool[1] = make_point(1450,560);
    whirlpool_field[1] = 130*u;
    whirlpool_pull = 380*u;

    //crabs walking left and right
    crab[0] = make_rect(160,625,44,30);
    crab_left_limit[0] = 140*u;
    crab_right_limit[0] = 266*u;
    crab_speed[0] = 110*u;
    crab[1] = make_rect(1100,125,44,30);
    crab_left_limit[1] = 1010*u;
    crab_right_limit[1] = 1356*u;
    crab_speed[1] = 130*u;
    crab[2] = make_rect(900,290,44,30);
    crab_left_limit[2] = 880*u;
    crab_right_limit[2] = 1250*u;
    crab_speed[2] = -130*u;
    crab[3] = make_rect(1500,745,44,30);
    crab_left_limit[3] = 1450*u;
    crab_right_limit[3] = 1816*u;
    crab_speed[3] = 150*u;

    //palm trees (only the trunk is solid) and beach balls
    trunk_radius = 14*u;
    palm[0] = make_point(300,930);
    palm[1] = make_point(220,200);
    palm[2] = make_point(1180,225);
    palm[3] = make_point(1790,300);
    beach_ball_radius = 24*u;
    beach_ball[0] = make_point(330,800);
    beach_ball[1] = make_point(1000,225);
    beach_ball[2] = make_point(1700,160);
    beach_ball[3] = make_point(1545,330);
    for (int i=0; i<4; i++)
    {
        crab_angry[i] = 0;
        beach_ball_hit[i] = 0;
    }

    //ball and pot
    start_position = make_point(130,990);
    ball = start_position;
    last_shot_position = start_position;
    speed.x = 0;
    speed.y= 0;
    pot = make_point(1560,200);
    stroke = 0;
    game_state = 0;
    aiming = 0;
    message_timer = 0;
    splash_timer = 0;
}

//bounce off a straight rectangle, rec_speed is how fast the rectangle itself is moving
int bounce_off_rectangle(Rectangle rec, Vector2 rec_speed)
{
    if (CheckCollisionCircleRec(ball,radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = ball.x - rec.x;
            float right = rec.x + rec.width - ball.x;
            float top = ball.y - rec.y;
            float bottom = rec.y + rec.height - ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }
        normal = Vector2Normalize(normal);
        ball = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));

        //reflect only if the ball is going into it
        Vector2 relative_speed = Vector2Subtract(speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            speed = Vector2Add(relative_speed,rec_speed);

            //moving crab side: also knock the ball out of its row, or it gets hit again and again
            if (rec_speed.x!=0 && normal.y==0)
            {
                if (ball.y < rec.y+rec.height/2) speed.y = speed.y - fabsf(rec_speed.x)/2;
                else speed.y = speed.y + fabsf(rec_speed.x)/2;
            }
            speed = Vector2ClampValue(speed,0,max_speed*u);
        }
        return 1;
    }
    return 0;
}

//bounce off a round thing that doesn't move, bounce is 1 for a normal one and more than 1 for a bouncy one
int bounce_off_circle(Vector2 center, float radius, float bounce)
{
    Vector2 normal = Vector2Subtract(ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        ball = Vector2Add(center,Vector2Scale(normal,radius + radius_ball));
        if (Vector2DotProduct(speed,normal)<0)
        {
            speed = Vector2ClampValue(Vector2Scale(Vector2Reflect(speed,normal),bounce),0,max_speed*u);
            return 1;
        }
    }
    return 0;
}


//is this point on sand (or shallow water), and not on a tidal bar at high tide
int on_safe_ground(Vector2 point)
{
    for (int i=0; i<10; i++)
    {
        if (CheckCollisionPointRec(point,sand[i])) return 1;
    }
    if (CheckCollisionPointRec(point,ford)) return 1;
    if (tide_timer<3.5 || tide_timer>=7.5)
    {
        for (int i=0; i<2; i++)
        {
            if (CheckCollisionPointRec(point,tidal[i])) return 1;
        }
    }
    return 0;
}


void update_obstacles(float dt)
{
    //crabs walking (the old crane beams)
    for (int i=0; i<4; i++)
    {
        crab[i].x = crab[i].x + crab_speed[i]*dt;
        if (crab[i].x < crab_left_limit[i])
        {
            crab[i].x = crab_left_limit[i];
            crab_speed[i] = fabsf(crab_speed[i]);
        }
        if (crab[i].x > crab_right_limit[i])
        {
            crab[i].x = crab_right_limit[i];
            crab_speed[i] = -fabsf(crab_speed[i]);
        }
        if (crab_angry[i]>0) crab_angry[i] = crab_angry[i] - dt;
        if (beach_ball_hit[i]>0) beach_ball_hit[i] = beach_ball_hit[i] - dt;
    }

    //tide clock
    tide_timer = tide_timer + dt;
    if (tide_timer>=8) tide_timer = tide_timer - 8;

    if (splash_timer>0) splash_timer = splash_timer - dt;
}


//after falling in the water, the ball goes back to where it was shot from
void send_ball_back()
{
    ball = last_shot_position;

    //not right in a crab's path, or it gets knocked into the water again and again
    for (int i=0; i<4; i++)
    {
        if (ball.x>crab_left_limit[i]-radius_ball && ball.x<crab_right_limit[i]+crab[i].width+radius_ball && ball.y>crab[i].y-radius_ball-1*u && ball.y<crab[i].y+crab[i].height+radius_ball+1*u)
        {
            if (ball.y < crab[i].y+crab[i].height/2) ball.y = crab[i].y - radius_ball - 2*u;
            else ball.y = crab[i].y + crab[i].height + radius_ball + 2*u;
        }
    }

    //the old spot is under water now (tide), start again
    if (on_safe_ground(ball)==0) ball = start_position;

    speed.x = 0;
    speed.y = 0;
}


void update_ball(float dt)
{
    int pushed = 0;

    //rip current (the old conveyor belt)
    if (CheckCollisionPointRec(ball,ford))
    {
        speed = Vector2Add(speed,Vector2Scale(ford_push,dt));
        pushed = 1;
    }

    //rising tide gently pushes the ball back towards land
    if (tide_timer>=3 && tide_timer<4)
    {
        for (int i=0; i<2; i++)
        {
            int on_sand = 0;
            for (int j=0; j<10; j++)
            {
                if (CheckCollisionPointRec(ball,sand[j])) on_sand = 1;
            }
            if (on_sand==0 && CheckCollisionPointRec(ball,tidal[i]))
            {
                if (Vector2Length(speed)<150*u) speed = Vector2Add(speed,Vector2Scale(tidal_push[i],dt));
                pushed = 1;
            }
        }
    }

    //whirlpool pull (the old magnet)
    for (int i=0; i<2; i++)
    {
        Vector2 to_whirlpool = Vector2Subtract(whirlpool[i],ball);
        float whirlpool_distance = Vector2Length(to_whirlpool);
        if (whirlpool_distance<whirlpool_field[i] && whirlpool_distance>1*u)
        {
            speed = Vector2Add(speed,Vector2Scale(Vector2Normalize(to_whirlpool),whirlpool_pull*dt));
            pushed = 1;
        }
    }

    //moving
    ball = Vector2Add(ball,Vector2Scale(speed,dt));

    //proportional deceleration (friction depends on the ground)
    float friction = 110*u;
    for (int i=0; i<4; i++)
    {
        if (CheckCollisionPointRec(ball,soft_sand[i])) friction = 400*u;
    }
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(ball,wet_sand[i])) friction = 20*u;
    }
    if (CheckCollisionPointRec(ball,ford)) friction = 250*u;
    float ball_speed = Vector2Length(speed);
    if (ball_speed>0)
    {
        float new_speed = ball_speed - friction*dt;
        if (new_speed<0) new_speed = 0;
        speed = Vector2Scale(speed,new_speed/ball_speed);
    }
    if (pushed==0 && Vector2Length(speed)<6*u)
    {
        speed.x = 0;
        speed.y= 0;
    }

    //water (checked before the collisions, so if something is on the old spot it pushes the ball out)
    if (on_safe_ground(ball)==0)
    {
        splash_position = ball;
        splash_timer = 0.5;
        send_ball_back();
        message_timer = 1;
    }

    //palm trunks and beach balls
    for (int i=0; i<4; i++)
    {
        bounce_off_circle(palm[i],trunk_radius,1);
        if (bounce_off_circle(beach_ball[i],beach_ball_radius,1.3)) beach_ball_hit[i] = 0.15;
    }

    //crabs
    for (int i=0; i<4; i++)
    {
        Vector2 crab_velocity = {crab_speed[i],0};
        if (bounce_off_rectangle(crab[i],crab_velocity)) crab_angry[i] = 0.4;
    }

    //score
    if ((ball.x>pot.x-3*radius_pot/4) && (ball.x<pot.x+3*radius_pot/4) && (ball.y>pot.y-3*radius_pot/4) && (ball.y<pot.y+3*radius_pot/4))
    {
        ball = pot;
        speed.x = 0;
        speed.y= 0;
        game_state = 1;
    }
}


//sand picture that repeats, lined up with the screen so pieces join without seams (the picture is 2x size)
void draw_sand(Rectangle rec, Color tint)
{
    Rectangle source = {rec.x/u*2,rec.y/u*2,rec.width/u*2,rec.height/u*2};
    Vector2 no_origin = {0,0};
    DrawTexturePro(sand_texture,source,rec,no_origin,0,tint);
}


//foam along the 4 edges of a piece of ground, the bumpy side faces the water
void draw_foam(Rectangle rec, Color tint)
{
    int frame = (int)(animation_time*8)%4;
    float long_side = rec.width + 40*u;
    Rectangle source = {rec.x/u*2,frame*128+1,long_side/u*2,126};
    Vector2 origin = {long_side/2,32*u};

    //top and bottom edges (top one turned around)
    Rectangle top = {rec.x+rec.width/2,rec.y-18*u,long_side,64*u};
    DrawTexturePro(foam_texture,source,top,origin,180,tint);
    Rectangle bottom = {rec.x+rec.width/2,rec.y+rec.height+18*u,long_side,64*u};
    DrawTexturePro(foam_texture,source,bottom,origin,0,tint);

    //left and right edges
    long_side = rec.height + 40*u;
    source.x = rec.y/u*2;
    source.width = long_side/u*2;
    origin.x = long_side/2;
    Rectangle left = {rec.x-18*u,rec.y+rec.height/2,long_side,64*u};
    DrawTexturePro(foam_texture,source,left,origin,90,tint);
    Rectangle right = {rec.x+rec.width+18*u,rec.y+rec.height/2,long_side,64*u};
    DrawTexturePro(foam_texture,source,right,origin,-90,tint);
}


void draw_ground()
{
    float t = animation_time;
    Vector2 no_origin = {0,0};

    //water tiles, 2 pictures swapping for the shimmer
    int water_frame = (int)(t*2)%2;
    Rectangle water_source = {water_frame*512,0,512,512};
    int columns = width/(256*u) + 1;
    int rows = height/(256*u) + 1;
    for (int i=0; i<columns; i++)
    {
        for (int j=0; j<rows; j++)
        {
            Rectangle tile = {i*256*u,j*256*u,256*u+1,256*u+1};
            DrawTexturePro(water_texture,water_source,tile,no_origin,0,WHITE);
        }
    }

    //whirlpools
    for (int i=0; i<2; i++)
    {
        Vector2 w = whirlpool[i];
        DrawCircleGradient(w,whirlpool_field[i],Fade(sea_deep,0.7),Fade(sea,0));
        for (int j=0; j<4; j++)
        {
            float ring = 18*u + j*24*u;
            float angle = t*(320 - j*60) + j*70;
            DrawRing(w,ring-2*u,ring+2*u,angle,angle+230,24,Fade(WHITE,0.55-j*0.1));
        }
        DrawCircle(w.x,w.y,10*u,sea_deep);
    }

    //how high the tide is, 0 low and 1 high
    float tide_level = 0;
    if (tide_timer>=3 && tide_timer<4) tide_level = tide_timer - 3;
    if (tide_timer>=4 && tide_timer<7) tide_level = 1;
    if (tide_timer>=7) tide_level = 8 - tide_timer;

    //foam first, then the ground on top, so the foam only shows on the water side
    for (int i=0; i<10; i++) draw_foam(sand[i],Fade(WHITE,0.85));
    for (int i=0; i<2; i++) draw_foam(tidal[i],Fade(WHITE,0.85*(1-tide_level)));

    //tidal bars, the water comes over them
    Color wet = {222,196,150,255};
    for (int i=0; i<2; i++)
    {
        draw_sand(tidal[i],wet);
        DrawRectangleRec(tidal[i],Fade(sea,tide_level*0.9));
    }

    for (int i=0; i<10; i++) draw_sand(sand[i],WHITE);

    //soft sand, darker with wind ripples
    Color soft = {226,196,140,255};
    for (int i=0; i<4; i++)
    {
        Rectangle s = soft_sand[i];
        draw_sand(s,soft);
        int ripple_rows = s.height/(14*u);
        int pieces = s.width/(24*u);
        for (int j=1; j<ripple_rows; j++)
        {
            float shift = 0;
            if (j%2==1) shift = 12*u;
            for (int k=0; k<pieces; k++)
            {
                Vector2 a = {s.x + k*24*u + shift, s.y + j*14*u};
                Vector2 b = {a.x + 14*u, a.y - 3*u};
                if (b.x < s.x+s.width) DrawLineEx(a,b,2*u,Fade(wood,0.35));
            }
        }
        DrawRectangleLinesEx(s,2*u,Fade(wood,0.25));
    }

    //wet sand, dark and shiny
    for (int i=0; i<2; i++)
    {
        Rectangle w = wet_sand[i];
        Color wet_dark = {196,160,112,255};
        draw_sand(w,wet_dark);
        for (int k=0; k<14; k++)
        {
            float shine_x = w.x + fmod(k*53*u,w.width);
            float shine_y = w.y + fmod(k*31*u + 7*u,w.height);
            DrawCircle(shine_x,shine_y,2.5*u,Fade(WHITE,0.25+0.25*sin(t*3+k)));
        }
    }

    //rip current, shallow water with foam lines moving the way it pushes
    Color shallow = {170,215,210,255};
    draw_sand(ford,shallow);
    DrawRectangleRec(ford,Fade(sea,0.45));
    int current_lines = ford.height/(22*u);
    for (int j=0; j<current_lines; j++)
    {
        Vector2 a = {ford.x + fmod(t*120*u + j*37*u,ford.width), ford.y + 11*u + j*22*u};
        Vector2 b = {a.x + 18*u, a.y};
        if (b.x > ford.x+ford.width) b.x = ford.x + ford.width;
        DrawLineEx(a,b,2*u,Fade(WHITE,0.6));
    }
}


void draw_obstacles()
{
    float t = animation_time;

    //start towel
    Rectangle towel = {start_position.x-45*u,start_position.y-28*u,90*u,56*u};
    DrawRectangle(towel.x+4*u,towel.y+5*u,towel.width,towel.height,Fade(BLACK,0.2));
    for (int j=0; j<6; j++)
    {
        Color stripe = coral_red;
        if (j%2==1) stripe = RAYWHITE;
        DrawRectangle(towel.x+j*15*u,towel.y,15*u,towel.height,stripe);
    }
    DrawText("START",start_position.x-MeasureText("START",16*u)/2,towel.y+4*u,16*u,wood_dark);

    //splash, 5 pictures in half a second
    if (splash_timer>0)
    {
        int frame = (0.5-splash_timer)/0.1;
        if (frame>4) frame = 4;
        Rectangle source = {frame*192,0,192,192};
        Rectangle dest = {splash_position.x,splash_position.y,96*u,96*u};
        Vector2 origin = {48*u,48*u};
        DrawTexturePro(splash_texture,source,dest,origin,0,WHITE);
    }

    //crabs, walking pictures 1-4, picture 5 when angry
    for (int i=0; i<4; i++)
    {
        Vector2 middle = {crab[i].x+crab[i].width/2,crab[i].y+crab[i].height/2};
        DrawCircle(middle.x+3*u,middle.y+6*u,18*u,Fade(BLACK,0.18));
        int frame = (int)(t*8)%4;
        if (crab_angry[i]>0) frame = 4;
        Rectangle source = {frame*128,0,128,128};
        Rectangle dest = {middle.x,middle.y-2*u,64*u,64*u};
        Vector2 origin = {32*u,32*u};
        DrawTexturePro(crab_texture,source,dest,origin,0,WHITE);
    }

    //beach balls, they get bigger for a moment when hit
    for (int i=0; i<4; i++)
    {
        Vector2 b = beach_ball[i];
        float r = beach_ball_radius;
        if (beach_ball_hit[i]>0) r = r*(1 + beach_ball_hit[i]);
        DrawCircle(b.x+3*u,b.y+5*u,r,Fade(BLACK,0.22));
        Color slice[6] = {coral_red,RAYWHITE,sea_deep,sun_yellow,RAYWHITE,LIME};
        for (int j=0; j<6; j++)
        {
            DrawCircleSector(b,r,t*40+j*60,t*40+j*60+60,8,slice[j]);
        }
        DrawCircle(b.x,b.y,5*u,RAYWHITE);
        DrawCircle(b.x-8*u,b.y-9*u,5*u,Fade(WHITE,0.45));
        DrawCircleLines(b.x,b.y,r,Fade(BLACK,0.35));
    }

    //palm tree shadows (the leaves are drawn after the ball)
    int palm_frame = (int)(t/0.8)%2;
    for (int i=0; i<4; i++)
    {
        Rectangle source = {palm_frame*384,0,384,384};
        Rectangle dest = {palm[i].x+14*u,palm[i].y+18*u,192*u,192*u};
        Vector2 origin = {96*u,96*u};
        DrawTexturePro(palm_texture,source,dest,origin,0,Fade(BLACK,0.22));
    }
}

void draw_ball_and_pot()
{
    //pot with striped ring
    for (int i=0; i<8; i++)
    {
        Color colour = WHITE;
        if (i%2==1) colour = coral_red;
        DrawRing(pot,radius_pot+6*u,radius_pot+12*u,i*45,i*45+45,6,colour);
    }
    DrawCircle(pot.x,pot.y,radius_pot+6*u,sand_light);
    DrawRing(pot,radius_pot+1*u,radius_pot+5*u,0,360,24,wood);
    DrawCircle(pot.x,pot.y,radius_pot,BLACK);

    //flag
    Vector2 pole_top = {pot.x,pot.y-60*u};
    DrawLineEx(pot,pole_top,3*u,sand_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(animation_time*6 - i*0.8)*2.5*u;
        DrawRectangle(pot.x+1*u+i*6*u,pole_top.y+wave,6*u,18*u,coral_red);
    }

    //ball
    DrawCircle(ball.x+3*u,ball.y+4*u,radius_ball,Fade(BLACK,0.45));
    DrawCircle(ball.x,ball.y,radius_ball,GetColor(0xEDEDEDFF));
    DrawCircleLines(ball.x,ball.y,radius_ball,GRAY);
    DrawCircle(ball.x-2*u,ball.y-2*u,2*u,WHITE);
}


//palm leaves go over the ball, so it hides under them (the aim line still shows)
void draw_palm_leaves()
{
    int palm_frame = (int)(animation_time/0.8)%2;
    for (int i=0; i<4; i++)
    {
        Rectangle source = {palm_frame*384,0,384,384};
        Rectangle dest = {palm[i].x,palm[i].y,192*u,192*u};
        Vector2 origin = {96*u,96*u};
        DrawTexturePro(palm_texture,source,dest,origin,0,WHITE);
    }

    //aim line
    if (aiming==1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        DrawLineEx(ball,mouse,4*u,Fade(WHITE,0.75));
        DrawCircle(mouse.x,mouse.y,5*u,Fade(WHITE,0.75));
    }
}


void draw_hud()
{
    //dark edges
    DrawRectangleGradientH(0,hud_height,90*u,height-hud_height,Fade(BLACK,0.25),BLANK);
    DrawRectangleGradientH(width-90*u,hud_height,90*u,height-hud_height,BLANK,Fade(BLACK,0.25));
    DrawRectangleGradientV(0,height-70*u,width,70*u,BLANK,Fade(BLACK,0.25));

    //wooden bar
    DrawRectangleGradientV(0,0,width,hud_height,wood_light,wood);
    DrawRectangle(0,hud_height-4*u,width,4*u,wood_dark);
    int rivets = width/(40*u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*u+i*40*u,8*u,3*u,wood_dark);
        DrawCircle(20*u+i*40*u,hud_height-12*u,3*u,wood_dark);
    }

    //text
    DrawText("LEVEL 2 - SHORELINE",32*u,20*u,36*u,BLACK);
    DrawText("LEVEL 2 - SHORELINE",30*u,18*u,36*u,sun_yellow);
    DrawText(TextFormat("STROKES %d / %d",stroke,stroke_limit),width/2-298*u,21*u,32*u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",stroke,stroke_limit),width/2-300*u,19*u,32*u,WHITE);
    for (int i=0; i<stroke_limit; i++)
    {
        Color pip = wood_dark;
        if (i<stroke) pip = coral_red;
        DrawCircle(width/2+10*u+i*22*u,35*u,7*u,pip);
        DrawCircleLines(width/2+10*u+i*22*u,35*u,7*u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC quit",26*u);
    DrawText("R restart   ESC quit",width-help_width-28*u,24*u,26*u,BLACK);
    DrawText("R restart   ESC quit",width-help_width-30*u,22*u,26*u,WHITE);

    //splash message
    if (message_timer>0)
    {
        DrawText("SPLASH!",width/2-MeasureText("SPLASH!",90*u)/2+4*u,height/2-41*u,90*u,Fade(sea_deep,message_timer));
        DrawText("SPLASH!",width/2-MeasureText("SPLASH!",90*u)/2,height/2-45*u,90*u,Fade(WHITE,message_timer));
    }

    //level clear or failed
    if (game_state!=0)
    {
        DrawRectangle(0,0,width,height,Fade(BLACK,0.6));
        Rectangle panel = {width/2-340*u,height/2-170*u,680*u,340*u};
        DrawRectangleRec(panel,wood_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*u};
        DrawRectangleRec(panel_band,sun_yellow);
        DrawRectangleLinesEx(panel,6*u,sand_light);
        if (game_state==1)
        {
            DrawText("LEVEL CLEAR!",width/2-MeasureText("LEVEL CLEAR!",80*u)/2,panel.y+60*u,80*u,sun_yellow);
        }
        else
        {
            DrawText("FAILED",width/2-MeasureText("FAILED",80*u)/2,panel.y+60*u,80*u,coral_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",stroke,stroke_limit),40*u);
        DrawText(TextFormat("Strokes: %d / %d",stroke,stroke_limit),width/2-strokes_width/2,panel.y+170*u,40*u,WHITE);
        DrawText("Press R to play again",width/2-MeasureText("Press R to play again",30*u)/2,panel.y+250*u,30*u,sand_light);
    }
}


int main()
{
    InitWindow(1280,720,"the ultimate golf - level 2");
    int monitor = GetCurrentMonitor();
    width = GetMonitorWidth(monitor);
    height = GetMonitorHeight(monitor);
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

    //pictures (made at 2x size, so they stay sharp)
    sand_texture = LoadTexture("assets/beach/beach_sand_tile.png");
    water_texture = LoadTexture("assets/beach/beach_water_tile.png");
    foam_texture = LoadTexture("assets/beach/beach_foam_strip.png");
    crab_texture = LoadTexture("assets/beach/beach_crab.png");
    palm_texture = LoadTexture("assets/beach/beach_palm.png");
    splash_texture = LoadTexture("assets/beach/beach_splash.png");
    SetTextureWrap(sand_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(foam_texture,TEXTURE_WRAP_REPEAT);
    SetTextureFilter(sand_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(water_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(foam_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(crab_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(palm_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(splash_texture,TEXTURE_FILTER_BILINEAR);

    //every size is N*u, so it looks the same on any screen
    u = height/1080.0;
    if (width/1920.0 < u) u = width/1920.0;
    reset_level();

    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (dt>1.0/30) dt = 1.0/30;
        animation_time = animation_time + dt;
        if (message_timer>0) message_timer = message_timer - dt;

        //restart
        if (IsKeyPressed(KEY_R)) reset_level();

        //shooting (the click has to start while the ball is still)
        int ball_stopped = 0;
        if (speed.x==0 && speed.y==0) ball_stopped = 1;
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && game_state==0) aiming = 1;
        if (game_state!=0) aiming = 0;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && aiming==1)
        {
            aiming = 0;
            Vector2 mouse = {GetMouseX(),GetMouseY()};
            Vector2 drag = Vector2Subtract(ball,mouse);
            if (ball_stopped==1 && stroke<stroke_limit && Vector2Length(drag)>=2*radius_ball)
            {
                last_shot_position = ball;
                speed = Vector2ClampValue(Vector2Scale(drag,2.5),0,max_speed*u);
                stroke++;
            }
        }

        //moving everything in 4 small steps, so nothing jumps through anything
        for (int i=0; i<4; i++)
        {
            update_obstacles(dt/4);
            if (game_state==0) update_ball(dt/4);
        }

        //out of strokes
        if (game_state==0 && stroke>=stroke_limit && speed.x==0 && speed.y==0) game_state = 2;


        BeginDrawing();

        draw_ground();
        draw_obstacles();
        draw_ball_and_pot();
        draw_palm_leaves();
        draw_hud();

        EndDrawing();
    }
    UnloadTexture(sand_texture);
    UnloadTexture(water_texture);
    UnloadTexture(foam_texture);
    UnloadTexture(crab_texture);
    UnloadTexture(palm_texture);
    UnloadTexture(splash_texture);
    CloseWindow();


    return 0;
}
