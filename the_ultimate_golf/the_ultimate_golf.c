#include "raylib.h"
#include "raymath.h"
#include <math.h>

//==================== SHARED BY ALL LEVELS ====================
//difficulty settings, changed in the menu (chad is the default)
float obstacle_speed = 1;
float preview_seconds = 0.3;

//dotted line showing where a straight shot goes (no bounces), longer on easier difficulty
void draw_straight_preview(Vector2 ball, int aiming, float radius, float top_speed, float scale)
{
    if (preview_seconds<=0 || aiming==0 || IsMouseButtonDown(MOUSE_BUTTON_LEFT)==0) return;
    Vector2 mouse = {GetMouseX(),GetMouseY()};
    Vector2 drag = Vector2Subtract(ball,mouse);
    if (Vector2Length(drag)<2*radius) return;

    Vector2 shot = Vector2ClampValue(Vector2Scale(drag,3.125),0,top_speed);
    float shot_speed = Vector2Length(shot);
    Vector2 direction = Vector2Normalize(shot);
    float friction = 110*scale;

    //how far it rolls in the preview time, or until it stops
    float time = preview_seconds;
    if (shot_speed/friction<time) time = shot_speed/friction;
    float distance = shot_speed*time - 0.5*friction*time*time;

    for (int i=1; i<=12; i++)
    {
        Vector2 dot = Vector2Add(ball,Vector2Scale(direction,distance*i/12));
        DrawCircle(dot.x,dot.y,3*scale,Fade(WHITE,1-i/14.0));
    }
}

//==================== LEVEL 1 ====================

//full global
int l1_width = 1920;
int l1_height = 1080;
float l1_u = 1;
int l1_stroke_base = 18;
int l1_stroke_limit = 18;
#define l1_max_speed 650

//colours
Color l1_brick = {139,58,43,255};
Color l1_brick_dark = {110,44,32,255};
Color l1_mortar = {58,51,48,255};
Color l1_steel = {90,95,102,255};
Color l1_steel_light = {138,144,153,255};
Color l1_steel_dark = {46,49,54,255};
Color l1_floor_colour = {74,74,72,255};
Color l1_hazard_yellow = {232,185,35,255};
Color l1_rust = {181,84,28,255};
Color l1_rust_dark = {107,46,14,255};
Color l1_molten_orange = {255,106,0,255};
Color l1_molten_yellow = {255,208,0,255};
Color l1_molten_dark = {139,26,0,255};
Color l1_laser_red = {255,32,48,255};
Color l1_magnet_red = {192,40,45,255};

//l1_ball and l1_pot
Vector2 l1_ball;
Vector2 l1_speed;
float l1_radius_ball;
Vector2 l1_pot;
float l1_radius_pot;
Vector2 l1_start_position;
Vector2 l1_last_shot_position;
int l1_stroke = 0;
int l1_game_state = 0;
int l1_aiming = 0;
float l1_message_timer = 0;
int l1_message_type = 0;
float l1_animation_time = 0;

//lanes
float l1_hud_height;
float l1_wall;
float l1_gap;
float l1_lane_width;
float l1_lane_x[6];
float l1_course_top;
float l1_course_bottom;
Rectangle l1_walls[9];
Vector2 l1_no_speed = {0,0};

//valve wheel bumpers (one in lane 1, two next to the l1_pot)
Vector2 l1_bumper[3];
float l1_bumper_radius;

//l1_steel l1_crate
Rectangle l1_crate;

//conveyor belts (0 pushes down in lane 1, 1 pushes up in lane 4)
Rectangle l1_belt[2];
float l1_belt_push[2];

//warning l1_diamond
Vector2 l1_diamond;
float l1_diamond_radius;

//l1_steel girders
Rectangle l1_girder[2];

//l1_oil slick
Vector2 l1_oil;
float l1_oil_radius;

//spinning l1_fan
Vector2 l1_fan;
float l1_fan_angle = 0;
float l1_fan_spin = 120;
float l1_fan_blade_length;
float l1_fan_blade_thickness;
float l1_fan_hub_radius;

//crane beams (0 in lane 3, 1 and 2 in lane 6)
Rectangle l1_beam[3];
float l1_beam_speed[3];
float l1_beam_left_limit[3];
float l1_beam_right_limit[3];

//l1_magnet
Vector2 l1_magnet;
float l1_magnet_core;
float l1_magnet_field;
float l1_magnet_pull;

//hydraulic l1_piston
Rectangle l1_piston;
float l1_piston_head;
float l1_piston_length = 0;
float l1_piston_max_length;
float l1_piston_speed = 0;
float l1_piston_timer = 0;
int l1_piston_state = 0;

//molten pits
Vector2 l1_pit[3];
float l1_pit_radius;

//l1_laser gate
Rectangle l1_laser;
float l1_laser_timer = 0;


void l1_reset_level()
{
    //sizes
    l1_hud_height = 70*l1_u;
    l1_wall = 28*l1_u;
    l1_gap = 230*l1_u;
    l1_lane_width = (l1_width - 7*l1_wall)/6;
    l1_course_top = l1_hud_height + l1_wall;
    l1_course_bottom = l1_height - l1_wall;
    l1_radius_ball = 7*l1_u;
    l1_radius_pot = 11*l1_u;

    float centre[6];
    for (int i=0; i<6; i++)
    {
        l1_lane_x[i] = l1_wall + i*(l1_lane_width + l1_wall);
        centre[i] = l1_lane_x[i] + l1_lane_width/2;
    }

    //frame l1_walls
    Rectangle top_wall = {0,l1_hud_height,l1_width,l1_wall};
    Rectangle bottom_wall = {0,l1_height-l1_wall,l1_width,l1_wall};
    Rectangle left_wall = {0,l1_hud_height,l1_wall,l1_height-l1_hud_height};
    Rectangle right_wall = {l1_width-l1_wall,l1_hud_height,l1_wall,l1_height-l1_hud_height};
    l1_walls[0] = top_wall;
    l1_walls[1] = bottom_wall;
    l1_walls[2] = left_wall;
    l1_walls[3] = right_wall;

    //divider l1_walls, the turn l1_gap is at the top after lane 1, 3, 5 and at the bottom after lane 2, 4
    for (int i=0; i<5; i++)
    {
        Rectangle divider = {l1_lane_x[i]+l1_lane_width,l1_course_top,l1_wall,l1_course_bottom-l1_course_top-l1_gap};
        if (i%2==0) divider.y = l1_course_top + l1_gap;
        l1_walls[4+i] = divider;
    }

    //l1_ball and l1_pot
    l1_start_position.x = centre[0];
    l1_start_position.y = l1_course_bottom - 70*l1_u;
    l1_ball = l1_start_position;
    l1_last_shot_position = l1_start_position;
    l1_speed.x = 0;
    l1_speed.y= 0;
    l1_pot.x = centre[5];
    l1_pot.y = l1_course_bottom - 110*l1_u;
    l1_stroke = 0;
    l1_game_state = 0;
    l1_aiming = 0;
    l1_message_timer = 0;

    //lane 1: l1_crate, valve wheel, conveyor pushing down
    l1_crate.x = centre[0] - 68*l1_u;
    l1_crate.y = l1_course_bottom - 233*l1_u;
    l1_crate.width = 46*l1_u;
    l1_crate.height = 46*l1_u;
    l1_bumper_radius = 26*l1_u;
    l1_bumper[0].x = centre[0] + 55*l1_u;
    l1_bumper[0].y = l1_course_bottom - 360*l1_u;
    l1_belt[0].x = l1_lane_x[0];
    l1_belt[0].y = l1_course_top + 330*l1_u;
    l1_belt[0].width = l1_lane_width;
    l1_belt[0].height = 64*l1_u;
    l1_belt_push[0] = 750*l1_u;

    //lane 2: l1_diamond, two girders, l1_oil slick
    l1_diamond_radius = 34*l1_u;
    l1_diamond.x = centre[1];
    l1_diamond.y = l1_course_top + 330*l1_u;
    l1_girder[0].x = l1_lane_x[1];
    l1_girder[0].y = l1_course_top + 500*l1_u;
    l1_girder[0].width = 150*l1_u;
    l1_girder[0].height = 18*l1_u;
    l1_girder[1].x = l1_lane_x[1] + l1_lane_width - 150*l1_u;
    l1_girder[1].y = l1_course_top + 640*l1_u;
    l1_girder[1].width = 150*l1_u;
    l1_girder[1].height = 18*l1_u;
    l1_oil_radius = 70*l1_u;
    l1_oil.x = centre[1];
    l1_oil.y = l1_course_bottom - 170*l1_u;

    //lane 3: crane l1_beam, spinning l1_fan
    l1_beam[0].x = l1_lane_x[2] + 42*l1_u;
    l1_beam[0].y = l1_course_bottom - 330*l1_u;
    l1_fan_blade_length = 190*l1_u;
    l1_fan_blade_thickness = 12*l1_u;
    l1_fan_hub_radius = 16*l1_u;
    l1_fan.x = centre[2];
    l1_fan.y = l1_course_top + 380*l1_u;
    l1_fan_angle = 0;

    //lane 4: conveyor pushing up, l1_piston, l1_magnet
    l1_belt[1].x = l1_lane_x[3];
    l1_belt[1].y = l1_course_top + 250*l1_u;
    l1_belt[1].width = l1_lane_width;
    l1_belt[1].height = 64*l1_u;
    l1_belt_push[1] = -750*l1_u;
    l1_piston_head = 30*l1_u;
    l1_piston.x = l1_lane_x[3];
    l1_piston.y = l1_course_top + 435*l1_u;
    l1_piston.width = l1_piston_head;
    l1_piston.height = 70*l1_u;
    l1_piston_max_length = l1_lane_width - l1_piston_head - 42*l1_u;
    l1_piston_length = 0;
    l1_piston_speed = 0;
    l1_piston_timer = 0;
    l1_piston_state = 0;
    l1_magnet_core = 22*l1_u;
    l1_magnet_field = 170*l1_u;
    l1_magnet_pull = 450*l1_u;
    l1_magnet.x = centre[3];
    l1_magnet.y = l1_course_top + 720*l1_u;

    //lane 5: two molten pits, l1_laser gate
    l1_pit_radius = 38*l1_u;
    l1_pit[0].x = centre[4] - 55*l1_u;
    l1_pit[0].y = l1_course_bottom - 330*l1_u;
    l1_pit[1].x = centre[4] + 55*l1_u;
    l1_pit[1].y = l1_course_top + 400*l1_u;
    l1_laser.x = l1_lane_x[4];
    l1_laser.y = l1_course_top + 256*l1_u;
    l1_laser.width = l1_lane_width;
    l1_laser.height = 8*l1_u;
    l1_laser_timer = 0;

    //lane 6: two crane beams, molten l1_pit, two bumpers guarding the l1_pot
    l1_beam[1].x = l1_lane_x[5] + 42*l1_u;
    l1_beam[1].y = l1_course_top + 300*l1_u;
    l1_beam[2].x = l1_lane_x[5] + l1_lane_width - 42*l1_u - 110*l1_u;
    l1_beam[2].y = l1_course_top + 440*l1_u;
    l1_pit[2].x = centre[5];
    l1_pit[2].y = l1_course_bottom - 300*l1_u;
    l1_bumper[1].x = centre[5] - 75*l1_u;
    l1_bumper[1].y = l1_course_bottom - 190*l1_u;
    l1_bumper[2].x = centre[5] + 75*l1_u;
    l1_bumper[2].y = l1_course_bottom - 190*l1_u;

    //same size and l1_speed for all crane beams
    for (int i=0; i<3; i++)
    {
        l1_beam[i].width = 110*l1_u;
        l1_beam[i].height = 18*l1_u;
        int lane = 2;
        if (i>0) lane = 5;
        l1_beam_left_limit[i] = l1_lane_x[lane] + 42*l1_u;
        l1_beam_right_limit[i] = l1_lane_x[lane] + l1_lane_width - 42*l1_u - l1_beam[i].width;
    }
    l1_beam_speed[0] = 110*l1_u;
    l1_beam_speed[1] = 110*l1_u;
    l1_beam_speed[2] = -110*l1_u;
}

//bounce off a straight rectangle, rec_speed is how fast the rectangle itself is moving
void l1_bounce_off_rectangle(Rectangle rec, Vector2 rec_speed)
{
    if (CheckCollisionCircleRec(l1_ball,l1_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(l1_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(l1_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(l1_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = l1_ball.x - rec.x;
            float right = rec.x + rec.width - l1_ball.x;
            float top = l1_ball.y - rec.y;
            float bottom = rec.y + rec.height - l1_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }
        normal = Vector2Normalize(normal);
        l1_ball = Vector2Add(collision_point,Vector2Scale(normal,l1_radius_ball));

        //reflect only if the l1_ball is going into it
        Vector2 relative_speed = Vector2Subtract(l1_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l1_speed = Vector2Add(relative_speed,rec_speed);

            //moving l1_beam or l1_piston side: also knock the l1_ball out of its row, or it gets hit again and again
            if (rec_speed.x!=0 && normal.y==0)
            {
                if (l1_ball.y < rec.y+rec.height/2) l1_speed.y = l1_speed.y - fabsf(rec_speed.x)/2;
                else l1_speed.y = l1_speed.y + fabsf(rec_speed.x)/2;
            }
            l1_speed = Vector2ClampValue(l1_speed,0,l1_max_speed*l1_u);
        }
    }
}

//bounce off a round thing that doesn't move
void l1_bounce_off_circle(Vector2 center, float radius)
{
    Vector2 normal = Vector2Subtract(l1_ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + l1_radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        l1_ball = Vector2Add(center,Vector2Scale(normal,radius + l1_radius_ball));
        if (Vector2DotProduct(l1_speed,normal)<0) l1_speed = Vector2Reflect(l1_speed,normal);
    }
}

//bounce off a turned rectangle (l1_diamond, l1_fan blades), spin is in degrees per second
void l1_bounce_off_rotated_rectangle(Vector2 center, float rec_width, float rec_height, float angle, float spin)
{
    //look at the l1_ball as if the rectangle was not turned
    Vector2 local_ball = Vector2Rotate(Vector2Subtract(l1_ball,center),-angle*DEG2RAD);
    Rectangle rec = {-rec_width/2,-rec_height/2,rec_width,rec_height};
    if (CheckCollisionCircleRec(local_ball,l1_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(local_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(local_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(local_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = local_ball.x - rec.x;
            float right = rec.x + rec.width - local_ball.x;
            float top = local_ball.y - rec.y;
            float bottom = rec.y + rec.height - local_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }

        //turn the point and normal back to the screen
        normal = Vector2Rotate(Vector2Normalize(normal),angle*DEG2RAD);
        collision_point = Vector2Add(center,Vector2Rotate(collision_point,angle*DEG2RAD));
        l1_ball = Vector2Add(collision_point,Vector2Scale(normal,l1_radius_ball));

        //l1_speed of the blade at the point it touches the l1_ball
        Vector2 arm = Vector2Subtract(collision_point,center);
        Vector2 rec_speed = {-arm.y*spin*DEG2RAD, arm.x*spin*DEG2RAD};

        Vector2 relative_speed = Vector2Subtract(l1_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l1_speed = Vector2ClampValue(Vector2Add(relative_speed,rec_speed),0,l1_max_speed*l1_u);
        }
    }
}


void l1_update_obstacles(float dt)
{
    //crane beams moving
    for (int i=0; i<3; i++)
    {
        l1_beam[i].x = l1_beam[i].x + l1_beam_speed[i]*dt;
        if (l1_beam[i].x < l1_beam_left_limit[i])
        {
            l1_beam[i].x = l1_beam_left_limit[i];
            l1_beam_speed[i] = fabsf(l1_beam_speed[i]);
        }
        if (l1_beam[i].x > l1_beam_right_limit[i])
        {
            l1_beam[i].x = l1_beam_right_limit[i];
            l1_beam_speed[i] = -fabsf(l1_beam_speed[i]);
        }
    }

    //l1_fan spinning
    l1_fan_angle = l1_fan_angle + l1_fan_spin*dt;
    if (l1_fan_angle>=360) l1_fan_angle = l1_fan_angle - 360;

    //l1_laser: off 1.2s, warning 0.4s, on 1.4s
    l1_laser_timer = l1_laser_timer + dt;
    if (l1_laser_timer>=3.0) l1_laser_timer = l1_laser_timer - 3.0;

    //l1_piston: 0 waiting, 1 going out, 2 holding, 3 going back
    l1_piston_timer = l1_piston_timer + dt;
    l1_piston_speed = 0;
    if (l1_piston_state==0)
    {
        if (l1_piston_timer>=1.2)
        {
            l1_piston_state = 1;
            l1_piston_timer = 0;
        }
    }
    else if (l1_piston_state==1)
    {
        l1_piston_length = l1_piston_length + 900*l1_u*dt;
        l1_piston_speed = 900*l1_u;
        if (l1_piston_length>=l1_piston_max_length)
        {
            l1_piston_length = l1_piston_max_length;
            l1_piston_state = 2;
            l1_piston_timer = 0;
        }
    }
    else if (l1_piston_state==2)
    {
        if (l1_piston_timer>=0.3)
        {
            l1_piston_state = 3;
            l1_piston_timer = 0;
        }
    }
    else if (l1_piston_state==3)
    {
        l1_piston_length = l1_piston_length - 250*l1_u*dt;
        l1_piston_speed = -250*l1_u;
        if (l1_piston_length<=0)
        {
            l1_piston_length = 0;
            l1_piston_state = 0;
            l1_piston_timer = 0;
        }
    }
    l1_piston.width = l1_piston_head + l1_piston_length;
}


//after lava or l1_laser, the l1_ball goes back to where it was shot from
void l1_send_ball_back()
{
    l1_ball = l1_last_shot_position;

    //not inside the l1_laser
    if (CheckCollisionCircleRec(l1_ball,l1_radius_ball,l1_laser)) l1_ball = l1_start_position;

    //not right next to the l1_piston's path, or it gets hit into the lava again and again
    if (l1_ball.x>l1_piston.x && l1_ball.x<l1_piston.x+l1_piston_head+l1_piston_max_length+l1_radius_ball && l1_ball.y>l1_piston.y-l1_radius_ball-1*l1_u && l1_ball.y<l1_piston.y+l1_piston.height+l1_radius_ball+1*l1_u)
    {
        if (l1_ball.y < l1_piston.y+l1_piston.height/2) l1_ball.y = l1_piston.y - l1_radius_ball - 2*l1_u;
        else l1_ball.y = l1_piston.y + l1_piston.height + l1_radius_ball + 2*l1_u;
    }

    l1_speed.x = 0;
    l1_speed.y = 0;
}


void l1_update_ball(float dt)
{
    int pushed = 0;

    //conveyor belts
    for (int i=0; i<2; i++)
    {
        if (l1_ball.x>l1_belt[i].x && l1_ball.x<l1_belt[i].x+l1_belt[i].width && l1_ball.y>l1_belt[i].y && l1_ball.y<l1_belt[i].y+l1_belt[i].height)
        {
            l1_speed.y = l1_speed.y + l1_belt_push[i]*dt;
            pushed = 1;
        }
    }

    //l1_magnet pull
    Vector2 to_magnet = Vector2Subtract(l1_magnet,l1_ball);
    float magnet_distance = Vector2Length(to_magnet);
    if (magnet_distance<l1_magnet_field && magnet_distance>l1_magnet_core+l1_radius_ball+1*l1_u)
    {
        l1_speed = Vector2Add(l1_speed,Vector2Scale(Vector2Normalize(to_magnet),l1_magnet_pull*dt));
        pushed = 1;
    }

    //moving
    l1_ball = Vector2Add(l1_ball,Vector2Scale(l1_speed,dt));

    //proportional deceleration (slows the whole l1_speed, so the direction stays the same)
    float friction = 110*l1_u;
    if (CheckCollisionPointCircle(l1_ball,l1_oil,l1_oil_radius)) friction = 20*l1_u;
    float ball_speed = Vector2Length(l1_speed);
    if (ball_speed>0)
    {
        float new_speed = ball_speed - friction*dt;
        if (new_speed<0) new_speed = 0;
        l1_speed = Vector2Scale(l1_speed,new_speed/ball_speed);
    }
    if (pushed==0 && Vector2Length(l1_speed)<6*l1_u)
    {
        l1_speed.x = 0;
        l1_speed.y= 0;
    }

    //molten pits (checked before the collisions, so if a moving thing is on the old spot it pushes the l1_ball out)
    for (int i=0; i<3; i++)
    {
        if (Vector2Length(Vector2Subtract(l1_ball,l1_pit[i])) < l1_pit_radius - 5*l1_u)
        {
            l1_send_ball_back();
            l1_message_type = 1;
            l1_message_timer = 1;
        }
    }

    //l1_laser gate
    if (l1_laser_timer>=1.6 && CheckCollisionCircleRec(l1_ball,l1_radius_ball,l1_laser))
    {
        l1_send_ball_back();
        l1_message_type = 2;
        l1_message_timer = 1;
    }

    //collision with things that don't move
    for (int i=0; i<3; i++)
    {
        l1_bounce_off_circle(l1_bumper[i],l1_bumper_radius);
    }
    l1_bounce_off_rectangle(l1_crate,l1_no_speed);
    for (int i=0; i<2; i++)
    {
        l1_bounce_off_rectangle(l1_girder[i],l1_no_speed);
    }
    l1_bounce_off_rotated_rectangle(l1_diamond,l1_diamond_radius*sqrt(2),l1_diamond_radius*sqrt(2),45,0);

    //collision with things that move
    l1_bounce_off_circle(l1_fan,l1_fan_hub_radius);
    l1_bounce_off_rotated_rectangle(l1_fan,l1_fan_blade_length,l1_fan_blade_thickness,l1_fan_angle,l1_fan_spin);
    l1_bounce_off_rotated_rectangle(l1_fan,l1_fan_blade_length,l1_fan_blade_thickness,l1_fan_angle+90,l1_fan_spin);
    for (int i=0; i<3; i++)
    {
        Vector2 beam_velocity = {l1_beam_speed[i],0};
        l1_bounce_off_rectangle(l1_beam[i],beam_velocity);
    }
    Vector2 piston_velocity = {l1_piston_speed,0};
    l1_bounce_off_rectangle(l1_piston,piston_velocity);

    //l1_magnet core, the l1_ball sticks to it
    Vector2 away_from_magnet = Vector2Subtract(l1_ball,l1_magnet);
    float distance = Vector2Length(away_from_magnet);
    if (distance < l1_magnet_core + l1_radius_ball)
    {
        if (distance==0) away_from_magnet.y = -1;
        away_from_magnet = Vector2Normalize(away_from_magnet);
        l1_ball = Vector2Add(l1_magnet,Vector2Scale(away_from_magnet,l1_magnet_core + l1_radius_ball));
        if (Vector2DotProduct(l1_speed,away_from_magnet)<0)
        {
            l1_speed.x = 0;
            l1_speed.y = 0;
        }
    }

    //l1_wall bounce (last, so the l1_ball never ends inside a l1_wall)
    for (int i=0; i<9; i++)
    {
        l1_bounce_off_rectangle(l1_walls[i],l1_no_speed);
    }

    //score
    if ((l1_ball.x>l1_pot.x-3*l1_radius_pot/4) && (l1_ball.x<l1_pot.x+3*l1_radius_pot/4) && (l1_ball.y>l1_pot.y-3*l1_radius_pot/4) && (l1_ball.y<l1_pot.y+3*l1_radius_pot/4))
    {
        l1_ball = l1_pot;
        l1_speed.x = 0;
        l1_speed.y= 0;
        l1_game_state = 1;
    }
}


//yellow and black bands, across the long side
void l1_draw_hazard_stripes(Rectangle rec)
{
    DrawRectangleRec(rec,l1_hazard_yellow);
    float band = 6*l1_u;
    if (rec.width>=rec.height)
    {
        int bands = rec.width/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x+i*band,rec.y,band,rec.height,BLACK);
        }
    }
    else
    {
        int bands = rec.height/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x,rec.y+i*band,rec.width,band,BLACK);
        }
    }
}


void l1_draw_background()
{
    ClearBackground(l1_steel_dark);

    //lane floor
    DrawRectangle(l1_wall,l1_course_top,l1_width-2*l1_wall,l1_course_bottom-l1_course_top,l1_floor_colour);

    //l1_diamond plate tread marks
    int columns = (l1_width-2*l1_wall)/(34*l1_u);
    int rows = (l1_course_bottom-l1_course_top)/(34*l1_u);
    for (int i=0; i<=columns; i++)
    {
        for (int j=0; j<=rows; j++)
        {
            float x = l1_wall + i*34*l1_u + 17*l1_u;
            float y = l1_course_top + j*34*l1_u + 17*l1_u;
            Vector2 mark_start = {x-6*l1_u,y-6*l1_u};
            Vector2 mark_end = {x+6*l1_u,y+6*l1_u};
            if ((i+j)%2==1)
            {
                mark_start.y = y+6*l1_u;
                mark_end.y = y-6*l1_u;
            }
            DrawLineEx(mark_start,mark_end,4*l1_u,Fade(BLACK,0.15));
            mark_start.y = mark_start.y - 1*l1_u;
            mark_end.y = mark_end.y - 1*l1_u;
            DrawLineEx(mark_start,mark_end,2*l1_u,Fade(l1_steel_light,0.25));
        }
    }

    //painted centre dashes and shadows next to the l1_walls
    for (int i=0; i<6; i++)
    {
        float centre = l1_lane_x[i] + l1_lane_width/2;
        int dashes = (l1_course_bottom-l1_course_top)/(60*l1_u);
        for (int j=0; j<dashes; j++)
        {
            DrawRectangle(centre-2*l1_u,l1_course_top+j*60*l1_u+17*l1_u,4*l1_u,26*l1_u,Fade(l1_hazard_yellow,0.18));
        }
        DrawRectangleGradientH(l1_lane_x[i],l1_course_top,20*l1_u,l1_course_bottom-l1_course_top,Fade(BLACK,0.35),BLANK);
        DrawRectangleGradientH(l1_lane_x[i]+l1_lane_width-20*l1_u,l1_course_top,20*l1_u,l1_course_bottom-l1_course_top,BLANK,Fade(BLACK,0.35));
    }
    DrawRectangleGradientV(l1_wall,l1_course_top,l1_width-2*l1_wall,20*l1_u,Fade(BLACK,0.35),BLANK);
}


void l1_draw_walls()
{
    float brick_height = 14*l1_u;
    float brick_length = 28*l1_u;
    for (int i=0; i<9; i++)
    {
        Rectangle rec = l1_walls[i];
        DrawRectangleRec(rec,l1_mortar);

        //bricks, every second row moved by half a l1_brick
        int rows = rec.height/brick_height + 1;
        for (int row=0; row<rows; row++)
        {
            float y = rec.y + row*brick_height;
            float bottom = y + brick_height;
            if (bottom>rec.y+rec.height) bottom = rec.y + rec.height;
            float x = rec.x;
            if (row%2==1) x = rec.x - brick_length/2;
            int count = 0;
            while (x < rec.x+rec.width)
            {
                float left = x;
                float right = x + brick_length;
                if (left<rec.x) left = rec.x;
                if (right>rec.x+rec.width) right = rec.x + rec.width;
                if (right-left>3*l1_u && bottom-y>3*l1_u)
                {
                    Color colour = l1_brick;
                    if ((row*3 + count*7)%5==0) colour = l1_brick_dark;
                    DrawRectangle(left+1*l1_u,y+1*l1_u,right-left-2*l1_u,bottom-y-2*l1_u,colour);
                    DrawRectangle(left+1*l1_u,bottom-3*l1_u,right-left-2*l1_u,2*l1_u,Fade(BLACK,0.25));
                }
                x = x + brick_length;
                count++;
            }
        }

        //l1_steel edge and rivets
        DrawRectangleLinesEx(rec,3*l1_u,l1_steel_dark);
        if (rec.height>rec.width)
        {
            int rivets = rec.height/(40*l1_u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+5*l1_u,rec.y+20*l1_u+j*40*l1_u,2.5*l1_u,l1_steel_light);
                DrawCircle(rec.x+rec.width-5*l1_u,rec.y+20*l1_u+j*40*l1_u,2.5*l1_u,l1_steel_light);
            }
        }
        else
        {
            int rivets = rec.width/(40*l1_u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+20*l1_u+j*40*l1_u,rec.y+5*l1_u,2.5*l1_u,l1_steel_light);
                DrawCircle(rec.x+20*l1_u+j*40*l1_u,rec.y+rec.height-5*l1_u,2.5*l1_u,l1_steel_light);
            }
        }
    }

    //hazard caps at the end of each divider
    for (int i=0; i<5; i++)
    {
        Rectangle cap = {l1_walls[4+i].x,l1_walls[4+i].y,l1_wall,22*l1_u};
        if (i%2==1) cap.y = l1_walls[4+i].y + l1_walls[4+i].height - 22*l1_u;
        l1_draw_hazard_stripes(cap);
        DrawRectangleLinesEx(cap,2*l1_u,BLACK);
    }
}


void l1_draw_path_arrows()
{
    float spacing = 90*l1_u;
    float offset = fmod(l1_animation_time*45*l1_u,spacing);
    Color arrow_colour = Fade(l1_hazard_yellow,0.3);

    //along the lanes
    int arrows = (l1_course_bottom-l1_course_top-l1_gap)/spacing;
    for (int i=0; i<6; i++)
    {
        Vector2 arrow;
        arrow.x = l1_lane_x[i] + l1_lane_width/2;
        for (int j=0; j<arrows; j++)
        {
            if (i%2==0)
            {
                arrow.y = l1_course_bottom - 115*l1_u - j*spacing - offset;
                DrawPoly(arrow,3,10*l1_u,270,arrow_colour);
            }
            else
            {
                arrow.y = l1_course_top + 115*l1_u + j*spacing + offset;
                DrawPoly(arrow,3,10*l1_u,90,arrow_colour);
            }
        }
    }

    //across the turns
    for (int i=0; i<5; i++)
    {
        Vector2 arrow;
        arrow.y = l1_course_top + 115*l1_u;
        if (i%2==1) arrow.y = l1_course_bottom - 115*l1_u;
        for (int j=0; j<3; j++)
        {
            arrow.x = l1_lane_x[i] + l1_lane_width/2 + 30*l1_u + j*spacing + offset;
            DrawPoly(arrow,3,10*l1_u,0,arrow_colour);
        }
    }
}


void l1_draw_obstacles()
{
    float t = l1_animation_time;

    //l1_oil slick
    DrawCircle(l1_oil.x,l1_oil.y,l1_oil_radius*0.75,Fade(BLACK,0.55));
    DrawCircle(l1_oil.x-30*l1_u,l1_oil.y+15*l1_u,l1_oil_radius*0.5,Fade(BLACK,0.5));
    DrawCircle(l1_oil.x+32*l1_u,l1_oil.y-12*l1_u,l1_oil_radius*0.5,Fade(BLACK,0.5));
    DrawCircle(l1_oil.x+8*l1_u,l1_oil.y+35*l1_u,l1_oil_radius*0.45,Fade(BLACK,0.45));
    DrawCircle(l1_oil.x-18*l1_u,l1_oil.y-35*l1_u,l1_oil_radius*0.4,Fade(BLACK,0.45));
    DrawRing(l1_oil,l1_oil_radius*0.3,l1_oil_radius*0.34,t*25,t*25+110,16,Fade(PURPLE,0.4));
    DrawRing(l1_oil,l1_oil_radius*0.5,l1_oil_radius*0.54,90-t*18,200-t*18,16,Fade(SKYBLUE,0.3));
    DrawRing(l1_oil,l1_oil_radius*0.66,l1_oil_radius*0.69,200+t*12,290+t*12,16,Fade(VIOLET,0.35));

    //conveyor belts
    for (int i=0; i<2; i++)
    {
        Rectangle b = l1_belt[i];
        DrawRectangleRec(b,GetColor(0x1E1E1EFF));
        float stripe = 16*l1_u;
        float move = fmod(t*80*l1_u,stripe);
        if (l1_belt_push[i]<0) move = stripe - move;
        int stripes = b.height/stripe;
        for (int j=0; j<stripes; j++)
        {
            float y = b.y + j*stripe + move;
            if (y+4*l1_u < b.y+b.height) DrawRectangle(b.x,y,b.width,4*l1_u,GetColor(0x3C3C3CFF));
        }
        //rollers
        DrawRectangleGradientV(b.x,b.y-4*l1_u,b.width,8*l1_u,l1_steel_light,l1_steel_dark);
        DrawRectangleGradientV(b.x,b.y+b.height-4*l1_u,b.width,8*l1_u,l1_steel_light,l1_steel_dark);
        //arrows
        for (int j=1; j<=3; j++)
        {
            Vector2 arrow = {b.x + j*b.width/4, b.y + b.height/2};
            if (l1_belt_push[i]>0) DrawPoly(arrow,3,12*l1_u,90,l1_hazard_yellow);
            else DrawPoly(arrow,3,12*l1_u,270,l1_hazard_yellow);
        }
    }

    //l1_steel l1_crate
    DrawRectangle(l1_crate.x+4*l1_u,l1_crate.y+5*l1_u,l1_crate.width,l1_crate.height,Fade(BLACK,0.4));
    DrawRectangleRec(l1_crate,l1_rust);
    DrawRectangleLinesEx(l1_crate,5*l1_u,l1_rust_dark);
    Vector2 crate_corner1 = {l1_crate.x+5*l1_u,l1_crate.y+5*l1_u};
    Vector2 crate_corner2 = {l1_crate.x+l1_crate.width-5*l1_u,l1_crate.y+l1_crate.height-5*l1_u};
    Vector2 crate_corner3 = {l1_crate.x+l1_crate.width-5*l1_u,l1_crate.y+5*l1_u};
    Vector2 crate_corner4 = {l1_crate.x+5*l1_u,l1_crate.y+l1_crate.height-5*l1_u};
    DrawLineEx(crate_corner1,crate_corner2,4*l1_u,l1_rust_dark);
    DrawLineEx(crate_corner3,crate_corner4,4*l1_u,l1_rust_dark);
    DrawCircle(crate_corner1.x,crate_corner1.y,2.5*l1_u,l1_steel_light);
    DrawCircle(crate_corner2.x,crate_corner2.y,2.5*l1_u,l1_steel_light);
    DrawCircle(crate_corner3.x,crate_corner3.y,2.5*l1_u,l1_steel_light);
    DrawCircle(crate_corner4.x,crate_corner4.y,2.5*l1_u,l1_steel_light);

    //valve wheel bumpers
    for (int i=0; i<3; i++)
    {
        Vector2 b = l1_bumper[i];
        DrawCircle(b.x+3*l1_u,b.y+4*l1_u,l1_bumper_radius,Fade(BLACK,0.4));
        DrawCircle(b.x,b.y,l1_bumper_radius,l1_steel);
        DrawRing(b,l1_bumper_radius-6*l1_u,l1_bumper_radius,0,360,32,l1_magnet_red);
        Vector2 spoke = {l1_bumper_radius-6*l1_u,0};
        for (int j=0; j<4; j++)
        {
            Vector2 turned = Vector2Rotate(spoke,j*45*DEG2RAD);
            DrawLineEx(Vector2Add(b,turned),Vector2Subtract(b,turned),4*l1_u,l1_steel_dark);
        }
        DrawCircle(b.x,b.y,7*l1_u,l1_steel_dark);
        DrawCircle(b.x,b.y,3*l1_u,l1_steel_light);
    }

    //warning l1_diamond
    Vector2 diamond_shadow = {l1_diamond.x+3*l1_u,l1_diamond.y+4*l1_u};
    DrawPoly(diamond_shadow,4,l1_diamond_radius,0,Fade(BLACK,0.4));
    DrawPoly(l1_diamond,4,l1_diamond_radius,0,l1_hazard_yellow);
    DrawPolyLinesEx(l1_diamond,4,l1_diamond_radius,0,4*l1_u,BLACK);
    DrawText("!",l1_diamond.x-MeasureText("!",30*l1_u)/2,l1_diamond.y-14*l1_u,30*l1_u,BLACK);

    //l1_steel girders
    for (int i=0; i<2; i++)
    {
        Rectangle g = l1_girder[i];
        DrawRectangle(g.x,g.y+4*l1_u,g.width,g.height,Fade(BLACK,0.35));
        DrawRectangleRec(g,l1_steel);
        DrawRectangle(g.x,g.y,g.width,4*l1_u,l1_steel_light);
        DrawRectangle(g.x,g.y+g.height-4*l1_u,g.width,4*l1_u,l1_steel_dark);
        int rivets = g.width/(20*l1_u);
        for (int j=1; j<rivets; j++)
        {
            DrawCircle(g.x+j*20*l1_u,g.y+g.height/2,2*l1_u,l1_steel_light);
        }
        Rectangle girder_end = {g.x+g.width-20*l1_u,g.y,20*l1_u,g.height};
        if (i==1) girder_end.x = g.x;
        l1_draw_hazard_stripes(girder_end);
        DrawRectangleLinesEx(g,2*l1_u,BLACK);
    }

    //crane beams
    for (int i=0; i<3; i++)
    {
        DrawRectangle(l1_beam[i].x+5*l1_u,l1_beam[i].y+6*l1_u,l1_beam[i].width,l1_beam[i].height,Fade(BLACK,0.4));
        l1_draw_hazard_stripes(l1_beam[i]);
        DrawRectangleLinesEx(l1_beam[i],3*l1_u,BLACK);
        DrawCircle(l1_beam[i].x+l1_beam[i].width/2,l1_beam[i].y+l1_beam[i].height/2,4*l1_u,l1_steel_dark);
    }

    //spinning l1_fan
    DrawCircle(l1_fan.x,l1_fan.y,l1_fan_blade_length/2+10*l1_u,Fade(BLACK,0.3));
    DrawRing(l1_fan,l1_fan_blade_length/2+4*l1_u,l1_fan_blade_length/2+10*l1_u,0,360,48,l1_steel_dark);
    DrawRing(l1_fan,l1_fan_blade_length/2+8*l1_u,l1_fan_blade_length/2+10*l1_u,0,360,48,l1_steel_light);
    Rectangle blade = {l1_fan.x,l1_fan.y,l1_fan_blade_length,l1_fan_blade_thickness};
    Vector2 blade_origin = {l1_fan_blade_length/2,l1_fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,l1_fan_angle-24,Fade(l1_steel_light,0.12));
    DrawRectanglePro(blade,blade_origin,l1_fan_angle+90-24,Fade(l1_steel_light,0.12));
    DrawRectanglePro(blade,blade_origin,l1_fan_angle-12,Fade(l1_steel_light,0.25));
    DrawRectanglePro(blade,blade_origin,l1_fan_angle+90-12,Fade(l1_steel_light,0.25));
    DrawRectanglePro(blade,blade_origin,l1_fan_angle,l1_steel_light);
    DrawRectanglePro(blade,blade_origin,l1_fan_angle+90,l1_steel_light);
    DrawCircle(l1_fan.x,l1_fan.y,l1_fan_hub_radius,l1_steel_dark);
    DrawCircleLines(l1_fan.x,l1_fan.y,l1_fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*l1_u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(l1_fan,Vector2Rotate(bolt_arm,(l1_fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*l1_u,l1_steel_light);
    }

    //hydraulic l1_piston
    Rectangle housing = {l1_piston.x-l1_wall,l1_piston.y-8*l1_u,l1_wall,l1_piston.height+16*l1_u};
    DrawRectangleRec(housing,l1_steel_dark);
    for (int j=0; j<4; j++)
    {
        DrawRectangle(housing.x,housing.y+8*l1_u+j*20*l1_u,housing.width,5*l1_u,l1_steel);
    }
    DrawRectangleLinesEx(housing,2*l1_u,BLACK);
    DrawRectangleGradientV(l1_piston.x,l1_piston.y,l1_piston_length,l1_piston.height,l1_steel,l1_steel_dark);
    DrawRectangleGradientV(l1_piston.x,l1_piston.y+l1_piston.height/2-10*l1_u,l1_piston_length,20*l1_u,RAYWHITE,l1_steel_light);
    Rectangle head = {l1_piston.x+l1_piston_length,l1_piston.y,l1_piston_head,l1_piston.height};
    l1_draw_hazard_stripes(head);
    DrawRectangleLinesEx(head,3*l1_u,BLACK);
    DrawRectangleLinesEx(l1_piston,2*l1_u,BLACK);
    Color lamp = GetColor(0x4A3A20FF);
    if (l1_piston_state==0 && l1_piston_timer>=0.8 && fmod(t*8,2)<1) lamp = l1_hazard_yellow;
    if (l1_piston_state==1 || l1_piston_state==2) lamp = l1_laser_red;
    DrawCircle(housing.x+l1_wall/2,housing.y-12*l1_u,9*l1_u,Fade(lamp,0.35));
    DrawCircle(housing.x+l1_wall/2,housing.y-12*l1_u,6*l1_u,lamp);

    //molten pits
    for (int i=0; i<3; i++)
    {
        Vector2 p = l1_pit[i];
        float pulse = sin(t*3 + i*2);
        DrawCircleGradient(p,l1_pit_radius*1.9,Fade(l1_molten_orange,0.3+0.1*pulse),BLANK);
        DrawCircle(p.x,p.y,l1_pit_radius+5*l1_u,GetColor(0x2A1A12FF));
        DrawCircleGradient(p,l1_pit_radius,l1_molten_orange,l1_molten_dark);
        DrawCircleGradient(p,l1_pit_radius*(0.55+0.08*pulse),l1_molten_yellow,Fade(l1_molten_orange,0));
        Vector2 lump_arm = {l1_pit_radius+1*l1_u,0};
        for (int j=0; j<8; j++)
        {
            Vector2 lump = Vector2Add(p,Vector2Rotate(lump_arm,(j*45+i*20)*DEG2RAD));
            DrawCircle(lump.x,lump.y,3.5*l1_u,GetColor(0x3B2418FF));
        }
        //bubbles
        for (int j=0; j<4; j++)
        {
            float life = fmod(t*0.8 + j*0.25 + i*0.13,1.0);
            float bubble_x = p.x + sin(j*2.1 + i)*l1_pit_radius*0.5;
            float bubble_y = p.y + cos(j*1.7 + i)*l1_pit_radius*0.5;
            DrawCircle(bubble_x,bubble_y,(1.5+life*3.5)*l1_u,Fade(l1_molten_yellow,(1-life)*0.9));
        }
    }

    //l1_laser gate
    float beam_y = l1_laser.y + l1_laser.height/2;
    Rectangle left_emitter = {l1_laser.x-l1_wall+4*l1_u,beam_y-14*l1_u,l1_wall-4*l1_u,28*l1_u};
    Rectangle right_emitter = {l1_laser.x+l1_laser.width,beam_y-14*l1_u,l1_wall-4*l1_u,28*l1_u};
    DrawRectangleRec(left_emitter,l1_steel_dark);
    DrawRectangleRec(right_emitter,l1_steel_dark);
    DrawRectangleLinesEx(left_emitter,2*l1_u,BLACK);
    DrawRectangleLinesEx(right_emitter,2*l1_u,BLACK);
    Color laser_lamp = GetColor(0x4A3A20FF);
    int dashes = l1_laser.width/(16*l1_u);
    if (l1_laser_timer<1.6)
    {
        float dash_alpha = 0.3;
        if (l1_laser_timer>=1.2)
        {
            dash_alpha = 0.55;
            if (fmod(t*10,2)<1) laser_lamp = l1_hazard_yellow;
        }
        for (int j=0; j<dashes; j++)
        {
            DrawRectangle(l1_laser.x+j*16*l1_u+4*l1_u,beam_y-1*l1_u,8*l1_u,2*l1_u,Fade(l1_laser_red,dash_alpha));
        }
    }
    else
    {
        laser_lamp = l1_laser_red;
        DrawRectangle(l1_laser.x,beam_y-14*l1_u,l1_laser.width,28*l1_u,Fade(l1_laser_red,0.15));
        DrawRectangle(l1_laser.x,beam_y-7*l1_u,l1_laser.width,14*l1_u,Fade(l1_laser_red,0.3));
        DrawRectangleRec(l1_laser,l1_laser_red);
        DrawRectangle(l1_laser.x,beam_y-1*l1_u,l1_laser.width,2*l1_u,WHITE);
    }
    DrawCircle(left_emitter.x+left_emitter.width/2,beam_y,5*l1_u,laser_lamp);
    DrawCircle(right_emitter.x+right_emitter.width/2,beam_y,5*l1_u,laser_lamp);

    //l1_magnet field rings
    for (int j=0; j<3; j++)
    {
        float ring = l1_magnet_field - fmod(t*50*l1_u + j*(l1_magnet_field-l1_magnet_core)/3,l1_magnet_field-l1_magnet_core);
        DrawRing(l1_magnet,ring-1.5*l1_u,ring+1.5*l1_u,0,360,48,Fade(SKYBLUE,0.05+0.35*(1-ring/l1_magnet_field)));
    }
    DrawCircle(l1_magnet.x,l1_magnet.y,l1_magnet_field,Fade(SKYBLUE,0.04));

    //l1_magnet
    Vector2 magnet_shadow = {l1_magnet.x+3*l1_u,l1_magnet.y+4*l1_u};
    DrawRing(magnet_shadow,12*l1_u,28*l1_u,180,360,24,Fade(BLACK,0.4));
    DrawRing(l1_magnet,12*l1_u,28*l1_u,180,360,24,l1_magnet_red);
    DrawRectangle(l1_magnet.x-28*l1_u,l1_magnet.y,16*l1_u,8*l1_u,l1_magnet_red);
    DrawRectangle(l1_magnet.x+12*l1_u,l1_magnet.y,16*l1_u,8*l1_u,l1_magnet_red);
    DrawRectangle(l1_magnet.x-28*l1_u,l1_magnet.y+8*l1_u,16*l1_u,10*l1_u,l1_steel_light);
    DrawRectangle(l1_magnet.x+12*l1_u,l1_magnet.y+8*l1_u,16*l1_u,10*l1_u,l1_steel_light);
}


void l1_draw_ball_and_pot()
{
    //start plate
    Rectangle plate = {l1_start_position.x-50*l1_u,l1_start_position.y-30*l1_u,100*l1_u,60*l1_u};
    DrawRectangleRec(plate,l1_steel);
    DrawRectangleLinesEx(plate,3*l1_u,l1_steel_dark);
    Rectangle plate_band = {plate.x,plate.y+plate.height-10*l1_u,plate.width,10*l1_u};
    l1_draw_hazard_stripes(plate_band);
    DrawText("START",l1_start_position.x-MeasureText("START",18*l1_u)/2,plate.y+5*l1_u,18*l1_u,WHITE);

    //l1_pot with hazard ring
    for (int i=0; i<8; i++)
    {
        Color colour = l1_hazard_yellow;
        if (i%2==1) colour = BLACK;
        DrawRing(l1_pot,l1_radius_pot+6*l1_u,l1_radius_pot+12*l1_u,i*45,i*45+45,6,colour);
    }
    DrawCircle(l1_pot.x,l1_pot.y,l1_radius_pot+6*l1_u,l1_steel_light);
    DrawRing(l1_pot,l1_radius_pot+1*l1_u,l1_radius_pot+5*l1_u,0,360,24,l1_steel);
    DrawCircle(l1_pot.x,l1_pot.y,l1_radius_pot,BLACK);

    //flag
    Vector2 pole_top = {l1_pot.x,l1_pot.y-60*l1_u};
    DrawLineEx(l1_pot,pole_top,3*l1_u,l1_steel_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(l1_animation_time*6 - i*0.8)*2.5*l1_u;
        DrawRectangle(l1_pot.x+1*l1_u+i*6*l1_u,pole_top.y+wave,6*l1_u,18*l1_u,l1_laser_red);
    }

    //l1_ball
    DrawCircle(l1_ball.x+3*l1_u,l1_ball.y+4*l1_u,l1_radius_ball,Fade(BLACK,0.45));
    DrawCircle(l1_ball.x,l1_ball.y,l1_radius_ball,GetColor(0xEDEDEDFF));
    DrawCircleLines(l1_ball.x,l1_ball.y,l1_radius_ball,GRAY);
    DrawCircle(l1_ball.x-2*l1_u,l1_ball.y-2*l1_u,2*l1_u,WHITE);

    //aim line
    if (l1_aiming==1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        DrawLineEx(l1_ball,mouse,4*l1_u,Fade(WHITE,0.75));
        DrawCircle(mouse.x,mouse.y,5*l1_u,Fade(WHITE,0.75));
    }
}


void l1_draw_hud()
{
    //dark edges
    DrawRectangleGradientH(0,l1_hud_height,90*l1_u,l1_height-l1_hud_height,Fade(BLACK,0.45),BLANK);
    DrawRectangleGradientH(l1_width-90*l1_u,l1_hud_height,90*l1_u,l1_height-l1_hud_height,BLANK,Fade(BLACK,0.45));
    DrawRectangleGradientV(0,l1_height-70*l1_u,l1_width,70*l1_u,BLANK,Fade(BLACK,0.45));

    //l1_steel bar
    DrawRectangleGradientV(0,0,l1_width,l1_hud_height,l1_steel_light,l1_steel_dark);
    DrawRectangle(0,l1_hud_height-4*l1_u,l1_width,4*l1_u,BLACK);
    int rivets = l1_width/(40*l1_u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*l1_u+i*40*l1_u,8*l1_u,3*l1_u,l1_steel_dark);
        DrawCircle(20*l1_u+i*40*l1_u,l1_hud_height-12*l1_u,3*l1_u,l1_steel_dark);
    }

    //text
    DrawText("LEVEL 1 - FOUNDRY",32*l1_u,20*l1_u,36*l1_u,BLACK);
    DrawText("LEVEL 1 - FOUNDRY",30*l1_u,18*l1_u,36*l1_u,l1_hazard_yellow);
    DrawText(TextFormat("STROKES %d / %d",l1_stroke,l1_stroke_limit),l1_width/2-298*l1_u,21*l1_u,32*l1_u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",l1_stroke,l1_stroke_limit),l1_width/2-300*l1_u,19*l1_u,32*l1_u,WHITE);
    for (int i=0; i<l1_stroke_limit; i++)
    {
        Color pip = l1_steel_dark;
        if (i<l1_stroke) pip = l1_laser_red;
        DrawCircle(l1_width/2+10*l1_u+i*22*l1_u,35*l1_u,7*l1_u,pip);
        DrawCircleLines(l1_width/2+10*l1_u+i*22*l1_u,35*l1_u,7*l1_u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC menu",26*l1_u);
    DrawText("R restart   ESC menu",l1_width-help_width-28*l1_u,24*l1_u,26*l1_u,BLACK);
    DrawText("R restart   ESC menu",l1_width-help_width-30*l1_u,22*l1_u,26*l1_u,WHITE);

    //melted or zapped message
    if (l1_message_timer>0)
    {
        if (l1_message_type==1)
        {
            DrawText("MELTED!",l1_width/2-MeasureText("MELTED!",90*l1_u)/2+4*l1_u,l1_height/2-41*l1_u,90*l1_u,Fade(BLACK,l1_message_timer));
            DrawText("MELTED!",l1_width/2-MeasureText("MELTED!",90*l1_u)/2,l1_height/2-45*l1_u,90*l1_u,Fade(l1_molten_orange,l1_message_timer));
        }
        else
        {
            DrawText("ZAPPED!",l1_width/2-MeasureText("ZAPPED!",90*l1_u)/2+4*l1_u,l1_height/2-41*l1_u,90*l1_u,Fade(BLACK,l1_message_timer));
            DrawText("ZAPPED!",l1_width/2-MeasureText("ZAPPED!",90*l1_u)/2,l1_height/2-45*l1_u,90*l1_u,Fade(l1_laser_red,l1_message_timer));
        }
    }

    //level clear or failed
    if (l1_game_state!=0)
    {
        DrawRectangle(0,0,l1_width,l1_height,Fade(BLACK,0.6));
        Rectangle panel = {l1_width/2-340*l1_u,l1_height/2-170*l1_u,680*l1_u,340*l1_u};
        DrawRectangleRec(panel,l1_steel_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*l1_u};
        l1_draw_hazard_stripes(panel_band);
        DrawRectangleLinesEx(panel,6*l1_u,l1_steel_light);
        if (l1_game_state==1)
        {
            DrawText("LEVEL CLEAR!",l1_width/2-MeasureText("LEVEL CLEAR!",80*l1_u)/2,panel.y+60*l1_u,80*l1_u,l1_hazard_yellow);
        }
        else
        {
            DrawText("FAILED",l1_width/2-MeasureText("FAILED",80*l1_u)/2,panel.y+60*l1_u,80*l1_u,l1_laser_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",l1_stroke,l1_stroke_limit),40*l1_u);
        DrawText(TextFormat("Strokes: %d / %d",l1_stroke,l1_stroke_limit),l1_width/2-strokes_width/2,panel.y+170*l1_u,40*l1_u,WHITE);
        DrawText("R play again   ESC menu",l1_width/2-MeasureText("R play again   ESC menu",30*l1_u)/2,panel.y+250*l1_u,30*l1_u,l1_steel_light);
    }
}


//level 1: keep the obstacles moving with no ball (intro and menu)
void l1_background_step(float dt)
{
    l1_animation_time = l1_animation_time + dt;
    l1_update_obstacles(dt);
}


//level 1: draw everything except the scoreboard (intro and menu)
void l1_draw_scene()
{

    l1_draw_background();
    l1_draw_path_arrows();
    l1_draw_walls();
    l1_draw_obstacles();
    l1_draw_ball_and_pot();

}


//level 1: set the screen size, load pictures, start the level
void l1_start(int screen_width, int screen_height)
{
    l1_width = screen_width;
    l1_height = screen_height;

    //every size is N*l1_u, so it looks the same on any screen
    l1_u = l1_height/1080.0;
    if (l1_width/1920.0 < l1_u) l1_u = l1_width/1920.0;
    l1_reset_level();

}


//level 1: one frame of input and movement
void l1_update()
{
    float dt = GetFrameTime();
    if (dt>1.0/30) dt = 1.0/30;
    l1_animation_time = l1_animation_time + dt;
    if (l1_message_timer>0) l1_message_timer = l1_message_timer - dt;

    //restart
    if (IsKeyPressed(KEY_R)) l1_reset_level();

    //shooting (the click has to start while the l1_ball is still)
    int ball_stopped = 0;
    if (l1_speed.x==0 && l1_speed.y==0) ball_stopped = 1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && l1_game_state==0) l1_aiming = 1;
    if (l1_game_state!=0) l1_aiming = 0;
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && l1_aiming==1)
    {
        l1_aiming = 0;
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        Vector2 drag = Vector2Subtract(l1_ball,mouse);
        if (ball_stopped==1 && l1_stroke<l1_stroke_limit && Vector2Length(drag)>=2*l1_radius_ball)
        {
            l1_last_shot_position = l1_ball;
            l1_speed = Vector2ClampValue(Vector2Scale(drag,3.125),0,l1_max_speed*l1_u);
            l1_stroke++;
        }
    }

    //moving everything in 4 small steps, so nothing jumps through anything
    for (int i=0; i<4; i++)
    {
        l1_update_obstacles(dt/4*obstacle_speed);
        if (l1_game_state==0) l1_update_ball(dt/4);
    }

    //out of strokes
    if (l1_game_state==0 && l1_stroke>=l1_stroke_limit && l1_speed.x==0 && l1_speed.y==0) l1_game_state = 2;


}


//level 1: one frame of drawing
void l1_draw()
{

    l1_draw_background();
    l1_draw_path_arrows();
    l1_draw_walls();
    l1_draw_obstacles();
    l1_draw_ball_and_pot();
    draw_straight_preview(l1_ball,l1_aiming,l1_radius_ball,l1_max_speed*l1_u,l1_u);
    l1_draw_hud();

}


void l1_unload()
{
}


//==================== LEVEL 2 ====================

//full global
int l2_width = 1920;
int l2_height = 1080;
float l2_u = 1;
int l2_stroke_base = 25;
int l2_stroke_limit = 25;
#define l2_max_speed 650

//colours
Color l2_sand_light = {245,232,200,255};
Color l2_wood = {139,94,52,255};
Color l2_wood_dark = {92,60,32,255};
Color l2_wood_light = {186,138,88,255};
Color l2_sun_yellow = {255,204,64,255};
Color l2_coral_red = {235,87,70,255};
Color l2_sea = {63,180,207,255};
Color l2_sea_deep = {26,127,168,255};

//pictures
Texture2D l2_sand_texture;
Texture2D l2_water_texture;
Texture2D l2_foam_texture;
Texture2D l2_crab_texture;
Texture2D l2_palm_texture;
Texture2D l2_splash_texture;

//l2_ball and l2_pot
Vector2 l2_ball;
Vector2 l2_speed;
float l2_radius_ball;
Vector2 l2_pot;
float l2_radius_pot;
Vector2 l2_start_position;
Vector2 l2_last_shot_position;
int l2_stroke = 0;
int l2_game_state = 0;
int l2_aiming = 0;
float l2_message_timer = 0;
float l2_animation_time = 0;
float l2_hud_height;

//ground: l2_sand is safe, everything else is water
Rectangle l2_sand[10];
Rectangle l2_soft_sand[4];
Rectangle l2_wet_sand[2];

//l2_tidal l2_sand bars, safe only at low tide (tide clock: low 3s, rising 1s, high 3s, falling 1s)
Rectangle l2_tidal[2];
Vector2 l2_tidal_push[2];
float l2_tide_timer = 0;

//rip current in shallow water (the old conveyor belt)
Rectangle l2_ford;
Vector2 l2_ford_push;

//whirlpools (the old magnet)
Vector2 l2_whirlpool[2];
float l2_whirlpool_field[2];
float l2_whirlpool_pull;

//crabs (the old crane beams)
Rectangle l2_crab[4];
float l2_crab_speed[4];
float l2_crab_left_limit[4];
float l2_crab_right_limit[4];
float l2_crab_angry[4];

//l2_palm trees and beach balls
Vector2 l2_palm[4];
float l2_trunk_radius;
Vector2 l2_beach_ball[4];
float l2_beach_ball_radius;
float l2_beach_ball_hit[4];

//splash
Vector2 l2_splash_position;
float l2_splash_timer = 0;


//a rectangle in design units (a 1920 x 1080 screen), made to fit the real screen
Rectangle l2_make_rect(float x, float y, float w, float h)
{
    Rectangle rec = {x*l2_u,y*l2_u,w*l2_u,h*l2_u};
    return rec;
}

//a point in design units
Vector2 l2_make_point(float x, float y)
{
    Vector2 point = {x*l2_u,y*l2_u};
    return point;
}


void l2_reset_level()
{
    //sizes
    l2_hud_height = 70*l2_u;
    l2_radius_ball = 7*l2_u;
    l2_radius_pot = 11*l2_u;

    //l2_sand islands and strips, the path goes up and down from left to right
    l2_sand[0] = l2_make_rect(60,760,340,270);     //start island
    l2_sand[1] = l2_make_rect(140,330,170,440);    //strip going up
    l2_sand[2] = l2_make_rect(60,110,520,230);     //top left island
    l2_sand[3] = l2_make_rect(450,640,200,390);    //bottom middle island
    l2_sand[4] = l2_make_rect(640,920,330,110);    //its narrow neck
    l2_sand[5] = l2_make_rect(860,330,110,600);    //strip going up (rip current in the middle)
    l2_sand[6] = l2_make_rect(800,110,600,230);    //top middle island
    l2_sand[7] = l2_make_rect(1310,330,70,380);    //narrow bridge going down
    l2_sand[8] = l2_make_rect(1100,700,760,330);   //bottom right island
    l2_sand[9] = l2_make_rect(1480,110,380,280);   //last island with the l2_pot

    l2_soft_sand[0] = l2_make_rect(140,470,170,90);
    l2_soft_sand[1] = l2_make_rect(1230,820,220,150);
    l2_soft_sand[2] = l2_make_rect(1590,230,130,110);
    l2_soft_sand[3] = l2_make_rect(860,360,110,140);    //just past the rip current, so the l2_ball stops before the island
    l2_wet_sand[0] = l2_make_rect(360,118,210,95);
    l2_wet_sand[1] = l2_make_rect(1600,940,250,80);

    //l2_tidal bars, the rising tide pushes the l2_ball back towards land
    l2_tidal[0] = l2_make_rect(470,330,100,320);
    l2_tidal_push[0] = l2_make_point(0,-400);
    l2_tidal[1] = l2_make_rect(1700,380,110,330);
    l2_tidal_push[1] = l2_make_point(0,400);
    l2_tide_timer = 0;

    //rip current across the strip, pushing to the right
    l2_ford = l2_make_rect(860,520,110,220);
    l2_ford_push = l2_make_point(260,0);

    //whirlpools
    l2_whirlpool[0] = l2_make_point(730,820);
    l2_whirlpool_field[0] = 125*l2_u;
    l2_whirlpool[1] = l2_make_point(1450,560);
    l2_whirlpool_field[1] = 130*l2_u;
    l2_whirlpool_pull = 380*l2_u;

    //crabs walking left and right
    l2_crab[0] = l2_make_rect(160,625,44,30);
    l2_crab_left_limit[0] = 140*l2_u;
    l2_crab_right_limit[0] = 266*l2_u;
    l2_crab_speed[0] = 110*l2_u;
    l2_crab[1] = l2_make_rect(1100,125,44,30);
    l2_crab_left_limit[1] = 1010*l2_u;
    l2_crab_right_limit[1] = 1356*l2_u;
    l2_crab_speed[1] = 130*l2_u;
    l2_crab[2] = l2_make_rect(900,290,44,30);
    l2_crab_left_limit[2] = 880*l2_u;
    l2_crab_right_limit[2] = 1250*l2_u;
    l2_crab_speed[2] = -130*l2_u;
    l2_crab[3] = l2_make_rect(1500,745,44,30);
    l2_crab_left_limit[3] = 1450*l2_u;
    l2_crab_right_limit[3] = 1816*l2_u;
    l2_crab_speed[3] = 150*l2_u;

    //l2_palm trees (only the trunk is solid) and beach balls
    l2_trunk_radius = 14*l2_u;
    l2_palm[0] = l2_make_point(300,930);
    l2_palm[1] = l2_make_point(220,200);
    l2_palm[2] = l2_make_point(1180,225);
    l2_palm[3] = l2_make_point(1790,300);
    l2_beach_ball_radius = 24*l2_u;
    l2_beach_ball[0] = l2_make_point(330,800);
    l2_beach_ball[1] = l2_make_point(1000,225);
    l2_beach_ball[2] = l2_make_point(1700,160);
    l2_beach_ball[3] = l2_make_point(1545,330);
    for (int i=0; i<4; i++)
    {
        l2_crab_angry[i] = 0;
        l2_beach_ball_hit[i] = 0;
    }

    //l2_ball and l2_pot
    l2_start_position = l2_make_point(130,990);
    l2_ball = l2_start_position;
    l2_last_shot_position = l2_start_position;
    l2_speed.x = 0;
    l2_speed.y= 0;
    l2_pot = l2_make_point(1560,200);
    l2_stroke = 0;
    l2_game_state = 0;
    l2_aiming = 0;
    l2_message_timer = 0;
    l2_splash_timer = 0;
}

//bounce off a straight rectangle, rec_speed is how fast the rectangle itself is moving
int l2_bounce_off_rectangle(Rectangle rec, Vector2 rec_speed)
{
    if (CheckCollisionCircleRec(l2_ball,l2_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(l2_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(l2_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(l2_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = l2_ball.x - rec.x;
            float right = rec.x + rec.width - l2_ball.x;
            float top = l2_ball.y - rec.y;
            float bottom = rec.y + rec.height - l2_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }
        normal = Vector2Normalize(normal);
        l2_ball = Vector2Add(collision_point,Vector2Scale(normal,l2_radius_ball));

        //reflect only if the l2_ball is going into it
        Vector2 relative_speed = Vector2Subtract(l2_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l2_speed = Vector2Add(relative_speed,rec_speed);

            //moving l2_crab side: also knock the l2_ball out of its row, or it gets hit again and again
            if (rec_speed.x!=0 && normal.y==0)
            {
                if (l2_ball.y < rec.y+rec.height/2) l2_speed.y = l2_speed.y - fabsf(rec_speed.x)/2;
                else l2_speed.y = l2_speed.y + fabsf(rec_speed.x)/2;
            }
            l2_speed = Vector2ClampValue(l2_speed,0,l2_max_speed*l2_u);
        }
        return 1;
    }
    return 0;
}

//bounce off a round thing that doesn't move, bounce is 1 for a normal one and more than 1 for a bouncy one
int l2_bounce_off_circle(Vector2 center, float radius, float bounce)
{
    Vector2 normal = Vector2Subtract(l2_ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + l2_radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        l2_ball = Vector2Add(center,Vector2Scale(normal,radius + l2_radius_ball));
        if (Vector2DotProduct(l2_speed,normal)<0)
        {
            l2_speed = Vector2ClampValue(Vector2Scale(Vector2Reflect(l2_speed,normal),bounce),0,l2_max_speed*l2_u);
            return 1;
        }
    }
    return 0;
}


//is this point on l2_sand (or shallow water), and not on a l2_tidal bar at high tide
int l2_on_safe_ground(Vector2 point)
{
    for (int i=0; i<10; i++)
    {
        if (CheckCollisionPointRec(point,l2_sand[i])) return 1;
    }
    if (CheckCollisionPointRec(point,l2_ford)) return 1;
    if (l2_tide_timer<3.5 || l2_tide_timer>=7.5)
    {
        for (int i=0; i<2; i++)
        {
            if (CheckCollisionPointRec(point,l2_tidal[i])) return 1;
        }
    }
    return 0;
}


void l2_update_obstacles(float dt)
{
    //crabs walking (the old crane beams)
    for (int i=0; i<4; i++)
    {
        l2_crab[i].x = l2_crab[i].x + l2_crab_speed[i]*dt;
        if (l2_crab[i].x < l2_crab_left_limit[i])
        {
            l2_crab[i].x = l2_crab_left_limit[i];
            l2_crab_speed[i] = fabsf(l2_crab_speed[i]);
        }
        if (l2_crab[i].x > l2_crab_right_limit[i])
        {
            l2_crab[i].x = l2_crab_right_limit[i];
            l2_crab_speed[i] = -fabsf(l2_crab_speed[i]);
        }
        if (l2_crab_angry[i]>0) l2_crab_angry[i] = l2_crab_angry[i] - dt;
        if (l2_beach_ball_hit[i]>0) l2_beach_ball_hit[i] = l2_beach_ball_hit[i] - dt;
    }

    //tide clock
    l2_tide_timer = l2_tide_timer + dt;
    if (l2_tide_timer>=8) l2_tide_timer = l2_tide_timer - 8;

    if (l2_splash_timer>0) l2_splash_timer = l2_splash_timer - dt;
}


//after falling in the water, the l2_ball goes back to where it was shot from
void l2_send_ball_back()
{
    l2_ball = l2_last_shot_position;

    //not right in a l2_crab's path, or it gets knocked into the water again and again
    for (int i=0; i<4; i++)
    {
        if (l2_ball.x>l2_crab_left_limit[i]-l2_radius_ball && l2_ball.x<l2_crab_right_limit[i]+l2_crab[i].width+l2_radius_ball && l2_ball.y>l2_crab[i].y-l2_radius_ball-1*l2_u && l2_ball.y<l2_crab[i].y+l2_crab[i].height+l2_radius_ball+1*l2_u)
        {
            if (l2_ball.y < l2_crab[i].y+l2_crab[i].height/2) l2_ball.y = l2_crab[i].y - l2_radius_ball - 2*l2_u;
            else l2_ball.y = l2_crab[i].y + l2_crab[i].height + l2_radius_ball + 2*l2_u;
        }
    }

    //the old spot is under water now (tide), start again
    if (l2_on_safe_ground(l2_ball)==0) l2_ball = l2_start_position;

    l2_speed.x = 0;
    l2_speed.y = 0;
}


void l2_update_ball(float dt)
{
    int pushed = 0;

    //rip current (the old conveyor belt)
    if (CheckCollisionPointRec(l2_ball,l2_ford))
    {
        l2_speed = Vector2Add(l2_speed,Vector2Scale(l2_ford_push,dt));
        pushed = 1;
    }

    //rising tide gently pushes the l2_ball back towards land
    if (l2_tide_timer>=3 && l2_tide_timer<4)
    {
        for (int i=0; i<2; i++)
        {
            int on_sand = 0;
            for (int j=0; j<10; j++)
            {
                if (CheckCollisionPointRec(l2_ball,l2_sand[j])) on_sand = 1;
            }
            if (on_sand==0 && CheckCollisionPointRec(l2_ball,l2_tidal[i]))
            {
                if (Vector2Length(l2_speed)<150*l2_u) l2_speed = Vector2Add(l2_speed,Vector2Scale(l2_tidal_push[i],dt));
                pushed = 1;
            }
        }
    }

    //l2_whirlpool pull (the old magnet)
    for (int i=0; i<2; i++)
    {
        Vector2 to_whirlpool = Vector2Subtract(l2_whirlpool[i],l2_ball);
        float whirlpool_distance = Vector2Length(to_whirlpool);
        if (whirlpool_distance<l2_whirlpool_field[i] && whirlpool_distance>1*l2_u)
        {
            l2_speed = Vector2Add(l2_speed,Vector2Scale(Vector2Normalize(to_whirlpool),l2_whirlpool_pull*dt));
            pushed = 1;
        }
    }

    //moving
    l2_ball = Vector2Add(l2_ball,Vector2Scale(l2_speed,dt));

    //proportional deceleration (friction depends on the ground)
    float friction = 110*l2_u;
    for (int i=0; i<4; i++)
    {
        if (CheckCollisionPointRec(l2_ball,l2_soft_sand[i])) friction = 400*l2_u;
    }
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(l2_ball,l2_wet_sand[i])) friction = 20*l2_u;
    }
    if (CheckCollisionPointRec(l2_ball,l2_ford)) friction = 250*l2_u;
    float ball_speed = Vector2Length(l2_speed);
    if (ball_speed>0)
    {
        float new_speed = ball_speed - friction*dt;
        if (new_speed<0) new_speed = 0;
        l2_speed = Vector2Scale(l2_speed,new_speed/ball_speed);
    }
    if (pushed==0 && Vector2Length(l2_speed)<6*l2_u)
    {
        l2_speed.x = 0;
        l2_speed.y= 0;
    }

    //water (checked before the collisions, so if something is on the old spot it pushes the l2_ball out)
    if (l2_on_safe_ground(l2_ball)==0)
    {
        l2_splash_position = l2_ball;
        l2_splash_timer = 0.5;
        l2_send_ball_back();
        l2_message_timer = 1;
    }

    //l2_palm trunks and beach balls
    for (int i=0; i<4; i++)
    {
        l2_bounce_off_circle(l2_palm[i],l2_trunk_radius,1);
        if (l2_bounce_off_circle(l2_beach_ball[i],l2_beach_ball_radius,1.3)) l2_beach_ball_hit[i] = 0.15;
    }

    //crabs
    for (int i=0; i<4; i++)
    {
        Vector2 crab_velocity = {l2_crab_speed[i],0};
        if (l2_bounce_off_rectangle(l2_crab[i],crab_velocity)) l2_crab_angry[i] = 0.4;
    }

    //score
    if ((l2_ball.x>l2_pot.x-3*l2_radius_pot/4) && (l2_ball.x<l2_pot.x+3*l2_radius_pot/4) && (l2_ball.y>l2_pot.y-3*l2_radius_pot/4) && (l2_ball.y<l2_pot.y+3*l2_radius_pot/4))
    {
        l2_ball = l2_pot;
        l2_speed.x = 0;
        l2_speed.y= 0;
        l2_game_state = 1;
    }
}


//l2_sand picture that repeats, lined up with the screen so pieces join without seams (the picture is 2x size)
void l2_draw_sand(Rectangle rec, Color tint)
{
    Rectangle source = {rec.x/l2_u*2,rec.y/l2_u*2,rec.width/l2_u*2,rec.height/l2_u*2};
    Vector2 no_origin = {0,0};
    DrawTexturePro(l2_sand_texture,source,rec,no_origin,0,tint);
}


//foam along the 4 edges of a piece of ground, the bumpy side faces the water
void l2_draw_foam(Rectangle rec, Color tint)
{
    int frame = (int)(l2_animation_time*8)%4;
    float long_side = rec.width + 40*l2_u;
    Rectangle source = {rec.x/l2_u*2,frame*128+1,long_side/l2_u*2,126};
    Vector2 origin = {long_side/2,32*l2_u};

    //top and bottom edges (top one turned around)
    Rectangle top = {rec.x+rec.width/2,rec.y-18*l2_u,long_side,64*l2_u};
    DrawTexturePro(l2_foam_texture,source,top,origin,180,tint);
    Rectangle bottom = {rec.x+rec.width/2,rec.y+rec.height+18*l2_u,long_side,64*l2_u};
    DrawTexturePro(l2_foam_texture,source,bottom,origin,0,tint);

    //left and right edges
    long_side = rec.height + 40*l2_u;
    source.x = rec.y/l2_u*2;
    source.width = long_side/l2_u*2;
    origin.x = long_side/2;
    Rectangle left = {rec.x-18*l2_u,rec.y+rec.height/2,long_side,64*l2_u};
    DrawTexturePro(l2_foam_texture,source,left,origin,90,tint);
    Rectangle right = {rec.x+rec.width+18*l2_u,rec.y+rec.height/2,long_side,64*l2_u};
    DrawTexturePro(l2_foam_texture,source,right,origin,-90,tint);
}


void l2_draw_ground()
{
    float t = l2_animation_time;
    Vector2 no_origin = {0,0};

    //water tiles, 2 pictures swapping for the shimmer
    int water_frame = (int)(t*2)%2;
    Rectangle water_source = {water_frame*512,0,512,512};
    int columns = l2_width/(256*l2_u) + 1;
    int rows = l2_height/(256*l2_u) + 1;
    for (int i=0; i<columns; i++)
    {
        for (int j=0; j<rows; j++)
        {
            Rectangle tile = {i*256*l2_u,j*256*l2_u,256*l2_u+1,256*l2_u+1};
            DrawTexturePro(l2_water_texture,water_source,tile,no_origin,0,WHITE);
        }
    }

    //whirlpools
    for (int i=0; i<2; i++)
    {
        Vector2 w = l2_whirlpool[i];
        DrawCircleGradient(w,l2_whirlpool_field[i],Fade(l2_sea_deep,0.7),Fade(l2_sea,0));
        for (int j=0; j<4; j++)
        {
            float ring = 18*l2_u + j*24*l2_u;
            float angle = t*(320 - j*60) + j*70;
            DrawRing(w,ring-2*l2_u,ring+2*l2_u,angle,angle+230,24,Fade(WHITE,0.55-j*0.1));
        }
        DrawCircle(w.x,w.y,10*l2_u,l2_sea_deep);
    }

    //how high the tide is, 0 low and 1 high
    float tide_level = 0;
    if (l2_tide_timer>=3 && l2_tide_timer<4) tide_level = l2_tide_timer - 3;
    if (l2_tide_timer>=4 && l2_tide_timer<7) tide_level = 1;
    if (l2_tide_timer>=7) tide_level = 8 - l2_tide_timer;

    //foam first, then the ground on top, so the foam only shows on the water side
    for (int i=0; i<10; i++) l2_draw_foam(l2_sand[i],Fade(WHITE,0.85));
    for (int i=0; i<2; i++) l2_draw_foam(l2_tidal[i],Fade(WHITE,0.85*(1-tide_level)));

    //l2_tidal bars, the water comes over them
    Color wet = {222,196,150,255};
    for (int i=0; i<2; i++)
    {
        l2_draw_sand(l2_tidal[i],wet);
        DrawRectangleRec(l2_tidal[i],Fade(l2_sea,tide_level*0.9));
    }

    for (int i=0; i<10; i++) l2_draw_sand(l2_sand[i],WHITE);

    //soft l2_sand, darker with wind ripples
    Color soft = {226,196,140,255};
    for (int i=0; i<4; i++)
    {
        Rectangle s = l2_soft_sand[i];
        l2_draw_sand(s,soft);
        int ripple_rows = s.height/(14*l2_u);
        int pieces = s.width/(24*l2_u);
        for (int j=1; j<ripple_rows; j++)
        {
            float shift = 0;
            if (j%2==1) shift = 12*l2_u;
            for (int k=0; k<pieces; k++)
            {
                Vector2 a = {s.x + k*24*l2_u + shift, s.y + j*14*l2_u};
                Vector2 b = {a.x + 14*l2_u, a.y - 3*l2_u};
                if (b.x < s.x+s.width) DrawLineEx(a,b,2*l2_u,Fade(l2_wood,0.35));
            }
        }
        DrawRectangleLinesEx(s,2*l2_u,Fade(l2_wood,0.25));
    }

    //wet l2_sand, dark and shiny
    for (int i=0; i<2; i++)
    {
        Rectangle w = l2_wet_sand[i];
        Color wet_dark = {196,160,112,255};
        l2_draw_sand(w,wet_dark);
        for (int k=0; k<14; k++)
        {
            float shine_x = w.x + fmod(k*53*l2_u,w.width);
            float shine_y = w.y + fmod(k*31*l2_u + 7*l2_u,w.height);
            DrawCircle(shine_x,shine_y,2.5*l2_u,Fade(WHITE,0.25+0.25*sin(t*3+k)));
        }
    }

    //rip current, shallow water with foam lines moving the way it pushes
    Color shallow = {170,215,210,255};
    l2_draw_sand(l2_ford,shallow);
    DrawRectangleRec(l2_ford,Fade(l2_sea,0.45));
    int current_lines = l2_ford.height/(22*l2_u);
    for (int j=0; j<current_lines; j++)
    {
        Vector2 a = {l2_ford.x + fmod(t*120*l2_u + j*37*l2_u,l2_ford.width), l2_ford.y + 11*l2_u + j*22*l2_u};
        Vector2 b = {a.x + 18*l2_u, a.y};
        if (b.x > l2_ford.x+l2_ford.width) b.x = l2_ford.x + l2_ford.width;
        DrawLineEx(a,b,2*l2_u,Fade(WHITE,0.6));
    }
}


void l2_draw_obstacles()
{
    float t = l2_animation_time;

    //start towel
    Rectangle towel = {l2_start_position.x-45*l2_u,l2_start_position.y-28*l2_u,90*l2_u,56*l2_u};
    DrawRectangle(towel.x+4*l2_u,towel.y+5*l2_u,towel.width,towel.height,Fade(BLACK,0.2));
    for (int j=0; j<6; j++)
    {
        Color stripe = l2_coral_red;
        if (j%2==1) stripe = RAYWHITE;
        DrawRectangle(towel.x+j*15*l2_u,towel.y,15*l2_u,towel.height,stripe);
    }
    DrawText("START",l2_start_position.x-MeasureText("START",16*l2_u)/2,towel.y+4*l2_u,16*l2_u,l2_wood_dark);

    //splash, 5 pictures in half a second
    if (l2_splash_timer>0)
    {
        int frame = (0.5-l2_splash_timer)/0.1;
        if (frame>4) frame = 4;
        Rectangle source = {frame*192,0,192,192};
        Rectangle dest = {l2_splash_position.x,l2_splash_position.y,96*l2_u,96*l2_u};
        Vector2 origin = {48*l2_u,48*l2_u};
        DrawTexturePro(l2_splash_texture,source,dest,origin,0,WHITE);
    }

    //crabs, walking pictures 1-4, picture 5 when angry
    for (int i=0; i<4; i++)
    {
        Vector2 middle = {l2_crab[i].x+l2_crab[i].width/2,l2_crab[i].y+l2_crab[i].height/2};
        DrawCircle(middle.x+3*l2_u,middle.y+6*l2_u,18*l2_u,Fade(BLACK,0.18));
        int frame = (int)(t*8)%4;
        if (l2_crab_angry[i]>0) frame = 4;
        Rectangle source = {frame*128,0,128,128};
        Rectangle dest = {middle.x,middle.y-2*l2_u,64*l2_u,64*l2_u};
        Vector2 origin = {32*l2_u,32*l2_u};
        DrawTexturePro(l2_crab_texture,source,dest,origin,0,WHITE);
    }

    //beach balls, they get bigger for a moment when hit
    for (int i=0; i<4; i++)
    {
        Vector2 b = l2_beach_ball[i];
        float r = l2_beach_ball_radius;
        if (l2_beach_ball_hit[i]>0) r = r*(1 + l2_beach_ball_hit[i]);
        DrawCircle(b.x+3*l2_u,b.y+5*l2_u,r,Fade(BLACK,0.22));
        Color slice[6] = {l2_coral_red,RAYWHITE,l2_sea_deep,l2_sun_yellow,RAYWHITE,LIME};
        for (int j=0; j<6; j++)
        {
            DrawCircleSector(b,r,t*40+j*60,t*40+j*60+60,8,slice[j]);
        }
        DrawCircle(b.x,b.y,5*l2_u,RAYWHITE);
        DrawCircle(b.x-8*l2_u,b.y-9*l2_u,5*l2_u,Fade(WHITE,0.45));
        DrawCircleLines(b.x,b.y,r,Fade(BLACK,0.35));
    }

    //l2_palm tree shadows (the leaves are drawn after the l2_ball)
    int palm_frame = (int)(t/0.8)%2;
    for (int i=0; i<4; i++)
    {
        Rectangle source = {palm_frame*384,0,384,384};
        Rectangle dest = {l2_palm[i].x+14*l2_u,l2_palm[i].y+18*l2_u,192*l2_u,192*l2_u};
        Vector2 origin = {96*l2_u,96*l2_u};
        DrawTexturePro(l2_palm_texture,source,dest,origin,0,Fade(BLACK,0.22));
    }
}

void l2_draw_ball_and_pot()
{
    //l2_pot with striped ring
    for (int i=0; i<8; i++)
    {
        Color colour = WHITE;
        if (i%2==1) colour = l2_coral_red;
        DrawRing(l2_pot,l2_radius_pot+6*l2_u,l2_radius_pot+12*l2_u,i*45,i*45+45,6,colour);
    }
    DrawCircle(l2_pot.x,l2_pot.y,l2_radius_pot+6*l2_u,l2_sand_light);
    DrawRing(l2_pot,l2_radius_pot+1*l2_u,l2_radius_pot+5*l2_u,0,360,24,l2_wood);
    DrawCircle(l2_pot.x,l2_pot.y,l2_radius_pot,BLACK);

    //flag
    Vector2 pole_top = {l2_pot.x,l2_pot.y-60*l2_u};
    DrawLineEx(l2_pot,pole_top,3*l2_u,l2_sand_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(l2_animation_time*6 - i*0.8)*2.5*l2_u;
        DrawRectangle(l2_pot.x+1*l2_u+i*6*l2_u,pole_top.y+wave,6*l2_u,18*l2_u,l2_coral_red);
    }

    //l2_ball
    DrawCircle(l2_ball.x+3*l2_u,l2_ball.y+4*l2_u,l2_radius_ball,Fade(BLACK,0.45));
    DrawCircle(l2_ball.x,l2_ball.y,l2_radius_ball,GetColor(0xEDEDEDFF));
    DrawCircleLines(l2_ball.x,l2_ball.y,l2_radius_ball,GRAY);
    DrawCircle(l2_ball.x-2*l2_u,l2_ball.y-2*l2_u,2*l2_u,WHITE);
}


//l2_palm leaves go over the l2_ball, so it hides under them (the aim line still shows)
void l2_draw_palm_leaves()
{
    int palm_frame = (int)(l2_animation_time/0.8)%2;
    for (int i=0; i<4; i++)
    {
        Rectangle source = {palm_frame*384,0,384,384};
        Rectangle dest = {l2_palm[i].x,l2_palm[i].y,192*l2_u,192*l2_u};
        Vector2 origin = {96*l2_u,96*l2_u};
        DrawTexturePro(l2_palm_texture,source,dest,origin,0,WHITE);
    }

    //aim line
    if (l2_aiming==1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        DrawLineEx(l2_ball,mouse,4*l2_u,Fade(WHITE,0.75));
        DrawCircle(mouse.x,mouse.y,5*l2_u,Fade(WHITE,0.75));
    }
}


void l2_draw_hud()
{
    //dark edges
    DrawRectangleGradientH(0,l2_hud_height,90*l2_u,l2_height-l2_hud_height,Fade(BLACK,0.25),BLANK);
    DrawRectangleGradientH(l2_width-90*l2_u,l2_hud_height,90*l2_u,l2_height-l2_hud_height,BLANK,Fade(BLACK,0.25));
    DrawRectangleGradientV(0,l2_height-70*l2_u,l2_width,70*l2_u,BLANK,Fade(BLACK,0.25));

    //wooden bar
    DrawRectangleGradientV(0,0,l2_width,l2_hud_height,l2_wood_light,l2_wood);
    DrawRectangle(0,l2_hud_height-4*l2_u,l2_width,4*l2_u,l2_wood_dark);
    int rivets = l2_width/(40*l2_u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*l2_u+i*40*l2_u,8*l2_u,3*l2_u,l2_wood_dark);
        DrawCircle(20*l2_u+i*40*l2_u,l2_hud_height-12*l2_u,3*l2_u,l2_wood_dark);
    }

    //text
    DrawText("LEVEL 2 - SHORELINE",32*l2_u,20*l2_u,36*l2_u,BLACK);
    DrawText("LEVEL 2 - SHORELINE",30*l2_u,18*l2_u,36*l2_u,l2_sun_yellow);
    DrawText(TextFormat("STROKES %d / %d",l2_stroke,l2_stroke_limit),l2_width/2-298*l2_u,21*l2_u,32*l2_u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",l2_stroke,l2_stroke_limit),l2_width/2-300*l2_u,19*l2_u,32*l2_u,WHITE);
    for (int i=0; i<l2_stroke_limit; i++)
    {
        Color pip = l2_wood_dark;
        if (i<l2_stroke) pip = l2_coral_red;
        DrawCircle(l2_width/2+10*l2_u+i*22*l2_u,35*l2_u,7*l2_u,pip);
        DrawCircleLines(l2_width/2+10*l2_u+i*22*l2_u,35*l2_u,7*l2_u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC menu",26*l2_u);
    DrawText("R restart   ESC menu",l2_width-help_width-28*l2_u,24*l2_u,26*l2_u,BLACK);
    DrawText("R restart   ESC menu",l2_width-help_width-30*l2_u,22*l2_u,26*l2_u,WHITE);

    //splash message
    if (l2_message_timer>0)
    {
        DrawText("SPLASH!",l2_width/2-MeasureText("SPLASH!",90*l2_u)/2+4*l2_u,l2_height/2-41*l2_u,90*l2_u,Fade(l2_sea_deep,l2_message_timer));
        DrawText("SPLASH!",l2_width/2-MeasureText("SPLASH!",90*l2_u)/2,l2_height/2-45*l2_u,90*l2_u,Fade(WHITE,l2_message_timer));
    }

    //level clear or failed
    if (l2_game_state!=0)
    {
        DrawRectangle(0,0,l2_width,l2_height,Fade(BLACK,0.6));
        Rectangle panel = {l2_width/2-340*l2_u,l2_height/2-170*l2_u,680*l2_u,340*l2_u};
        DrawRectangleRec(panel,l2_wood_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*l2_u};
        DrawRectangleRec(panel_band,l2_sun_yellow);
        DrawRectangleLinesEx(panel,6*l2_u,l2_sand_light);
        if (l2_game_state==1)
        {
            DrawText("LEVEL CLEAR!",l2_width/2-MeasureText("LEVEL CLEAR!",80*l2_u)/2,panel.y+60*l2_u,80*l2_u,l2_sun_yellow);
        }
        else
        {
            DrawText("FAILED",l2_width/2-MeasureText("FAILED",80*l2_u)/2,panel.y+60*l2_u,80*l2_u,l2_coral_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",l2_stroke,l2_stroke_limit),40*l2_u);
        DrawText(TextFormat("Strokes: %d / %d",l2_stroke,l2_stroke_limit),l2_width/2-strokes_width/2,panel.y+170*l2_u,40*l2_u,WHITE);
        DrawText("R play again   ESC menu",l2_width/2-MeasureText("R play again   ESC menu",30*l2_u)/2,panel.y+250*l2_u,30*l2_u,l2_sand_light);
    }
}


//level 2: keep the obstacles moving with no ball (intro and menu)
void l2_background_step(float dt)
{
    l2_animation_time = l2_animation_time + dt;
    l2_update_obstacles(dt);
}


//level 2: draw everything except the scoreboard (intro and menu)
void l2_draw_scene()
{

    l2_draw_ground();
    l2_draw_obstacles();
    l2_draw_ball_and_pot();
    l2_draw_palm_leaves();

}


//level 2: set the screen size, load pictures, start the level
void l2_start(int screen_width, int screen_height)
{
    l2_width = screen_width;
    l2_height = screen_height;

    //pictures (made at 2x size, so they stay sharp)
    l2_sand_texture = LoadTexture("assets/beach/beach_sand_tile.png");
    l2_water_texture = LoadTexture("assets/beach/beach_water_tile.png");
    l2_foam_texture = LoadTexture("assets/beach/beach_foam_strip.png");
    l2_crab_texture = LoadTexture("assets/beach/beach_crab.png");
    l2_palm_texture = LoadTexture("assets/beach/beach_palm.png");
    l2_splash_texture = LoadTexture("assets/beach/beach_splash.png");
    SetTextureWrap(l2_sand_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(l2_foam_texture,TEXTURE_WRAP_REPEAT);
    SetTextureFilter(l2_sand_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l2_water_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l2_foam_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l2_crab_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l2_palm_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l2_splash_texture,TEXTURE_FILTER_BILINEAR);

    //every size is N*l2_u, so it looks the same on any screen
    l2_u = l2_height/1080.0;
    if (l2_width/1920.0 < l2_u) l2_u = l2_width/1920.0;
    l2_reset_level();

}


//level 2: one frame of input and movement
void l2_update()
{
    float dt = GetFrameTime();
    if (dt>1.0/30) dt = 1.0/30;
    l2_animation_time = l2_animation_time + dt;
    if (l2_message_timer>0) l2_message_timer = l2_message_timer - dt;

    //restart
    if (IsKeyPressed(KEY_R)) l2_reset_level();

    //shooting (the click has to start while the l2_ball is still)
    int ball_stopped = 0;
    if (l2_speed.x==0 && l2_speed.y==0) ball_stopped = 1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && l2_game_state==0) l2_aiming = 1;
    if (l2_game_state!=0) l2_aiming = 0;
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && l2_aiming==1)
    {
        l2_aiming = 0;
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        Vector2 drag = Vector2Subtract(l2_ball,mouse);
        if (ball_stopped==1 && l2_stroke<l2_stroke_limit && Vector2Length(drag)>=2*l2_radius_ball)
        {
            l2_last_shot_position = l2_ball;
            l2_speed = Vector2ClampValue(Vector2Scale(drag,3.125),0,l2_max_speed*l2_u);
            l2_stroke++;
        }
    }

    //moving everything in 4 small steps, so nothing jumps through anything
    for (int i=0; i<4; i++)
    {
        l2_update_obstacles(dt/4*obstacle_speed);
        if (l2_game_state==0) l2_update_ball(dt/4);
    }

    //out of strokes
    if (l2_game_state==0 && l2_stroke>=l2_stroke_limit && l2_speed.x==0 && l2_speed.y==0) l2_game_state = 2;


}


//level 2: one frame of drawing
void l2_draw()
{

    l2_draw_ground();
    l2_draw_obstacles();
    l2_draw_ball_and_pot();
    l2_draw_palm_leaves();
    draw_straight_preview(l2_ball,l2_aiming,l2_radius_ball,l2_max_speed*l2_u,l2_u);
    l2_draw_hud();

}


void l2_unload()
{
    UnloadTexture(l2_sand_texture);
    UnloadTexture(l2_water_texture);
    UnloadTexture(l2_foam_texture);
    UnloadTexture(l2_crab_texture);
    UnloadTexture(l2_palm_texture);
    UnloadTexture(l2_splash_texture);
}


//==================== LEVEL 3 ====================

//full global
int l3_width = 1920;
int l3_height = 1080;
float l3_u = 1;
int l3_stroke_base = 20;
int l3_stroke_limit = 20;
#define l3_max_speed 650

//colours
Color l3_brick = {139,58,43,255};
Color l3_brick_dark = {110,44,32,255};
Color l3_mortar = {58,51,48,255};
Color l3_steel = {90,95,102,255};
Color l3_steel_light = {138,144,153,255};
Color l3_steel_dark = {46,49,54,255};
Color l3_floor_colour = {74,74,72,255};
Color l3_hazard_yellow = {232,185,35,255};
Color l3_rust = {181,84,28,255};
Color l3_rust_dark = {107,46,14,255};
Color l3_molten_orange = {255,106,0,255};
Color l3_molten_yellow = {255,208,0,255};
Color l3_molten_dark = {139,26,0,255};
Color l3_laser_red = {255,32,48,255};
Color l3_magnet_red = {192,40,45,255};
Color l3_space_dark = {6,8,22,255};
Color l3_neon_cyan = {80,230,255,255};
Color l3_neon_purple = {190,90,255,255};
Color l3_neon_orange = {255,150,40,255};
Color l3_neon_green = {110,255,120,255};
Color l3_panel_blue = {50,100,210,255};

//pictures
Texture2D l3_asteroid_texture;
Texture2D l3_ufo_texture;

//l3_ball and l3_pot
Vector2 l3_ball;
Vector2 l3_speed;
float l3_radius_ball;
Vector2 l3_pot;
float l3_radius_pot;
Vector2 l3_start_position;
Vector2 l3_last_shot_position;
int l3_stroke = 0;
int l3_game_state = 0;
int l3_aiming = 0;
float l3_message_timer = 0;
int l3_message_type = 0;
float l3_animation_time = 0;
float l3_hud_height;
Vector2 l3_no_speed = {0,0};

//walkways in path order: standing on one is safe, falling off is lost in space
Rectangle l3_platform[11];

//l3_vacuum strips (almost no friction) and solar wind (the old rip current, pushes towards the void)
Rectangle l3_vacuum[2];
Rectangle l3_solar_wind;
Vector2 l3_solar_wind_push;

//gravity: planets (solid core) and the black hole, it only bends a moving l3_ball
Vector2 l3_planet[2];
float l3_planet_core[2];
float l3_planet_field;
float l3_planet_pull;
Vector2 l3_black_hole;
float l3_black_hole_field;
float l3_black_hole_pull;

//wormholes, one way from in to out, the l3_ball leaves the way the exit faces
Vector2 l3_wormhole_in[3];
Vector2 l3_wormhole_out[3];
float l3_wormhole_angle[3];
float l3_wormhole_radius;
float l3_wormhole_cooldown = 0;

//asteroids drifting back and forth between two points
Vector2 l3_asteroid[4];
Vector2 l3_asteroid_start[4];
Vector2 l3_asteroid_end[4];
Vector2 l3_asteroid_velocity[4];
float l3_asteroid_t[4];
float l3_asteroid_time[4];
float l3_asteroid_radius[4];
int l3_asteroid_direction[4];

//laser gates (from level 1): off 1.2s, warning 0.4s, on 1.4s
Rectangle l3_laser_gate[2];
float l3_laser_clock[2];

//spinning satellite (the level 1 l3_fan)
Vector2 l3_fan;
float l3_fan_angle = 0;
float l3_fan_spin = 90;
float l3_fan_blade_length;
float l3_fan_blade_thickness;
float l3_fan_hub_radius;

//energy bumpers
Vector2 l3_bumper[4];
float l3_bumper_radius;
float l3_bumper_hit[4];

//lost in space effect
Vector2 l3_lost_position;
float l3_lost_timer = 0;

//fly-through events: 0 nothing, 1 l3_comet, 2 l3_meteor shower, 3 l3_ufo (the first 1.5s of each is a warning)
int l3_event_type = 0;
float l3_event_timer = 0;
float l3_next_event = 10;
int l3_shower_done = 0;
int l3_ufo_done = 0;
Vector2 l3_event_start;
Vector2 l3_event_direction;
float l3_event_speed;
Vector2 l3_comet;
Vector2 l3_meteor[5];
Vector2 l3_ufo;
float l3_beam_timer = 0;
int l3_beam_used = 0;
int l3_abducted = 0;
float l3_carry_timer = 0;
int l3_abducted_section = 0;

//stars
Vector2 l3_star[150];
float l3_star_size[150];


//a rectangle in design units (a 1920 x 1080 screen), made to fit the real screen
Rectangle l3_make_rect(float x, float y, float w, float h)
{
    Rectangle rec = {x*l3_u,y*l3_u,w*l3_u,h*l3_u};
    return rec;
}

//a point in design units
Vector2 l3_make_point(float x, float y)
{
    Vector2 point = {x*l3_u,y*l3_u};
    return point;
}


//stars at random places, 3 sizes (bigger ones drift faster)
void l3_make_stars()
{
    for (int i=0; i<150; i++)
    {
        l3_star[i].x = GetRandomValue(0,l3_width);
        l3_star[i].y = GetRandomValue(0,l3_height);
        l3_star_size[i] = GetRandomValue(1,3)*l3_u;
    }
}


void l3_reset_level()
{
    //sizes
    l3_hud_height = 70*l3_u;
    l3_radius_ball = 7*l3_u;
    l3_radius_pot = 11*l3_u;

    //walkways, spiralling in to the black hole
    l3_platform[0] = l3_make_rect(60,940,1800,100);    //outer ring bottom (start)
    l3_platform[1] = l3_make_rect(1760,110,100,930);   //outer ring right
    l3_platform[2] = l3_make_rect(60,110,1800,100);    //outer ring top
    l3_platform[3] = l3_make_rect(60,110,100,750);     //outer ring left
    l3_platform[4] = l3_make_rect(60,760,1620,100);    //inner ring bottom
    l3_platform[5] = l3_make_rect(1580,290,100,570);   //inner ring right
    l3_platform[6] = l3_make_rect(240,290,1440,100);   //inner ring top
    l3_platform[7] = l3_make_rect(240,290,100,390);    //inner ring left
    l3_platform[8] = l3_make_rect(240,470,560,210);    //centre left
    l3_platform[9] = l3_make_rect(800,470,320,60);     //narrow bridge past the black hole
    l3_platform[10] = l3_make_rect(1120,470,380,210);  //l3_pot l3_platform

    l3_vacuum[0] = l3_make_rect(500,940,350,100);
    l3_vacuum[1] = l3_make_rect(500,290,350,100);
    l3_solar_wind = l3_make_rect(60,400,100,180);
    l3_solar_wind_push = l3_make_point(-280,0);

    //gravity
    l3_planet[0] = l3_make_point(600,720);
    l3_planet[1] = l3_make_point(1450,250);
    l3_planet_core[0] = 28*l3_u;
    l3_planet_core[1] = 28*l3_u;
    l3_planet_field = 200*l3_u;
    l3_planet_pull = 700*l3_u;
    l3_black_hole = l3_make_point(960,640);
    l3_black_hole_field = 240*l3_u;
    l3_black_hole_pull = 500*l3_u;

    //wormholes: purple shortcut, cyan to the l3_pot, orange trap
    l3_wormhole_radius = 26*l3_u;
    l3_wormhole_in[0] = l3_make_point(1840,700);
    l3_wormhole_out[0] = l3_make_point(1300,340);
    l3_wormhole_angle[0] = 180;
    l3_wormhole_in[1] = l3_make_point(700,600);
    l3_wormhole_out[1] = l3_make_point(1170,600);
    l3_wormhole_angle[1] = 0;
    l3_wormhole_in[2] = l3_make_point(1300,810);
    l3_wormhole_out[2] = l3_make_point(960,250);
    l3_wormhole_angle[2] = 90;
    l3_wormhole_cooldown = 0;

    //asteroids
    l3_asteroid_start[0] = l3_make_point(700,50);
    l3_asteroid_end[0] = l3_make_point(700,250);
    l3_asteroid_radius[0] = 30*l3_u;
    l3_asteroid_time[0] = 2.4;
    l3_asteroid_start[1] = l3_make_point(1250,250);
    l3_asteroid_end[1] = l3_make_point(1250,50);
    l3_asteroid_radius[1] = 26*l3_u;
    l3_asteroid_time[1] = 2.0;
    l3_asteroid_start[2] = l3_make_point(1000,250);
    l3_asteroid_end[2] = l3_make_point(1000,430);
    l3_asteroid_radius[2] = 22*l3_u;
    l3_asteroid_time[2] = 1.8;
    l3_asteroid_start[3] = l3_make_point(420,430);
    l3_asteroid_end[3] = l3_make_point(420,720);
    l3_asteroid_radius[3] = 30*l3_u;
    l3_asteroid_time[3] = 3.0;
    for (int i=0; i<4; i++)
    {
        l3_asteroid_t[i] = 0;
        l3_asteroid_direction[i] = 1;
        l3_asteroid[i] = l3_asteroid_start[i];
        l3_asteroid_velocity[i] = l3_no_speed;
        l3_bumper_hit[i] = 0;
    }

    //laser gates across the two right hand walkways
    l3_laser_gate[0] = l3_make_rect(1760,450,100,8);
    l3_laser_gate[1] = l3_make_rect(1580,600,100,8);
    l3_laser_clock[0] = 0;
    l3_laser_clock[1] = 1.5;

    //satellite
    l3_fan = l3_make_point(1200,990);
    l3_fan_blade_length = 150*l3_u;
    l3_fan_blade_thickness = 12*l3_u;
    l3_fan_hub_radius = 16*l3_u;
    l3_fan_angle = 0;

    //energy bumpers
    l3_bumper_radius = 20*l3_u;
    l3_bumper[0] = l3_make_point(110,700);
    l3_bumper[1] = l3_make_point(1630,420);
    l3_bumper[2] = l3_make_point(1330,520);
    l3_bumper[3] = l3_make_point(1330,640);

    //fly-through events
    l3_event_type = 0;
    l3_next_event = 10;
    l3_shower_done = 0;
    l3_ufo_done = 0;
    l3_abducted = 0;
    l3_beam_timer = 0;
    l3_beam_used = 0;

    //l3_ball and l3_pot
    l3_start_position = l3_make_point(140,990);
    l3_ball = l3_start_position;
    l3_last_shot_position = l3_start_position;
    l3_speed.x = 0;
    l3_speed.y= 0;
    l3_pot = l3_make_point(1400,575);
    l3_stroke = 0;
    l3_game_state = 0;
    l3_aiming = 0;
    l3_message_timer = 0;
    l3_lost_timer = 0;
}


//bounce off a round thing: bounce is 1 for normal and more for bouncy, circle_speed is how fast it moves
int l3_bounce_off_circle(Vector2 center, float radius, float bounce, Vector2 circle_speed)
{
    Vector2 normal = Vector2Subtract(l3_ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + l3_radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        l3_ball = Vector2Add(center,Vector2Scale(normal,radius + l3_radius_ball));
        Vector2 relative_speed = Vector2Subtract(l3_speed,circle_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Scale(Vector2Reflect(relative_speed,normal),bounce);
            l3_speed = Vector2ClampValue(Vector2Add(relative_speed,circle_speed),0,l3_max_speed*l3_u);
            return 1;
        }
    }
    return 0;
}


//bounce off a turned rectangle (diamond, l3_fan blades), spin is in degrees per second
void l3_bounce_off_rotated_rectangle(Vector2 center, float rec_width, float rec_height, float angle, float spin)
{
    //look at the l3_ball as if the rectangle was not turned
    Vector2 local_ball = Vector2Rotate(Vector2Subtract(l3_ball,center),-angle*DEG2RAD);
    Rectangle rec = {-rec_width/2,-rec_height/2,rec_width,rec_height};
    if (CheckCollisionCircleRec(local_ball,l3_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(local_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(local_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(local_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = local_ball.x - rec.x;
            float right = rec.x + rec.width - local_ball.x;
            float top = local_ball.y - rec.y;
            float bottom = rec.y + rec.height - local_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }

        //turn the point and normal back to the screen
        normal = Vector2Rotate(Vector2Normalize(normal),angle*DEG2RAD);
        collision_point = Vector2Add(center,Vector2Rotate(collision_point,angle*DEG2RAD));
        l3_ball = Vector2Add(collision_point,Vector2Scale(normal,l3_radius_ball));

        //l3_speed of the blade at the point it touches the l3_ball
        Vector2 arm = Vector2Subtract(collision_point,center);
        Vector2 rec_speed = {-arm.y*spin*DEG2RAD, arm.x*spin*DEG2RAD};

        Vector2 relative_speed = Vector2Subtract(l3_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l3_speed = Vector2ClampValue(Vector2Add(relative_speed,rec_speed),0,l3_max_speed*l3_u);
        }
    }
}


//pull towards a centre, stronger the closer the l3_ball is
Vector2 l3_gravity_pull(Vector2 center, float field, float strength, float dt)
{
    Vector2 pull = {0,0};
    Vector2 to_center = Vector2Subtract(center,l3_ball);
    float distance = Vector2Length(to_center);
    if (distance<field && distance>1*l3_u)
    {
        float closeness = 1 - distance/field;
        pull = Vector2Scale(Vector2Normalize(to_center),strength*closeness*closeness*dt);
    }
    return pull;
}


//is this point on a walkway
int l3_on_safe_ground(Vector2 point)
{
    for (int i=0; i<11; i++)
    {
        if (CheckCollisionPointRec(point,l3_platform[i])) return 1;
    }
    return 0;
}


//a random point on a walkway, 20 units inside its edges
Vector2 l3_random_point_on(Rectangle rec)
{
    Vector2 point;
    point.x = rec.x + 20*l3_u + GetRandomValue(0,1000)/1000.0*(rec.width-40*l3_u);
    point.y = rec.y + 20*l3_u + GetRandomValue(0,1000)/1000.0*(rec.height-40*l3_u);
    return point;
}


//which walkway a point is on (the later one if two overlap)
int l3_section_of(Vector2 point)
{
    int section = 0;
    for (int i=0; i<11; i++)
    {
        if (CheckCollisionPointRec(point,l3_platform[i])) section = i;
    }
    return section;
}


//a spot where a dropped l3_ball won't land in trouble
int l3_spot_is_clear(Vector2 spot)
{
    if (l3_on_safe_ground(spot)==0) return 0;
    for (int i=0; i<2; i++)
    {
        if (Vector2Distance(spot,l3_planet[i]) < l3_planet_core[i]+30*l3_u) return 0;
        if (CheckCollisionCircleRec(spot,30*l3_u,l3_laser_gate[i])) return 0;
    }
    for (int i=0; i<4; i++)
    {
        if (Vector2Distance(spot,l3_bumper[i]) < l3_bumper_radius+30*l3_u) return 0;
        if (Vector2Distance(spot,l3_asteroid[i]) < l3_asteroid_radius[i]+30*l3_u) return 0;
    }
    for (int i=0; i<3; i++)
    {
        if (Vector2Distance(spot,l3_wormhole_in[i]) < l3_wormhole_radius+30*l3_u) return 0;
    }
    if (Vector2Distance(spot,l3_fan) < l3_fan_blade_length/2+30*l3_u) return 0;
    if (CheckCollisionPointRec(spot,l3_solar_wind)) return 0;
    if (Vector2Distance(spot,l3_pot) < 40*l3_u) return 0;
    return 1;
}


void l3_update_obstacles(float dt)
{
    //asteroids drifting back and forth
    for (int i=0; i<4; i++)
    {
        l3_asteroid_t[i] = l3_asteroid_t[i] + l3_asteroid_direction[i]*dt/l3_asteroid_time[i];
        if (l3_asteroid_t[i]>=1)
        {
            l3_asteroid_t[i] = 1;
            l3_asteroid_direction[i] = -1;
        }
        if (l3_asteroid_t[i]<=0)
        {
            l3_asteroid_t[i] = 0;
            l3_asteroid_direction[i] = 1;
        }
        l3_asteroid[i] = Vector2Lerp(l3_asteroid_start[i],l3_asteroid_end[i],l3_asteroid_t[i]);
        l3_asteroid_velocity[i] = Vector2Scale(Vector2Subtract(l3_asteroid_end[i],l3_asteroid_start[i]),l3_asteroid_direction[i]/l3_asteroid_time[i]);
        if (l3_bumper_hit[i]>0) l3_bumper_hit[i] = l3_bumper_hit[i] - dt;
    }

    //l3_fan spinning
    l3_fan_angle = l3_fan_angle + l3_fan_spin*dt;
    if (l3_fan_angle>=360) l3_fan_angle = l3_fan_angle - 360;

    //laser clocks
    for (int i=0; i<2; i++)
    {
        l3_laser_clock[i] = l3_laser_clock[i] + dt;
        if (l3_laser_clock[i]>=3.0) l3_laser_clock[i] = l3_laser_clock[i] - 3.0;
    }

    if (l3_wormhole_cooldown>0) l3_wormhole_cooldown = l3_wormhole_cooldown - dt;
    if (l3_lost_timer>0) l3_lost_timer = l3_lost_timer - dt;
}


//after falling off or getting zapped, the l3_ball goes back to where it was shot from
void l3_send_ball_back()
{
    l3_ball = l3_last_shot_position;

    //not right in an l3_asteroid's path, or it gets knocked off again and again
    for (int i=0; i<4; i++)
    {
        Vector2 path = Vector2Subtract(l3_asteroid_end[i],l3_asteroid_start[i]);
        float along = Vector2DotProduct(Vector2Subtract(l3_ball,l3_asteroid_start[i]),path)/Vector2DotProduct(path,path);
        along = Clamp(along,0,1);
        Vector2 closest = Vector2Add(l3_asteroid_start[i],Vector2Scale(path,along));
        Vector2 away = Vector2Subtract(l3_ball,closest);
        float gap = l3_asteroid_radius[i] + l3_radius_ball + 2*l3_u;
        if (Vector2Length(away)<gap)
        {
            Vector2 side = {-path.y,path.x};
            side = Vector2Normalize(side);
            if (Vector2DotProduct(away,side)<0) side = Vector2Negate(side);
            l3_ball = Vector2Add(closest,Vector2Scale(side,gap));
        }
    }

    //not where the satellite blades sweep, or it gets knocked off again and again (the level 1 piston rule)
    float sweep = l3_fan_blade_length/2 + l3_radius_ball + 2*l3_u;
    if (Vector2Distance(l3_ball,l3_fan)<sweep)
    {
        if (l3_ball.x<l3_fan.x) l3_ball.x = l3_fan.x - sweep;
        else l3_ball.x = l3_fan.x + sweep;
    }

    //not inside a laser gate (level 1 rule), not off a walkway
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionCircleRec(l3_ball,l3_radius_ball,l3_laser_gate[i])) l3_ball = l3_start_position;
    }
    if (l3_on_safe_ground(l3_ball)==0) l3_ball = l3_start_position;

    l3_speed.x = 0;
    l3_speed.y = 0;
}


//a l3_comet or l3_meteor hits the l3_ball and knocks it along
void l3_knock_ball(Vector2 center, float radius)
{
    if (l3_game_state!=0 || l3_abducted==1) return;
    Vector2 normal = Vector2Subtract(l3_ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + l3_radius_ball)
    {
        if (distance==0) normal = l3_event_direction;
        normal = Vector2Normalize(normal);
        l3_ball = Vector2Add(center,Vector2Scale(normal,radius + l3_radius_ball));
        l3_speed = Vector2ClampValue(Vector2Add(Vector2Scale(l3_event_direction,400*l3_u),Vector2Scale(normal,150*l3_u)),0,l3_max_speed*l3_u);
    }
}


//the l3_ufo lets go: a random clear spot on the walkway before or after the one the l3_ball was on
void l3_drop_ball()
{
    int section = l3_abducted_section - 1;
    if (GetRandomValue(0,1)==1) section = l3_abducted_section + 1;
    if (section<0) section = 1;
    if (section>10) section = 9;
    if (section==9) section = 8 + 2*GetRandomValue(0,1);    //never the narrow bridge
    l3_ball = l3_last_shot_position;
    for (int tries=0; tries<30; tries++)
    {
        Vector2 spot = l3_random_point_on(l3_platform[section]);
        if (l3_spot_is_clear(spot))
        {
            l3_ball = spot;
            break;
        }
    }
    l3_speed.x = 0;
    l3_speed.y = 0;
    l3_abducted = 0;
    l3_beam_timer = 0;
}


//pick a new fly-through, the l3_meteor shower and the l3_ufo only come once
void l3_start_event()
{
    l3_event_type = GetRandomValue(1,3);
    if (l3_event_type==2 && l3_shower_done==1) l3_event_type = 1;
    if (l3_event_type==3 && l3_ufo_done==1) l3_event_type = 1;
    l3_event_timer = 0;

    if (l3_event_type==3)
    {
        //the l3_ufo flies straight across, over one of the long walkways
        l3_ufo_done = 1;
        l3_beam_timer = 0;
        l3_beam_used = 0;
        int lane = GetRandomValue(0,3)*2;
        l3_event_start.y = l3_platform[lane].y + l3_platform[lane].height/2;
        l3_event_start.x = -120*l3_u;
        l3_event_direction.x = 1;
        l3_event_direction.y = 0;
        if (GetRandomValue(0,1)==1)
        {
            l3_event_start.x = l3_width + 120*l3_u;
            l3_event_direction.x = -1;
        }
        l3_event_speed = 170*l3_u;
        l3_ufo = l3_event_start;
        return;
    }

    //l3_comet or l3_meteor shower: aimed at a random walkway spot from a random side, starting just off the screen
    if (l3_event_type==2) l3_shower_done = 1;
    Vector2 target = l3_random_point_on(l3_platform[GetRandomValue(0,10)]);
    float angle = GetRandomValue(0,359)*DEG2RAD;
    l3_event_direction.x = cos(angle);
    l3_event_direction.y = sin(angle);
    float back_x = 10000;
    float back_y = 10000;
    if (l3_event_direction.x>0.01) back_x = target.x/l3_event_direction.x;
    if (l3_event_direction.x<-0.01) back_x = (l3_width-target.x)/(-l3_event_direction.x);
    if (l3_event_direction.y>0.01) back_y = target.y/l3_event_direction.y;
    if (l3_event_direction.y<-0.01) back_y = (l3_height-target.y)/(-l3_event_direction.y);
    float back = back_x;
    if (back_y<back) back = back_y;
    l3_event_start = Vector2Subtract(target,Vector2Scale(l3_event_direction,back+60*l3_u));
    l3_event_speed = 900*l3_u;
    if (l3_event_type==2) l3_event_speed = 650*l3_u;
    l3_comet = l3_event_start;
}


void l3_update_events(float dt)
{
    //waiting for the next one
    if (l3_event_type==0)
    {
        l3_next_event = l3_next_event - dt;
        if (l3_next_event<=0) l3_start_event();
        return;
    }

    l3_event_timer = l3_event_timer + dt;
    float flying = l3_event_timer - 1.5;
    if (flying<0) return;

    //l3_comet
    if (l3_event_type==1)
    {
        l3_comet = Vector2Add(l3_event_start,Vector2Scale(l3_event_direction,l3_event_speed*flying));
        l3_knock_ball(l3_comet,12*l3_u);
    }

    //l3_meteor shower, 5 rocks side by side, a quarter of a second apart
    if (l3_event_type==2)
    {
        for (int i=0; i<5; i++)
        {
            Vector2 side = {-l3_event_direction.y*(i-2)*45*l3_u, l3_event_direction.x*(i-2)*45*l3_u};
            float travelled = l3_event_speed*(flying - i*0.25);
            l3_meteor[i] = Vector2Add(Vector2Add(l3_event_start,side),Vector2Scale(l3_event_direction,travelled));
            if (travelled>0) l3_knock_ball(l3_meteor[i],10*l3_u);
        }
    }

    //l3_ufo, beams up a l3_ball that sits still under it
    if (l3_event_type==3)
    {
        l3_ufo = Vector2Add(l3_event_start,Vector2Scale(l3_event_direction,l3_event_speed*flying));
        if (l3_abducted==0 && l3_beam_used==0 && l3_game_state==0)
        {
            if (l3_speed.x==0 && l3_speed.y==0 && Vector2Distance(l3_ball,l3_ufo)<80*l3_u)
            {
                l3_beam_timer = l3_beam_timer + dt;
                if (l3_beam_timer>=0.8)
                {
                    l3_abducted = 1;
                    l3_beam_used = 1;
                    l3_carry_timer = 1;
                    l3_aiming = 0;
                    l3_abducted_section = l3_section_of(l3_ball);
                    l3_message_type = 3;
                    l3_message_timer = 1;
                }
            }
            else l3_beam_timer = 0;
        }
        if (l3_abducted==1)
        {
            l3_ball = l3_ufo;
            l3_speed.x = 0;
            l3_speed.y = 0;
            l3_carry_timer = l3_carry_timer - dt;
            if (l3_carry_timer<=0) l3_drop_ball();
        }
    }

    //far enough to be off the screen: over, wait for the next one
    float travelled = l3_event_speed*flying;
    float needed = l3_width + l3_height + 400*l3_u;
    if (l3_event_type==2) travelled = l3_event_speed*(flying - 1);
    if (l3_event_type==3) needed = l3_width + 240*l3_u;
    if (travelled>needed)
    {
        if (l3_abducted==1) l3_drop_ball();
        l3_event_type = 0;
        l3_next_event = GetRandomValue(12,22);
    }
}


void l3_update_ball(float dt)
{
    int pushed = 0;

    //solar wind (the old rip current)
    if (CheckCollisionPointRec(l3_ball,l3_solar_wind))
    {
        l3_speed = Vector2Add(l3_speed,Vector2Scale(l3_solar_wind_push,dt));
        pushed = 1;
    }

    //gravity from the planets and the black hole, only bends a moving l3_ball
    if (l3_speed.x!=0 || l3_speed.y!=0)
    {
        for (int i=0; i<2; i++)
        {
            l3_speed = Vector2Add(l3_speed,l3_gravity_pull(l3_planet[i],l3_planet_field,l3_planet_pull,dt));
        }
        l3_speed = Vector2Add(l3_speed,l3_gravity_pull(l3_black_hole,l3_black_hole_field,l3_black_hole_pull,dt));
    }

    //moving
    l3_ball = Vector2Add(l3_ball,Vector2Scale(l3_speed,dt));

    //proportional deceleration (almost no friction in a l3_vacuum strip)
    float friction = 110*l3_u;
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(l3_ball,l3_vacuum[i])) friction = 25*l3_u;
    }
    float ball_speed = Vector2Length(l3_speed);
    if (ball_speed>0)
    {
        float new_speed = ball_speed - friction*dt;
        if (new_speed<0) new_speed = 0;
        l3_speed = Vector2Scale(l3_speed,new_speed/ball_speed);
    }
    if (pushed==0 && Vector2Length(l3_speed)<6*l3_u)
    {
        l3_speed.x = 0;
        l3_speed.y= 0;
    }

    //wormholes, one way, the l3_ball comes out the way the exit faces
    if (l3_wormhole_cooldown<=0)
    {
        for (int i=0; i<3; i++)
        {
            if (Vector2Distance(l3_ball,l3_wormhole_in[i])<l3_wormhole_radius)
            {
                Vector2 facing = {cos(l3_wormhole_angle[i]*DEG2RAD),sin(l3_wormhole_angle[i]*DEG2RAD)};
                float ball_speed = Vector2Length(l3_speed);
                if (ball_speed<80*l3_u) ball_speed = 80*l3_u;
                l3_ball = Vector2Add(l3_wormhole_out[i],Vector2Scale(facing,l3_wormhole_radius + l3_radius_ball + 2*l3_u));
                l3_speed = Vector2Scale(facing,ball_speed);
                l3_wormhole_cooldown = 0.5;
                break;
            }
        }
    }

    //lost in space (checked before the collisions, so if something is on the old spot it pushes the l3_ball out)
    if (l3_on_safe_ground(l3_ball)==0)
    {
        l3_lost_position = l3_ball;
        l3_lost_timer = 0.5;
        l3_send_ball_back();
        l3_message_type = 1;
        l3_message_timer = 1;
    }

    //laser gates (from level 1)
    for (int i=0; i<2; i++)
    {
        if (l3_laser_clock[i]>=1.6 && CheckCollisionCircleRec(l3_ball,l3_radius_ball,l3_laser_gate[i]))
        {
            l3_send_ball_back();
            l3_message_type = 2;
            l3_message_timer = 1;
        }
    }

    //l3_planet cores, energy bumpers and asteroids
    for (int i=0; i<2; i++)
    {
        l3_bounce_off_circle(l3_planet[i],l3_planet_core[i],1,l3_no_speed);
    }
    for (int i=0; i<4; i++)
    {
        if (l3_bounce_off_circle(l3_bumper[i],l3_bumper_radius,1.3,l3_no_speed)) l3_bumper_hit[i] = 0.15;
        l3_bounce_off_circle(l3_asteroid[i],l3_asteroid_radius[i],1,l3_asteroid_velocity[i]);
    }

    //spinning satellite (the level 1 l3_fan)
    l3_bounce_off_circle(l3_fan,l3_fan_hub_radius,1,l3_no_speed);
    l3_bounce_off_rotated_rectangle(l3_fan,l3_fan_blade_length,l3_fan_blade_thickness,l3_fan_angle,l3_fan_spin);
    l3_bounce_off_rotated_rectangle(l3_fan,l3_fan_blade_length,l3_fan_blade_thickness,l3_fan_angle+90,l3_fan_spin);

    //score
    if ((l3_ball.x>l3_pot.x-3*l3_radius_pot/4) && (l3_ball.x<l3_pot.x+3*l3_radius_pot/4) && (l3_ball.y>l3_pot.y-3*l3_radius_pot/4) && (l3_ball.y<l3_pot.y+3*l3_radius_pot/4))
    {
        l3_ball = l3_pot;
        l3_speed.x = 0;
        l3_speed.y= 0;
        l3_game_state = 1;
    }
}


//yellow and black bands, across the long side
void l3_draw_hazard_stripes(Rectangle rec)
{
    DrawRectangleRec(rec,l3_hazard_yellow);
    float band = 6*l3_u;
    if (rec.width>=rec.height)
    {
        int bands = rec.width/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x+i*band,rec.y,band,rec.height,BLACK);
        }
    }
    else
    {
        int bands = rec.height/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x,rec.y+i*band,rec.width,band,BLACK);
        }
    }
}

void l3_draw_space()
{
    float t = l3_animation_time;
    DrawRectangleGradientV(0,0,l3_width,l3_height,l3_space_dark,GetColor(0x140A2AFF));

    //nebula clouds
    DrawCircleGradient(l3_make_point(420,300),420*l3_u,Fade(l3_neon_purple,0.12),BLANK);
    DrawCircleGradient(l3_make_point(1500,820),480*l3_u,Fade(l3_panel_blue,0.14),BLANK);
    DrawCircleGradient(l3_make_point(1650,200),300*l3_u,Fade(l3_neon_cyan,0.07),BLANK);
    DrawCircleGradient(l3_make_point(900,950),350*l3_u,Fade(PINK,0.06),BLANK);

    //stars drifting slowly to the left (bigger ones faster), all twinkling
    for (int i=0; i<150; i++)
    {
        float x = fmod(l3_star[i].x - t*l3_star_size[i]*6 + l3_width*100,l3_width);
        float twinkle = 0.5 + 0.5*sin(t*2 + i);
        DrawCircle(x,l3_star[i].y,l3_star_size[i]*0.7,Fade(WHITE,0.3+0.6*twinkle));
    }

    //black hole: glow, spinning bright rings, dark middle
    DrawCircleGradient(l3_black_hole,l3_black_hole_field,Fade(l3_neon_purple,0.22),BLANK);
    for (int j=0; j<5; j++)
    {
        float ring = 34*l3_u + j*13*l3_u;
        float angle = t*(220 - j*35) + j*60;
        Color ring_colour = l3_neon_orange;
        if (j%2==1) ring_colour = l3_neon_purple;
        DrawRing(l3_black_hole,ring-2*l3_u,ring+2*l3_u,angle,angle+250,32,Fade(ring_colour,0.8-j*0.12));
    }
    DrawCircleGradient(l3_black_hole,40*l3_u,BLACK,Fade(BLACK,0));
    DrawCircle(l3_black_hole.x,l3_black_hole.y,26*l3_u,BLACK);
    DrawRing(l3_black_hole,26*l3_u,28*l3_u,0,360,48,Fade(WHITE,0.5));
}


void l3_draw_walkways()
{
    float t = l3_animation_time;

    //glow, shadow and edge lights first, then the walkways on top, so they only show on the outside
    for (int i=0; i<11; i++)
    {
        Rectangle p = l3_platform[i];
        Rectangle glow = {p.x-6*l3_u,p.y-6*l3_u,p.width+12*l3_u,p.height+12*l3_u};
        DrawRectangleRec(glow,Fade(l3_neon_cyan,0.18));
        Rectangle shadow = {p.x+8*l3_u,p.y+10*l3_u,p.width,p.height};
        DrawRectangleRec(shadow,Fade(BLACK,0.5));
        int lights_x = p.width/(60*l3_u);
        int lights_y = p.height/(60*l3_u);
        for (int j=0; j<=lights_x; j++)
        {
            float blink = 0.3 + 0.7*(sin(t*3 - j*0.7 - i)>0.6);
            DrawCircle(p.x+j*60*l3_u,p.y-4*l3_u,2.5*l3_u,Fade(l3_neon_cyan,blink));
            DrawCircle(p.x+j*60*l3_u,p.y+p.height+4*l3_u,2.5*l3_u,Fade(l3_neon_cyan,blink));
        }
        for (int j=0; j<=lights_y; j++)
        {
            float blink = 0.3 + 0.7*(sin(t*3 - j*0.7 - i)>0.6);
            DrawCircle(p.x-4*l3_u,p.y+j*60*l3_u,2.5*l3_u,Fade(l3_neon_cyan,blink));
            DrawCircle(p.x+p.width+4*l3_u,p.y+j*60*l3_u,2.5*l3_u,Fade(l3_neon_cyan,blink));
        }
    }

    //l3_steel walkways with panel lines lined up with the screen, so joins match
    for (int i=0; i<11; i++)
    {
        Rectangle p = l3_platform[i];
        DrawRectangleRec(p,l3_steel_dark);
        int first_column = p.x/(50*l3_u) + 1;
        int first_row = p.y/(50*l3_u) + 1;
        for (int j=first_column; j*50*l3_u<p.x+p.width; j++)
        {
            DrawLine(j*50*l3_u,p.y,j*50*l3_u,p.y+p.height,Fade(l3_steel,0.5));
        }
        for (int j=first_row; j*50*l3_u<p.y+p.height; j++)
        {
            DrawLine(p.x,j*50*l3_u,p.x+p.width,j*50*l3_u,Fade(l3_steel,0.5));
        }
    }

    //l3_vacuum strips: darker with little sparkles
    for (int i=0; i<2; i++)
    {
        Rectangle v = l3_vacuum[i];
        DrawRectangleRec(v,Fade(BLACK,0.45));
        for (int k=0; k<16; k++)
        {
            float x = v.x + fmod(k*67*l3_u + t*20*l3_u,v.width);
            float y = v.y + fmod(k*29*l3_u + 11*l3_u,v.height);
            DrawCircle(x,y,1.5*l3_u,Fade(l3_neon_cyan,0.3+0.3*sin(t*4+k)));
        }
        DrawRectangleLinesEx(v,2*l3_u,Fade(l3_neon_cyan,0.35));
    }

    //solar wind: yellow streaks blowing the way it pushes
    DrawRectangleRec(l3_solar_wind,Fade(l3_neon_orange,0.12));
    int streaks = l3_solar_wind.height/(18*l3_u);
    for (int j=0; j<streaks; j++)
    {
        float x = l3_solar_wind.x + l3_solar_wind.width - fmod(t*140*l3_u + j*31*l3_u,l3_solar_wind.width);
        Vector2 a = {x, l3_solar_wind.y + 9*l3_u + j*18*l3_u};
        Vector2 b = {x + 16*l3_u, a.y};
        if (b.x > l3_solar_wind.x+l3_solar_wind.width) b.x = l3_solar_wind.x + l3_solar_wind.width;
        DrawLineEx(a,b,2*l3_u,Fade(l3_hazard_yellow,0.6));
    }
}


void l3_draw_obstacles()
{
    float t = l3_animation_time;

    //planets: glow, shaded l3_ball, thin ring
    Color planet_light[2] = {{255,190,120,255},{120,240,220,255}};
    Color planet_dark[2] = {{150,60,30,255},{20,90,110,255}};
    for (int i=0; i<2; i++)
    {
        DrawCircleGradient(l3_planet[i],l3_planet_field*0.5,Fade(planet_light[i],0.12),BLANK);
        DrawCircleGradient(l3_planet[i],l3_planet_core[i],planet_light[i],planet_dark[i]);
        DrawCircle(l3_planet[i].x-8*l3_u,l3_planet[i].y-9*l3_u,6*l3_u,Fade(WHITE,0.35));
        DrawRing(l3_planet[i],l3_planet_core[i]+8*l3_u,l3_planet_core[i]+11*l3_u,0,360,48,Fade(planet_light[i],0.5));
    }

    //wormholes: spinning rings at the way in, a dimmer ring and an arrow at the way out
    Color wormhole_colour[3] = {l3_neon_purple,l3_neon_cyan,l3_neon_orange};
    for (int i=0; i<3; i++)
    {
        Color c = wormhole_colour[i];
        DrawCircleGradient(l3_wormhole_in[i],l3_wormhole_radius*1.6,Fade(c,0.35),BLANK);
        for (int j=0; j<3; j++)
        {
            float ring = l3_wormhole_radius*(0.4 + j*0.25);
            float angle = t*(260 + j*80) + j*120;
            DrawRing(l3_wormhole_in[i],ring,ring+3*l3_u,angle,angle+200,24,Fade(c,0.9-j*0.2));
        }
        DrawCircle(l3_wormhole_in[i].x,l3_wormhole_in[i].y,l3_wormhole_radius*0.3,BLACK);

        DrawRing(l3_wormhole_out[i],l3_wormhole_radius*0.7,l3_wormhole_radius*0.8,-t*120,-t*120+270,24,Fade(c,0.6));
        Vector2 facing = {cos(l3_wormhole_angle[i]*DEG2RAD),sin(l3_wormhole_angle[i]*DEG2RAD)};
        Vector2 arrow = Vector2Add(l3_wormhole_out[i],Vector2Scale(facing,l3_wormhole_radius+6*l3_u));
        DrawPoly(arrow,3,9*l3_u,l3_wormhole_angle[i],Fade(c,0.85));
    }

    //energy bumpers, they get bigger for a moment when hit
    for (int i=0; i<4; i++)
    {
        float r = l3_bumper_radius*(1 + l3_bumper_hit[i]*1.5);
        float pulse = 0.5 + 0.5*sin(t*4 + i);
        DrawCircleGradient(l3_bumper[i],r*1.8,Fade(l3_neon_green,0.1+0.25*pulse),BLANK);
        DrawCircle(l3_bumper[i].x,l3_bumper[i].y,r,GetColor(0x0E2A1AFF));
        DrawRing(l3_bumper[i],r-4*l3_u,r,0,360,32,l3_neon_green);
        DrawCircle(l3_bumper[i].x,l3_bumper[i].y,r*0.35,Fade(WHITE,0.6+0.3*pulse));
    }

    //asteroids, turning slowly
    for (int i=0; i<4; i++)
    {
        float size = l3_asteroid_radius[i]*2.4;
        Rectangle source = {(i%3)*192,0,192,192};
        Rectangle dest = {l3_asteroid[i].x,l3_asteroid[i].y,size,size};
        Vector2 origin = {size/2,size/2};
        float spin = t*35;
        if (i%2==1) spin = -t*28;
        DrawTexturePro(l3_asteroid_texture,source,dest,origin,spin,WHITE);
    }

    //lost in space: rings shrinking into the spot
    if (l3_lost_timer>0)
    {
        for (int j=0; j<3; j++)
        {
            float ring = l3_lost_timer*80*l3_u + j*10*l3_u;
            DrawRing(l3_lost_position,ring,ring+2*l3_u,0,360,32,Fade(l3_neon_purple,l3_lost_timer*2));
        }
    }

    //spinning satellite (the level 1 l3_fan, with solar panels)
    Rectangle blade = {l3_fan.x,l3_fan.y,l3_fan_blade_length,l3_fan_blade_thickness};
    Vector2 blade_origin = {l3_fan_blade_length/2,l3_fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,l3_fan_angle-24,Fade(l3_panel_blue,0.12));
    DrawRectanglePro(blade,blade_origin,l3_fan_angle+90-24,Fade(l3_panel_blue,0.12));
    DrawRectanglePro(blade,blade_origin,l3_fan_angle-12,Fade(l3_panel_blue,0.25));
    DrawRectanglePro(blade,blade_origin,l3_fan_angle+90-12,Fade(l3_panel_blue,0.25));
    DrawRectanglePro(blade,blade_origin,l3_fan_angle,l3_panel_blue);
    DrawRectanglePro(blade,blade_origin,l3_fan_angle+90,l3_panel_blue);
    DrawCircle(l3_fan.x,l3_fan.y,l3_fan_hub_radius,l3_steel_dark);
    DrawCircleLines(l3_fan.x,l3_fan.y,l3_fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*l3_u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(l3_fan,Vector2Rotate(bolt_arm,(l3_fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*l3_u,l3_steel_light);
    }

    //laser gates (from level 1)
    for (int i=0; i<2; i++)
    {
        Rectangle laser = l3_laser_gate[i];
        float laser_timer = l3_laser_clock[i];
        float wall = 24*l3_u;
        float beam_y = laser.y + laser.height/2;
        Rectangle left_emitter = {laser.x-wall+4*l3_u,beam_y-14*l3_u,wall-4*l3_u,28*l3_u};
        Rectangle right_emitter = {laser.x+laser.width,beam_y-14*l3_u,wall-4*l3_u,28*l3_u};
        DrawRectangleRec(left_emitter,l3_steel_dark);
        DrawRectangleRec(right_emitter,l3_steel_dark);
        DrawRectangleLinesEx(left_emitter,2*l3_u,BLACK);
        DrawRectangleLinesEx(right_emitter,2*l3_u,BLACK);
        Color laser_lamp = GetColor(0x4A3A20FF);
        int dashes = laser.width/(16*l3_u);
        if (laser_timer<1.6)
        {
            float dash_alpha = 0.3;
            if (laser_timer>=1.2)
            {
                dash_alpha = 0.55;
                if (fmod(t*10,2)<1) laser_lamp = l3_hazard_yellow;
            }
            for (int j=0; j<dashes; j++)
            {
                DrawRectangle(laser.x+j*16*l3_u+4*l3_u,beam_y-1*l3_u,8*l3_u,2*l3_u,Fade(l3_laser_red,dash_alpha));
            }
        }
        else
        {
            laser_lamp = l3_laser_red;
            DrawRectangle(laser.x,beam_y-14*l3_u,laser.width,28*l3_u,Fade(l3_laser_red,0.15));
            DrawRectangle(laser.x,beam_y-7*l3_u,laser.width,14*l3_u,Fade(l3_laser_red,0.3));
            DrawRectangleRec(laser,l3_laser_red);
            DrawRectangle(laser.x,beam_y-1*l3_u,laser.width,2*l3_u,WHITE);
        }
        DrawCircle(left_emitter.x+left_emitter.width/2,beam_y,5*l3_u,laser_lamp);
        DrawCircle(right_emitter.x+right_emitter.width/2,beam_y,5*l3_u,laser_lamp);
    }
}

void l3_draw_ball_and_pot()
{
    //start plate
    Rectangle plate = {l3_start_position.x-50*l3_u,l3_start_position.y-30*l3_u,100*l3_u,60*l3_u};
    DrawRectangleRec(plate,l3_steel);
    DrawRectangleLinesEx(plate,3*l3_u,l3_steel_dark);
    Rectangle plate_band = {plate.x,plate.y+plate.height-10*l3_u,plate.width,10*l3_u};
    l3_draw_hazard_stripes(plate_band);
    DrawText("START",l3_start_position.x-MeasureText("START",18*l3_u)/2,plate.y+5*l3_u,18*l3_u,WHITE);

    //l3_pot with hazard ring
    for (int i=0; i<8; i++)
    {
        Color colour = l3_hazard_yellow;
        if (i%2==1) colour = BLACK;
        DrawRing(l3_pot,l3_radius_pot+6*l3_u,l3_radius_pot+12*l3_u,i*45,i*45+45,6,colour);
    }
    DrawCircle(l3_pot.x,l3_pot.y,l3_radius_pot+6*l3_u,l3_steel_light);
    DrawRing(l3_pot,l3_radius_pot+1*l3_u,l3_radius_pot+5*l3_u,0,360,24,l3_steel);
    DrawCircle(l3_pot.x,l3_pot.y,l3_radius_pot,BLACK);

    //flag
    Vector2 pole_top = {l3_pot.x,l3_pot.y-60*l3_u};
    DrawLineEx(l3_pot,pole_top,3*l3_u,l3_steel_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(l3_animation_time*6 - i*0.8)*2.5*l3_u;
        DrawRectangle(l3_pot.x+1*l3_u+i*6*l3_u,pole_top.y+wave,6*l3_u,18*l3_u,l3_laser_red);
    }

    //l3_ball
    DrawCircle(l3_ball.x+3*l3_u,l3_ball.y+4*l3_u,l3_radius_ball,Fade(BLACK,0.45));
    DrawCircle(l3_ball.x,l3_ball.y,l3_radius_ball,GetColor(0xEDEDEDFF));
    DrawCircleLines(l3_ball.x,l3_ball.y,l3_radius_ball,GRAY);
    DrawCircle(l3_ball.x-2*l3_u,l3_ball.y-2*l3_u,2*l3_u,WHITE);

    //aim line
    if (l3_aiming==1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        DrawLineEx(l3_ball,mouse,4*l3_u,Fade(WHITE,0.75));
        DrawCircle(mouse.x,mouse.y,5*l3_u,Fade(WHITE,0.75));
    }
}

//short preview of the shot (0.3 seconds), gravity bend included
void l3_draw_aim_preview()
{
    if (preview_seconds<=0) return;
    if (l3_aiming==0 || IsMouseButtonDown(MOUSE_BUTTON_LEFT)==0) return;
    Vector2 mouse = {GetMouseX(),GetMouseY()};
    Vector2 drag = Vector2Subtract(l3_ball,mouse);
    if (Vector2Length(drag)<2*l3_radius_ball) return;

    //remember everything l3_update_ball can change
    Vector2 saved_ball = l3_ball;
    Vector2 saved_speed = l3_speed;
    Vector2 saved_last_shot = l3_last_shot_position;
    Vector2 saved_lost_position = l3_lost_position;
    float saved_lost_timer = l3_lost_timer;
    float saved_message_timer = l3_message_timer;
    int saved_message_type = l3_message_type;
    int saved_game_state = l3_game_state;
    float saved_cooldown = l3_wormhole_cooldown;
    float saved_hit[4];
    for (int i=0; i<4; i++) saved_hit[i] = l3_bumper_hit[i];

    //try the shot for 72 tiny steps, a dot every 6, stop at a hazard or a wormhole jump
    l3_speed = Vector2ClampValue(Vector2Scale(drag,3.125),0,l3_max_speed*l3_u);
    l3_message_timer = -1;
    for (int i=1; i<=preview_seconds*240; i++)
    {
        Vector2 before = l3_ball;
        l3_update_ball(1.0/240);
        if (l3_message_timer==1 || l3_game_state!=0 || Vector2Distance(before,l3_ball)>20*l3_u) break;
        if (i%6==0) DrawCircle(l3_ball.x,l3_ball.y,3*l3_u,Fade(WHITE,1-i/(preview_seconds*240+18)));
        if (l3_speed.x==0 && l3_speed.y==0) break;
    }

    //put everything back
    l3_ball = saved_ball;
    l3_speed = saved_speed;
    l3_last_shot_position = saved_last_shot;
    l3_lost_position = saved_lost_position;
    l3_lost_timer = saved_lost_timer;
    l3_message_timer = saved_message_timer;
    l3_message_type = saved_message_type;
    l3_game_state = saved_game_state;
    l3_wormhole_cooldown = saved_cooldown;
    for (int i=0; i<4; i++) l3_bumper_hit[i] = saved_hit[i];
}


void l3_draw_events()
{
    float t = l3_animation_time;
    if (l3_event_type==0) return;

    //warning: blinking red arrow at the edge where it comes in
    if (l3_event_timer<1.5)
    {
        Vector2 sign = l3_event_start;
        sign.x = Clamp(sign.x,40*l3_u,l3_width-40*l3_u);
        sign.y = Clamp(sign.y,l3_hud_height+40*l3_u,l3_height-40*l3_u);
        float angle = atan2(l3_event_direction.y,l3_event_direction.x)*RAD2DEG;
        if (fmod(t*6,2)<1.3)
        {
            DrawCircle(sign.x,sign.y,26*l3_u,Fade(RED,0.35));
            DrawPoly(sign,3,18*l3_u,angle,RED);
        }
        return;
    }

    //l3_comet: bright head and a fading tail
    if (l3_event_type==1)
    {
        for (int j=12; j>0; j--)
        {
            Vector2 bit = Vector2Subtract(l3_comet,Vector2Scale(l3_event_direction,j*14*l3_u));
            DrawCircle(bit.x,bit.y,(12-j*0.8)*l3_u,Fade(l3_neon_cyan,0.5-j*0.035));
        }
        DrawCircleGradient(l3_comet,26*l3_u,Fade(WHITE,0.6),BLANK);
        DrawCircle(l3_comet.x,l3_comet.y,12*l3_u,WHITE);
    }

    //l3_meteor shower: small rocks with orange tails
    if (l3_event_type==2)
    {
        for (int i=0; i<5; i++)
        {
            Vector2 tail = Vector2Subtract(l3_meteor[i],Vector2Scale(l3_event_direction,40*l3_u));
            DrawLineEx(tail,l3_meteor[i],6*l3_u,Fade(l3_neon_orange,0.5));
            float size = 29*l3_u;
            Rectangle source = {(i%3)*192,0,192,192};
            Rectangle dest = {l3_meteor[i].x,l3_meteor[i].y,size,size};
            Vector2 origin = {size/2,size/2};
            DrawTexturePro(l3_asteroid_texture,source,dest,origin,t*200,WHITE);
        }
    }

    //l3_ufo: green beam while it takes the l3_ball, chasing lights otherwise
    if (l3_event_type==3)
    {
        int frame = (int)(t*6)%3;
        if (l3_beam_timer>0 || l3_abducted==1)
        {
            frame = 3;
            DrawCircleGradient(l3_ufo,80*l3_u,Fade(l3_neon_green,0.45),Fade(l3_neon_green,0.05));
            DrawRing(l3_ufo,76*l3_u,80*l3_u,0,360,48,Fade(l3_neon_green,0.6));
        }
        DrawCircle(l3_ufo.x+20*l3_u,l3_ufo.y+26*l3_u,70*l3_u,Fade(BLACK,0.3));
        Rectangle source = {frame*320,0,320,320};
        Rectangle dest = {l3_ufo.x,l3_ufo.y,150*l3_u,150*l3_u};
        Vector2 origin = {75*l3_u,75*l3_u};
        DrawTexturePro(l3_ufo_texture,source,dest,origin,0,WHITE);
    }
}


void l3_draw_hud()
{
    //dark edges
    DrawRectangleGradientH(0,l3_hud_height,90*l3_u,l3_height-l3_hud_height,Fade(BLACK,0.45),BLANK);
    DrawRectangleGradientH(l3_width-90*l3_u,l3_hud_height,90*l3_u,l3_height-l3_hud_height,BLANK,Fade(BLACK,0.45));
    DrawRectangleGradientV(0,l3_height-70*l3_u,l3_width,70*l3_u,BLANK,Fade(BLACK,0.45));

    //l3_steel bar
    DrawRectangleGradientV(0,0,l3_width,l3_hud_height,l3_steel_light,l3_steel_dark);
    DrawRectangle(0,l3_hud_height-4*l3_u,l3_width,4*l3_u,BLACK);
    int rivets = l3_width/(40*l3_u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*l3_u+i*40*l3_u,8*l3_u,3*l3_u,l3_steel_dark);
        DrawCircle(20*l3_u+i*40*l3_u,l3_hud_height-12*l3_u,3*l3_u,l3_steel_dark);
    }

    //text
    DrawText("LEVEL 3 - EVENT HORIZON",32*l3_u,20*l3_u,36*l3_u,BLACK);
    DrawText("LEVEL 3 - EVENT HORIZON",30*l3_u,18*l3_u,36*l3_u,l3_hazard_yellow);
    DrawText(TextFormat("STROKES %d / %d",l3_stroke,l3_stroke_limit),l3_width/2-298*l3_u,21*l3_u,32*l3_u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",l3_stroke,l3_stroke_limit),l3_width/2-300*l3_u,19*l3_u,32*l3_u,WHITE);
    for (int i=0; i<l3_stroke_limit; i++)
    {
        Color pip = l3_steel_dark;
        if (i<l3_stroke) pip = l3_laser_red;
        DrawCircle(l3_width/2+10*l3_u+i*22*l3_u,35*l3_u,7*l3_u,pip);
        DrawCircleLines(l3_width/2+10*l3_u+i*22*l3_u,35*l3_u,7*l3_u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC menu",26*l3_u);
    DrawText("R restart   ESC menu",l3_width-help_width-28*l3_u,24*l3_u,26*l3_u,BLACK);
    DrawText("R restart   ESC menu",l3_width-help_width-30*l3_u,22*l3_u,26*l3_u,WHITE);

    //lost, zapped or beamed up message
    if (l3_message_timer>0)
    {
        if (l3_message_type==1)
        {
            DrawText("LOST IN SPACE!",l3_width/2-MeasureText("LOST IN SPACE!",90*l3_u)/2+4*l3_u,l3_height/2-41*l3_u,90*l3_u,Fade(BLACK,l3_message_timer));
            DrawText("LOST IN SPACE!",l3_width/2-MeasureText("LOST IN SPACE!",90*l3_u)/2,l3_height/2-45*l3_u,90*l3_u,Fade(l3_neon_purple,l3_message_timer));
        }
        else if (l3_message_type==2)
        {
            DrawText("ZAPPED!",l3_width/2-MeasureText("ZAPPED!",90*l3_u)/2+4*l3_u,l3_height/2-41*l3_u,90*l3_u,Fade(BLACK,l3_message_timer));
            DrawText("ZAPPED!",l3_width/2-MeasureText("ZAPPED!",90*l3_u)/2,l3_height/2-45*l3_u,90*l3_u,Fade(l3_laser_red,l3_message_timer));
        }
        else
        {
            DrawText("BEAMED UP!",l3_width/2-MeasureText("BEAMED UP!",90*l3_u)/2+4*l3_u,l3_height/2-41*l3_u,90*l3_u,Fade(BLACK,l3_message_timer));
            DrawText("BEAMED UP!",l3_width/2-MeasureText("BEAMED UP!",90*l3_u)/2,l3_height/2-45*l3_u,90*l3_u,Fade(l3_neon_green,l3_message_timer));
        }
    }

    //level clear or failed
    if (l3_game_state!=0)
    {
        DrawRectangle(0,0,l3_width,l3_height,Fade(BLACK,0.6));
        Rectangle panel = {l3_width/2-340*l3_u,l3_height/2-170*l3_u,680*l3_u,340*l3_u};
        DrawRectangleRec(panel,l3_steel_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*l3_u};
        l3_draw_hazard_stripes(panel_band);
        DrawRectangleLinesEx(panel,6*l3_u,l3_steel_light);
        if (l3_game_state==1)
        {
            DrawText("LEVEL CLEAR!",l3_width/2-MeasureText("LEVEL CLEAR!",80*l3_u)/2,panel.y+60*l3_u,80*l3_u,l3_hazard_yellow);
        }
        else
        {
            DrawText("FAILED",l3_width/2-MeasureText("FAILED",80*l3_u)/2,panel.y+60*l3_u,80*l3_u,l3_laser_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",l3_stroke,l3_stroke_limit),40*l3_u);
        DrawText(TextFormat("Strokes: %d / %d",l3_stroke,l3_stroke_limit),l3_width/2-strokes_width/2,panel.y+170*l3_u,40*l3_u,WHITE);
        DrawText("R play again   ESC menu",l3_width/2-MeasureText("R play again   ESC menu",30*l3_u)/2,panel.y+250*l3_u,30*l3_u,l3_steel_light);
    }
}


//level 3: keep the obstacles moving with no ball (intro and menu)
void l3_background_step(float dt)
{
    l3_animation_time = l3_animation_time + dt;
    l3_update_obstacles(dt);
}


//level 3: draw everything except the scoreboard (intro and menu)
void l3_draw_scene()
{

    l3_draw_space();
    l3_draw_walkways();
    l3_draw_obstacles();
    l3_draw_ball_and_pot();
    l3_draw_aim_preview();
    l3_draw_events();

}


//level 3: set the screen size, load pictures, start the level
void l3_start(int screen_width, int screen_height)
{
    l3_width = screen_width;
    l3_height = screen_height;

    //pictures (made at 2x size, so they stay sharp)
    l3_asteroid_texture = LoadTexture("assets/space/space_asteroid.png");
    l3_ufo_texture = LoadTexture("assets/space/space_ufo.png");
    SetTextureFilter(l3_asteroid_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l3_ufo_texture,TEXTURE_FILTER_BILINEAR);

    //every size is N*l3_u, so it looks the same on any screen
    l3_u = l3_height/1080.0;
    if (l3_width/1920.0 < l3_u) l3_u = l3_width/1920.0;
    l3_make_stars();
    l3_reset_level();

}


//level 3: one frame of input and movement
void l3_update()
{
    float dt = GetFrameTime();
    if (dt>1.0/30) dt = 1.0/30;
    l3_animation_time = l3_animation_time + dt;
    if (l3_message_timer>0) l3_message_timer = l3_message_timer - dt;

    //restart
    if (IsKeyPressed(KEY_R)) l3_reset_level();

    //shooting (the click has to start while the l3_ball is still)
    int ball_stopped = 0;
    if (l3_speed.x==0 && l3_speed.y==0) ball_stopped = 1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && l3_game_state==0 && l3_abducted==0) l3_aiming = 1;
    if (l3_game_state!=0) l3_aiming = 0;
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && l3_aiming==1)
    {
        l3_aiming = 0;
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        Vector2 drag = Vector2Subtract(l3_ball,mouse);
        if (ball_stopped==1 && l3_stroke<l3_stroke_limit && Vector2Length(drag)>=2*l3_radius_ball)
        {
            l3_last_shot_position = l3_ball;
            l3_speed = Vector2ClampValue(Vector2Scale(drag,3.125),0,l3_max_speed*l3_u);
            l3_stroke++;
        }
    }

    //moving everything in 4 small steps, so nothing jumps through anything
    for (int i=0; i<4; i++)
    {
        l3_update_obstacles(dt/4*obstacle_speed);
        l3_update_events(dt/4*obstacle_speed);
        if (l3_game_state==0 && l3_abducted==0) l3_update_ball(dt/4);
    }

    //out of strokes
    if (l3_game_state==0 && l3_stroke>=l3_stroke_limit && l3_speed.x==0 && l3_speed.y==0) l3_game_state = 2;


}


//level 3: one frame of drawing
void l3_draw()
{

    l3_draw_space();
    l3_draw_walkways();
    l3_draw_obstacles();
    l3_draw_ball_and_pot();
    l3_draw_aim_preview();
    l3_draw_events();
    l3_draw_hud();

}


void l3_unload()
{
    UnloadTexture(l3_asteroid_texture);
    UnloadTexture(l3_ufo_texture);
}


//==================== LEVEL 4 ====================

//full global
int l4_width = 1920;
int l4_height = 1080;
float l4_u = 1;
int l4_stroke_base = 18;
int l4_stroke_limit = 18;
#define l4_max_speed 650

//colours
Color l4_brick = {139,58,43,255};
Color l4_brick_dark = {110,44,32,255};
Color l4_mortar = {58,51,48,255};
Color l4_steel = {90,95,102,255};
Color l4_steel_light = {138,144,153,255};
Color l4_steel_dark = {46,49,54,255};
Color l4_floor_colour = {74,74,72,255};
Color l4_hazard_yellow = {232,185,35,255};
Color l4_rust = {181,84,28,255};
Color l4_rust_dark = {107,46,14,255};
Color l4_molten_orange = {255,106,0,255};
Color l4_molten_yellow = {255,208,0,255};
Color l4_molten_dark = {139,26,0,255};
Color l4_laser_red = {255,32,48,255};
Color l4_magnet_red = {192,40,45,255};
Color l4_stone = {120,126,112,255};
Color l4_stone_dark = {84,90,78,255};
Color l4_stone_mortar = {52,58,48,255};
Color l4_moss_green = {94,127,58,255};
Color l4_jungle_dark = {24,60,32,255};
Color l4_gold = {232,184,64,255};
Color l4_gold_light = {255,224,130,255};
Color l4_river_tint = {150,215,170,255};
Color l4_wood_brown = {120,80,40,255};

//pictures
Texture2D l4_ground_texture;
Texture2D l4_temple_texture;
Texture2D l4_foliage_texture;
Texture2D l4_plank_texture;
Texture2D l4_water_texture;
Texture2D l4_foam_texture;
Texture2D l4_splash_texture;
Texture2D l4_boulder_texture;

//l4_ball and l4_pot
Vector2 l4_ball;
Vector2 l4_speed;
float l4_radius_ball;
Vector2 l4_pot;
float l4_radius_pot;
Vector2 l4_start_position;
Vector2 l4_last_shot_position;
int l4_stroke = 0;
int l4_game_state = 0;
int l4_aiming = 0;
float l4_message_timer = 0;
int l4_message_type = 0;
float l4_animation_time = 0;
float l4_hud_height;
Vector2 l4_no_speed = {0,0};

//l4_walls: outer frame, temple l4_walls, altar room l4_walls, and the 2 jungle hedges (last two)
Rectangle l4_walls[13];
Rectangle l4_temple_area;

//l4_river (fall in = chomp) and the l4_plank bridges: 0 normal one at the top, 1 risky shortcut at the bottom
Rectangle l4_river;
Rectangle l4_plank[2][5];
float l4_plank_timer[2][5];
float l4_crack_time[2];
Vector2 l4_bridge_bank[2];

//l4_mud (slow), l4_moss (slippery), l4_quicksand (slow, and a l4_ball that stays still sinks)
Rectangle l4_mud;
Rectangle l4_moss[2];
Rectangle l4_quicksand[2];
float l4_sink_timer = 0;

//boulders rolling back and forth (the level 3 asteroids)
Vector2 l4_boulder[3];
Vector2 l4_boulder_start[3];
Vector2 l4_boulder_end[3];
Vector2 l4_boulder_velocity[3];
float l4_boulder_t[3];
float l4_boulder_time[3];
float l4_boulder_radius[3];
int l4_boulder_direction[3];

//dart traps (the level 1 laser): off 1.2s, warning 0.4s, on 1.4s
Rectangle l4_dart_gate[4];
float l4_dart_clock[4];

//spinning totem (the level 1 l4_fan)
Vector2 l4_fan;
float l4_fan_angle = 0;
float l4_fan_spin = 100;
float l4_fan_blade_length;
float l4_fan_blade_thickness;
float l4_fan_hub_radius;

//mushrooms (the level 3 bumpers)
Vector2 l4_bumper[6];
float l4_bumper_radius;
float l4_bumper_hit[6];

//pressure plates (0 opens the temple gate for 6s, 1 and 2 open the altar l4_door) and the 2 l4_stone doors
Vector2 l4_plate[3];
float l4_plate_radius;
int l4_plate_down[3];
float l4_gate_timer = 0;
Rectangle l4_door[2];
float l4_door_closed_y[2];
float l4_door_open[2];
Vector2 l4_door_velocity[2];

//splash
Vector2 l4_splash_position;
float l4_splash_timer = 0;


//a rectangle in design units (a 1920 x 1080 screen), made to fit the real screen
Rectangle l4_make_rect(float x, float y, float w, float h)
{
    Rectangle rec = {x*l4_u,y*l4_u,w*l4_u,h*l4_u};
    return rec;
}

//a point in design units
Vector2 l4_make_point(float x, float y)
{
    Vector2 point = {x*l4_u,y*l4_u};
    return point;
}


void l4_reset_level()
{
    //sizes
    l4_hud_height = 70*l4_u;
    l4_radius_ball = 7*l4_u;
    l4_radius_pot = 11*l4_u;

    //l4_walls
    l4_walls[0] = l4_make_rect(0,70,1920,28);        //frame top
    l4_walls[1] = l4_make_rect(0,1052,1920,28);      //frame bottom
    l4_walls[2] = l4_make_rect(0,70,28,1010);        //frame left
    l4_walls[3] = l4_make_rect(1892,70,28,1010);     //frame right
    l4_walls[4] = l4_make_rect(1152,98,28,422);      //temple outer wall, above the gate
    l4_walls[5] = l4_make_rect(1152,620,28,432);     //temple outer wall, below the gate
    l4_walls[6] = l4_make_rect(1380,300,348,28);     //altar room top
    l4_walls[7] = l4_make_rect(1380,822,348,28);     //altar room bottom
    l4_walls[8] = l4_make_rect(1380,300,28,550);     //altar room left
    l4_walls[9] = l4_make_rect(1700,300,28,220);     //altar room right, above the l4_door
    l4_walls[10] = l4_make_rect(1700,620,28,230);    //altar room right, below the l4_door
    l4_walls[11] = l4_make_rect(190,700,430,40);     //jungle hedge, low
    l4_walls[12] = l4_make_rect(28,400,430,40);      //jungle hedge, high
    l4_temple_area = l4_make_rect(1152,98,740,954);

    //l4_river and bridges
    l4_river = l4_make_rect(620,98,140,954);
    for (int i=0; i<5; i++)
    {
        l4_plank[0][i] = l4_make_rect(620+i*28,165,28,90);
        l4_plank[1][i] = l4_make_rect(620+i*28,915,28,90);
        l4_plank_timer[0][i] = 0;
        l4_plank_timer[1][i] = 0;
    }
    l4_crack_time[0] = 0.5;
    l4_crack_time[1] = 0.2;
    l4_bridge_bank[0] = l4_make_point(590,210);
    l4_bridge_bank[1] = l4_make_point(590,960);

    //ground patches
    l4_mud = l4_make_rect(250,850,180,140);
    l4_moss[0] = l4_make_rect(770,260,130,180);
    l4_moss[1] = l4_make_rect(1740,330,140,170);
    l4_quicksand[0] = l4_make_rect(850,720,200,150);
    l4_quicksand[1] = l4_make_rect(1200,120,160,160);
    l4_sink_timer = 0;

    //boulders
    l4_boulder_start[0] = l4_make_point(100,570);
    l4_boulder_end[0] = l4_make_point(560,570);
    l4_boulder_radius[0] = 30*l4_u;
    l4_boulder_time[0] = 3.0;
    l4_boulder_start[1] = l4_make_point(1070,420);
    l4_boulder_end[1] = l4_make_point(830,420);
    l4_boulder_radius[1] = 28*l4_u;
    l4_boulder_time[1] = 2.2;
    l4_boulder_start[2] = l4_make_point(1280,380);
    l4_boulder_end[2] = l4_make_point(1280,760);
    l4_boulder_radius[2] = 30*l4_u;
    l4_boulder_time[2] = 2.6;
    for (int i=0; i<3; i++)
    {
        l4_boulder_t[i] = 0;
        l4_boulder_direction[i] = 1;
        l4_boulder[i] = l4_boulder_start[i];
        l4_boulder_velocity[i] = l4_no_speed;
    }

    //dart traps
    l4_dart_gate[0] = l4_make_rect(1180,330,200,8);
    l4_dart_gate[1] = l4_make_rect(1180,820,200,8);
    l4_dart_gate[2] = l4_make_rect(1540,98,8,202);
    l4_dart_gate[3] = l4_make_rect(1540,850,8,202);
    for (int i=0; i<4; i++) l4_dart_clock[i] = i*0.75;

    //totem in front of the altar l4_door
    l4_fan = l4_make_point(1810,570);
    l4_fan_blade_length = 80*l4_u;
    l4_fan_blade_thickness = 12*l4_u;
    l4_fan_hub_radius = 12*l4_u;
    l4_fan_angle = 0;

    //mushrooms
    l4_bumper_radius = 22*l4_u;
    l4_bumper[0] = l4_make_point(250,220);
    l4_bumper[1] = l4_make_point(430,300);
    l4_bumper[2] = l4_make_point(450,820);
    l4_bumper[3] = l4_make_point(1060,960);
    l4_bumper[4] = l4_make_point(1480,470);
    l4_bumper[5] = l4_make_point(1620,690);
    for (int i=0; i<6; i++) l4_bumper_hit[i] = 0;

    //plates and doors
    l4_plate_radius = 26*l4_u;
    l4_plate[0] = l4_make_point(1000,570);
    l4_plate[1] = l4_make_point(1810,200);
    l4_plate[2] = l4_make_point(1810,950);
    for (int i=0; i<3; i++) l4_plate_down[i] = 0;
    l4_gate_timer = 0;
    l4_door[0] = l4_make_rect(1152,520,28,100);
    l4_door[1] = l4_make_rect(1700,520,28,100);
    for (int i=0; i<2; i++)
    {
        l4_door_closed_y[i] = l4_door[i].y;
        l4_door_open[i] = 0;
        l4_door_velocity[i] = l4_no_speed;
    }

    //l4_ball and l4_pot
    l4_start_position = l4_make_point(120,980);
    l4_ball = l4_start_position;
    l4_last_shot_position = l4_start_position;
    l4_speed.x = 0;
    l4_speed.y= 0;
    l4_pot = l4_make_point(1560,575);
    l4_stroke = 0;
    l4_game_state = 0;
    l4_aiming = 0;
    l4_message_timer = 0;
    l4_splash_timer = 0;
}


//bounce off a straight rectangle, rec_speed is how fast the rectangle itself is moving
int l4_bounce_off_rectangle(Rectangle rec, Vector2 rec_speed)
{
    if (CheckCollisionCircleRec(l4_ball,l4_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(l4_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(l4_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(l4_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = l4_ball.x - rec.x;
            float right = rec.x + rec.width - l4_ball.x;
            float top = l4_ball.y - rec.y;
            float bottom = rec.y + rec.height - l4_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }
        normal = Vector2Normalize(normal);
        l4_ball = Vector2Add(collision_point,Vector2Scale(normal,l4_radius_ball));

        //reflect only if the l4_ball is going into it
        Vector2 relative_speed = Vector2Subtract(l4_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l4_speed = Vector2Add(relative_speed,rec_speed);

            //moving crab side: also knock the l4_ball out of its row, or it gets hit again and again
            if (rec_speed.x!=0 && normal.y==0)
            {
                if (l4_ball.y < rec.y+rec.height/2) l4_speed.y = l4_speed.y - fabsf(rec_speed.x)/2;
                else l4_speed.y = l4_speed.y + fabsf(rec_speed.x)/2;
            }
            l4_speed = Vector2ClampValue(l4_speed,0,l4_max_speed*l4_u);
        }
        return 1;
    }
    return 0;
}

//bounce off a round thing: bounce is 1 for normal and more for bouncy, circle_speed is how fast it moves
int l4_bounce_off_circle(Vector2 center, float radius, float bounce, Vector2 circle_speed)
{
    Vector2 normal = Vector2Subtract(l4_ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + l4_radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        l4_ball = Vector2Add(center,Vector2Scale(normal,radius + l4_radius_ball));
        Vector2 relative_speed = Vector2Subtract(l4_speed,circle_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Scale(Vector2Reflect(relative_speed,normal),bounce);
            l4_speed = Vector2ClampValue(Vector2Add(relative_speed,circle_speed),0,l4_max_speed*l4_u);
            return 1;
        }
    }
    return 0;
}

//bounce off a turned rectangle (diamond, l4_fan blades), spin is in degrees per second
void l4_bounce_off_rotated_rectangle(Vector2 center, float rec_width, float rec_height, float angle, float spin)
{
    //look at the l4_ball as if the rectangle was not turned
    Vector2 local_ball = Vector2Rotate(Vector2Subtract(l4_ball,center),-angle*DEG2RAD);
    Rectangle rec = {-rec_width/2,-rec_height/2,rec_width,rec_height};
    if (CheckCollisionCircleRec(local_ball,l4_radius_ball,rec))
    {
        Vector2 collision_point;
        collision_point.x = Clamp(local_ball.x,rec.x,rec.x+rec.width);
        collision_point.y = Clamp(local_ball.y,rec.y,rec.y+rec.height);
        Vector2 normal = Vector2Subtract(local_ball,collision_point);

        //centre is inside, go out through the nearest side
        if (normal.x==0 && normal.y==0)
        {
            float left = local_ball.x - rec.x;
            float right = rec.x + rec.width - local_ball.x;
            float top = local_ball.y - rec.y;
            float bottom = rec.y + rec.height - local_ball.y;
            if (left<=right && left<=top && left<=bottom) { normal.x = -1; collision_point.x = rec.x; }
            else if (right<=top && right<=bottom) { normal.x = 1; collision_point.x = rec.x + rec.width; }
            else if (top<=bottom) { normal.y = -1; collision_point.y = rec.y; }
            else { normal.y = 1; collision_point.y = rec.y + rec.height; }
        }

        //turn the point and normal back to the screen
        normal = Vector2Rotate(Vector2Normalize(normal),angle*DEG2RAD);
        collision_point = Vector2Add(center,Vector2Rotate(collision_point,angle*DEG2RAD));
        l4_ball = Vector2Add(collision_point,Vector2Scale(normal,l4_radius_ball));

        //l4_speed of the blade at the point it touches the l4_ball
        Vector2 arm = Vector2Subtract(collision_point,center);
        Vector2 rec_speed = {-arm.y*spin*DEG2RAD, arm.x*spin*DEG2RAD};

        Vector2 relative_speed = Vector2Subtract(l4_speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            l4_speed = Vector2ClampValue(Vector2Add(relative_speed,rec_speed),0,l4_max_speed*l4_u);
        }
    }
}

//is this point in the l4_river, and not on a l4_plank that is still there
int l4_in_the_river(Vector2 point)
{
    if (CheckCollisionPointRec(point,l4_river)==0) return 0;
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (CheckCollisionPointRec(point,l4_plank[b][i]) && l4_plank_timer[b][i]<l4_crack_time[b]) return 0;
        }
    }
    return 1;
}


void l4_update_obstacles(float dt)
{
    //boulders rolling back and forth (the level 3 asteroids)
    for (int i=0; i<3; i++)
    {
        l4_boulder_t[i] = l4_boulder_t[i] + l4_boulder_direction[i]*dt/l4_boulder_time[i];
        if (l4_boulder_t[i]>=1)
        {
            l4_boulder_t[i] = 1;
            l4_boulder_direction[i] = -1;
        }
        if (l4_boulder_t[i]<=0)
        {
            l4_boulder_t[i] = 0;
            l4_boulder_direction[i] = 1;
        }
        l4_boulder[i] = Vector2Lerp(l4_boulder_start[i],l4_boulder_end[i],l4_boulder_t[i]);
        l4_boulder_velocity[i] = Vector2Scale(Vector2Subtract(l4_boulder_end[i],l4_boulder_start[i]),l4_boulder_direction[i]/l4_boulder_time[i]);
    }

    //totem spinning (the level 1 l4_fan)
    l4_fan_angle = l4_fan_angle + l4_fan_spin*dt;
    if (l4_fan_angle>=360) l4_fan_angle = l4_fan_angle - 360;

    //dart clocks
    for (int i=0; i<4; i++)
    {
        l4_dart_clock[i] = l4_dart_clock[i] + dt;
        if (l4_dart_clock[i]>=3.0) l4_dart_clock[i] = l4_dart_clock[i] - 3.0;
    }

    //planks: cracking, then gone for 5 seconds, then back
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (l4_plank_timer[b][i]>0) l4_plank_timer[b][i] = l4_plank_timer[b][i] + dt;
            if (l4_plank_timer[b][i]>=l4_crack_time[b]+5) l4_plank_timer[b][i] = 0;
        }
    }

    //doors slide down into the wall in half a second, and never close on the l4_ball (it jams)
    if (l4_gate_timer>0) l4_gate_timer = l4_gate_timer - dt;
    for (int i=0; i<2; i++)
    {
        int want_open = 0;
        if (i==0 && l4_gate_timer>0) want_open = 1;
        if (i==1 && l4_plate_down[1]==1 && l4_plate_down[2]==1) want_open = 1;
        Rectangle doorway = {l4_door[i].x,l4_door_closed_y[i],l4_door[i].width,100*l4_u};
        l4_door_velocity[i] = l4_no_speed;
        if (want_open==1 && l4_door_open[i]<1)
        {
            l4_door_open[i] = l4_door_open[i] + 2*dt;
            if (l4_door_open[i]>1) l4_door_open[i] = 1;
            l4_door_velocity[i].y = 200*l4_u;
        }
        if (want_open==0 && l4_door_open[i]>0 && CheckCollisionCircleRec(l4_ball,l4_radius_ball+2*l4_u,doorway)==0)
        {
            l4_door_open[i] = l4_door_open[i] - 2*dt;
            if (l4_door_open[i]<0) l4_door_open[i] = 0;
            l4_door_velocity[i].y = -200*l4_u;
        }
        l4_door[i].y = l4_door_closed_y[i] + l4_door_open[i]*100*l4_u;
    }

    for (int i=0; i<6; i++)
    {
        if (l4_bumper_hit[i]>0) l4_bumper_hit[i] = l4_bumper_hit[i] - dt;
    }
    if (l4_splash_timer>0) l4_splash_timer = l4_splash_timer - dt;
}


//after the l4_river, darts or l4_quicksand, the l4_ball goes back to where it was shot from
void l4_send_ball_back()
{
    l4_ball = l4_last_shot_position;

    //not right in an l4_boulder's path, or it gets knocked off again and again
    for (int i=0; i<3; i++)
    {
        Vector2 path = Vector2Subtract(l4_boulder_end[i],l4_boulder_start[i]);
        float along = Vector2DotProduct(Vector2Subtract(l4_ball,l4_boulder_start[i]),path)/Vector2DotProduct(path,path);
        along = Clamp(along,0,1);
        Vector2 closest = Vector2Add(l4_boulder_start[i],Vector2Scale(path,along));
        Vector2 away = Vector2Subtract(l4_ball,closest);
        float gap = l4_boulder_radius[i] + l4_radius_ball + 2*l4_u;
        if (Vector2Length(away)<gap)
        {
            Vector2 side = {-path.y,path.x};
            side = Vector2Normalize(side);
            if (Vector2DotProduct(away,side)<0) side = Vector2Negate(side);
            l4_ball = Vector2Add(closest,Vector2Scale(side,gap));
        }
    }

    //not where the totem arms sweep, or it gets knocked off again and again (the level 1 piston rule)
    float sweep = l4_fan_blade_length/2 + l4_radius_ball + 2*l4_u;
    if (Vector2Distance(l4_ball,l4_fan)<sweep)
    {
        if (l4_ball.y<l4_fan.y) l4_ball.y = l4_fan.y - sweep;
        else l4_ball.y = l4_fan.y + sweep;
    }

    //not inside a dart trap (level 1 rule)
    for (int i=0; i<4; i++)
    {
        if (CheckCollisionCircleRec(l4_ball,l4_radius_ball,l4_dart_gate[i])) l4_ball = l4_start_position;
    }

    //the l4_plank it was on has fallen: back to that bridge's bank
    if (l4_in_the_river(l4_ball))
    {
        Vector2 target = l4_ball;
        l4_ball = l4_start_position;
        for (int b=0; b<2; b++)
        {
            if (target.y>=l4_plank[b][0].y && target.y<=l4_plank[b][0].y+l4_plank[b][0].height) l4_ball = l4_bridge_bank[b];
        }
    }

    //falling in or getting hit lets both altar plates back up
    l4_plate_down[1] = 0;
    l4_plate_down[2] = 0;
    l4_sink_timer = 0;

    l4_speed.x = 0;
    l4_speed.y = 0;
}


void l4_update_ball(float dt)
{
    int pushed = 0;

    //moving
    l4_ball = Vector2Add(l4_ball,Vector2Scale(l4_speed,dt));

    //proportional deceleration (l4_mud and l4_quicksand are slow, l4_moss is slippery)
    float friction = 110*l4_u;
    int in_quicksand = 0;
    if (CheckCollisionPointRec(l4_ball,l4_mud)) friction = 400*l4_u;
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(l4_ball,l4_moss[i])) friction = 25*l4_u;
        if (CheckCollisionPointRec(l4_ball,l4_quicksand[i]))
        {
            friction = 400*l4_u;
            in_quicksand = 1;
        }
    }
    float ball_speed = Vector2Length(l4_speed);
    if (ball_speed>0)
    {
        float new_speed = ball_speed - friction*dt;
        if (new_speed<0) new_speed = 0;
        l4_speed = Vector2Scale(l4_speed,new_speed/ball_speed);
    }
    if (pushed==0 && Vector2Length(l4_speed)<6*l4_u)
    {
        l4_speed.x = 0;
        l4_speed.y= 0;
    }

    //planks start cracking when the l4_ball rolls onto them
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (l4_plank_timer[b][i]==0 && CheckCollisionPointRec(l4_ball,l4_plank[b][i])) l4_plank_timer[b][i] = 0.001;
        }
    }

    //pressure plates
    if (Vector2Distance(l4_ball,l4_plate[0])<l4_plate_radius) l4_gate_timer = 6;
    for (int i=1; i<3; i++)
    {
        if (Vector2Distance(l4_ball,l4_plate[i])<l4_plate_radius) l4_plate_down[i] = 1;
    }

    //l4_river (checked before the collisions, so if something is on the old spot it pushes the l4_ball out)
    if (l4_in_the_river(l4_ball))
    {
        l4_splash_position = l4_ball;
        l4_splash_timer = 0.5;
        l4_send_ball_back();
        l4_message_type = 1;
        l4_message_timer = 1;
    }

    //l4_quicksand: a l4_ball that stays still sinks in 3 seconds
    if (in_quicksand==1 && l4_speed.x==0 && l4_speed.y==0)
    {
        l4_sink_timer = l4_sink_timer + dt;
        if (l4_sink_timer>=3)
        {
            l4_send_ball_back();
            l4_message_type = 3;
            l4_message_timer = 1;
        }
    }
    else l4_sink_timer = 0;

    //dart traps (the level 1 laser)
    for (int i=0; i<4; i++)
    {
        if (l4_dart_clock[i]>=1.6 && CheckCollisionCircleRec(l4_ball,l4_radius_ball,l4_dart_gate[i]))
        {
            l4_send_ball_back();
            l4_message_type = 2;
            l4_message_timer = 1;
        }
    }

    //mushrooms and boulders
    for (int i=0; i<6; i++)
    {
        if (l4_bounce_off_circle(l4_bumper[i],l4_bumper_radius,1.3,l4_no_speed)) l4_bumper_hit[i] = 0.15;
    }
    for (int i=0; i<3; i++)
    {
        l4_bounce_off_circle(l4_boulder[i],l4_boulder_radius[i],1,l4_boulder_velocity[i]);
    }

    //spinning totem (the level 1 l4_fan)
    l4_bounce_off_circle(l4_fan,l4_fan_hub_radius,1,l4_no_speed);
    l4_bounce_off_rotated_rectangle(l4_fan,l4_fan_blade_length,l4_fan_blade_thickness,l4_fan_angle,l4_fan_spin);
    l4_bounce_off_rotated_rectangle(l4_fan,l4_fan_blade_length,l4_fan_blade_thickness,l4_fan_angle+90,l4_fan_spin);

    //l4_stone doors
    for (int i=0; i<2; i++)
    {
        l4_bounce_off_rectangle(l4_door[i],l4_door_velocity[i]);
    }

    //l4_walls (last, so the l4_ball never ends inside a wall)
    for (int i=0; i<13; i++)
    {
        l4_bounce_off_rectangle(l4_walls[i],l4_no_speed);
    }

    //score
    if ((l4_ball.x>l4_pot.x-3*l4_radius_pot/4) && (l4_ball.x<l4_pot.x+3*l4_radius_pot/4) && (l4_ball.y>l4_pot.y-3*l4_radius_pot/4) && (l4_ball.y<l4_pot.y+3*l4_radius_pot/4))
    {
        l4_ball = l4_pot;
        l4_speed.x = 0;
        l4_speed.y= 0;
        l4_game_state = 1;
    }
}


//yellow and black bands, across the long side
void l4_draw_hazard_stripes(Rectangle rec)
{
    DrawRectangleRec(rec,l4_hazard_yellow);
    float band = 6*l4_u;
    if (rec.width>=rec.height)
    {
        int bands = rec.width/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x+i*band,rec.y,band,rec.height,BLACK);
        }
    }
    else
    {
        int bands = rec.height/band;
        for (int i=0; i<bands; i++)
        {
            if (i%2==1) DrawRectangle(rec.x,rec.y+i*band,rec.width,band,BLACK);
        }
    }
}

//a picture that repeats (made at 2x size), lined up with the screen so pieces join without seams
void l4_draw_tiled(Texture2D texture, Rectangle rec, Color tint)
{
    Rectangle source = {rec.x/l4_u*2,rec.y/l4_u*2,rec.width/l4_u*2,rec.height/l4_u*2};
    Vector2 no_origin = {0,0};
    DrawTexturePro(texture,source,rec,no_origin,0,tint);
}


//foam along the 4 edges of a piece of ground, the bumpy side faces the water
void l4_draw_foam(Rectangle rec, Color tint)
{
    int frame = (int)(l4_animation_time*8)%4;
    float long_side = rec.width + 40*l4_u;
    Rectangle source = {rec.x/l4_u*2,frame*128+1,long_side/l4_u*2,126};
    Vector2 origin = {long_side/2,32*l4_u};

    //top and bottom edges (top one turned around)
    Rectangle top = {rec.x+rec.width/2,rec.y-18*l4_u,long_side,64*l4_u};
    DrawTexturePro(l4_foam_texture,source,top,origin,180,tint);
    Rectangle bottom = {rec.x+rec.width/2,rec.y+rec.height+18*l4_u,long_side,64*l4_u};
    DrawTexturePro(l4_foam_texture,source,bottom,origin,0,tint);

    //left and right edges
    long_side = rec.height + 40*l4_u;
    source.x = rec.y/l4_u*2;
    source.width = long_side/l4_u*2;
    origin.x = long_side/2;
    Rectangle left = {rec.x-18*l4_u,rec.y+rec.height/2,long_side,64*l4_u};
    DrawTexturePro(l4_foam_texture,source,left,origin,90,tint);
    Rectangle right = {rec.x+rec.width+18*l4_u,rec.y+rec.height/2,long_side,64*l4_u};
    DrawTexturePro(l4_foam_texture,source,right,origin,-90,tint);
}


void l4_draw_ground()
{
    float t = l4_animation_time;
    Vector2 no_origin = {0,0};

    //jungle floor everywhere, temple floor inside the temple
    Rectangle everything = {0,0,l4_width,l4_height};
    l4_draw_tiled(l4_ground_texture,everything,WHITE);

    //l4_river: the beach water tinted green, foam along both banks
    int water_frame = (int)(t*2)%2;
    Rectangle water_source = {water_frame*512,0,512,512};
    BeginScissorMode(l4_river.x,l4_river.y,l4_river.width,l4_river.height);
    int rows = l4_river.height/(256*l4_u) + 1;
    for (int j=0; j<rows; j++)
    {
        Rectangle tile = {l4_river.x,l4_river.y+j*256*l4_u,256*l4_u,256*l4_u};
        DrawTexturePro(l4_water_texture,water_source,tile,no_origin,0,l4_river_tint);
    }
    EndScissorMode();
    Rectangle left_bank = l4_make_rect(28,98,592,954);
    Rectangle right_bank = l4_make_rect(760,98,392,954);
    l4_draw_foam(left_bank,Fade(WHITE,0.8));
    l4_draw_foam(right_bank,Fade(WHITE,0.8));
    l4_draw_tiled(l4_temple_texture,l4_temple_area,WHITE);

    //l4_mud, with ripples
    l4_draw_tiled(l4_ground_texture,l4_mud,GetColor(0x8C6440FF));
    for (int j=1; j<l4_mud.height/(16*l4_u); j++)
    {
        Vector2 a = {l4_mud.x+10*l4_u,l4_mud.y+j*16*l4_u};
        Vector2 b = {l4_mud.x+l4_mud.width-10*l4_u,a.y+3*l4_u};
        DrawLineEx(a,b,2*l4_u,Fade(BLACK,0.18));
    }
    DrawRectangleLinesEx(l4_mud,2*l4_u,Fade(BLACK,0.25));

    //l4_moss, green and shiny
    for (int i=0; i<2; i++)
    {
        DrawRectangleRec(l4_moss[i],Fade(l4_moss_green,0.55));
        for (int k=0; k<12; k++)
        {
            float shine_x = l4_moss[i].x + fmod(k*47*l4_u,l4_moss[i].width);
            float shine_y = l4_moss[i].y + fmod(k*31*l4_u + 9*l4_u,l4_moss[i].height);
            DrawCircle(shine_x,shine_y,2.5*l4_u,Fade(WHITE,0.2+0.2*sin(t*3+k)));
        }
    }

    //l4_quicksand, slowly turning
    for (int i=0; i<2; i++)
    {
        Rectangle q = l4_quicksand[i];
        Vector2 middle = {q.x+q.width/2,q.y+q.height/2};
        DrawRectangleRec(q,GetColor(0xC8A56EFF));
        for (int j=0; j<4; j++)
        {
            float ring = 12*l4_u + j*16*l4_u;
            float turn = t*40;
            if (j%2==1) turn = -t*40;
            DrawRing(middle,ring,ring+3*l4_u,turn+j*50,turn+j*50+220,24,Fade(GetColor(0x8A6A3AFF),0.6));
        }
        DrawRectangleLinesEx(q,3*l4_u,GetColor(0x8A6A3AFF));
    }

    //l4_plank bridges with rope rails: cracked picture while cracking, gone for 5 seconds
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            float timer = l4_plank_timer[b][i];
            if (timer>=l4_crack_time[b]) continue;
            Rectangle p = l4_plank[b][i];
            int frame = 0;
            float shake = 0;
            if (timer>0)
            {
                frame = 1;
                shake = sin(t*60)*2*l4_u;
            }
            Rectangle source = {frame*256+8,12,240,56};
            Rectangle dest = {p.x+p.width/2+shake,p.y+p.height/2,p.height,p.width-2*l4_u};
            Vector2 origin = {p.height/2,(p.width-2*l4_u)/2};
            DrawTexturePro(l4_plank_texture,source,dest,origin,90,WHITE);
        }
        Vector2 rope1 = {l4_river.x-6*l4_u,l4_plank[b][0].y};
        Vector2 rope2 = {l4_river.x+l4_river.width+6*l4_u,l4_plank[b][0].y};
        DrawLineEx(rope1,rope2,3*l4_u,GetColor(0xC9A66BFF));
        rope1.y = rope1.y + l4_plank[b][0].height;
        rope2.y = rope2.y + l4_plank[b][0].height;
        DrawLineEx(rope1,rope2,3*l4_u,GetColor(0xC9A66BFF));
    }

    //pressure plates, l4_gold when pressed (the gate l4_plate shows how much time is left)
    for (int i=0; i<3; i++)
    {
        int down = l4_plate_down[i];
        if (i==0 && l4_gate_timer>0) down = 1;
        DrawCircle(l4_plate[i].x,l4_plate[i].y,l4_plate_radius+4*l4_u,l4_stone_mortar);
        if (down==1)
        {
            DrawCircleGradient(l4_plate[i],l4_plate_radius*2,Fade(l4_gold,0.35),BLANK);
            DrawCircle(l4_plate[i].x,l4_plate[i].y,l4_plate_radius,l4_gold);
            DrawRing(l4_plate[i],l4_plate_radius*0.45,l4_plate_radius*0.6,0,360,24,l4_gold_light);
        }
        else
        {
            DrawCircle(l4_plate[i].x,l4_plate[i].y,l4_plate_radius,l4_stone);
            DrawRing(l4_plate[i],l4_plate_radius*0.45,l4_plate_radius*0.6,0,360,24,l4_stone_dark);
        }
        if (i==0 && l4_gate_timer>0) DrawRing(l4_plate[i],l4_plate_radius+6*l4_u,l4_plate_radius+10*l4_u,-90,-90+l4_gate_timer/6*360,32,l4_gold_light);
    }
}


//l4_stone doors (drawn before the l4_walls, so the part that slid into the wall is hidden)
void l4_draw_doors()
{
    for (int i=0; i<2; i++)
    {
        DrawRectangleRec(l4_door[i],l4_stone_dark);
        DrawRectangleLinesEx(l4_door[i],3*l4_u,l4_gold);
        DrawCircle(l4_door[i].x+l4_door[i].width/2,l4_door[i].y+l4_door[i].height/2,6*l4_u,l4_gold);
    }
}


//mossy l4_stone l4_walls (the level 1 l4_brick l4_walls)
void l4_draw_walls()
{
    float brick_height = 14*l4_u;
    float brick_length = 28*l4_u;
    for (int i=0; i<13; i++)
    {
        Rectangle rec = l4_walls[i];

        //jungle hedges are leaves, not l4_stone
        if (i>=11)
        {
            DrawRectangleRec(rec,l4_jungle_dark);
            DrawRectangleLinesEx(rec,3*l4_u,Fade(BLACK,0.3));
            continue;
        }
        DrawRectangleRec(rec,l4_stone_mortar);

        //bricks, every second row moved by half a l4_brick
        int rows = rec.height/brick_height + 1;
        for (int row=0; row<rows; row++)
        {
            float y = rec.y + row*brick_height;
            float bottom = y + brick_height;
            if (bottom>rec.y+rec.height) bottom = rec.y + rec.height;
            float x = rec.x;
            if (row%2==1) x = rec.x - brick_length/2;
            int count = 0;
            while (x < rec.x+rec.width)
            {
                float left = x;
                float right = x + brick_length;
                if (left<rec.x) left = rec.x;
                if (right>rec.x+rec.width) right = rec.x + rec.width;
                if (right-left>3*l4_u && bottom-y>3*l4_u)
                {
                    Color colour = l4_stone;
                    if ((row*3 + count*7)%5==0) colour = l4_stone_dark;
                    DrawRectangle(left+1*l4_u,y+1*l4_u,right-left-2*l4_u,bottom-y-2*l4_u,colour);
                    DrawRectangle(left+1*l4_u,bottom-3*l4_u,right-left-2*l4_u,2*l4_u,Fade(BLACK,0.25));
                }
                x = x + brick_length;
                count++;
            }
        }

        //dark edge and l4_moss
        DrawRectangleLinesEx(rec,3*l4_u,Fade(BLACK,0.35));
        if (rec.height>rec.width)
        {
            int rivets = rec.height/(40*l4_u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+5*l4_u,rec.y+20*l4_u+j*40*l4_u,2.5*l4_u,l4_moss_green);
                DrawCircle(rec.x+rec.width-5*l4_u,rec.y+20*l4_u+j*40*l4_u,2.5*l4_u,l4_moss_green);
            }
        }
        else
        {
            int rivets = rec.width/(40*l4_u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+20*l4_u+j*40*l4_u,rec.y+5*l4_u,2.5*l4_u,l4_moss_green);
                DrawCircle(rec.x+20*l4_u+j*40*l4_u,rec.y+rec.height-5*l4_u,2.5*l4_u,l4_moss_green);
            }
        }
    }
}


void l4_draw_darts()
{
    float t = l4_animation_time;

    //dart traps: l4_stone heads at both ends, eyes blink red before firing, darts fly while on
    for (int i=0; i<4; i++)
    {
        Rectangle d = l4_dart_gate[i];
        int across = 0;
        if (d.width>d.height) across = 1;
        Vector2 head1 = {d.x+d.width/2,d.y-10*l4_u};
        Vector2 head2 = {d.x+d.width/2,d.y+d.height+10*l4_u};
        if (across==1)
        {
            head1.x = d.x-10*l4_u;
            head1.y = d.y+d.height/2;
            head2.x = d.x+d.width+10*l4_u;
            head2.y = d.y+d.height/2;
        }
        Color eye = Fade(BLACK,0.7);
        if (l4_dart_clock[i]>=1.2 && l4_dart_clock[i]<1.6 && fmod(t*10,2)<1) eye = RED;
        if (l4_dart_clock[i]>=1.6) eye = RED;
        DrawCircle(head1.x,head1.y,13*l4_u,l4_stone_dark);
        DrawCircle(head2.x,head2.y,13*l4_u,l4_stone_dark);
        DrawCircleLines(head1.x,head1.y,13*l4_u,l4_gold);
        DrawCircleLines(head2.x,head2.y,13*l4_u,l4_gold);
        DrawCircle(head1.x,head1.y,4*l4_u,eye);
        DrawCircle(head2.x,head2.y,4*l4_u,eye);

        if (l4_dart_clock[i]>=1.6)
        {
            DrawRectangleRec(d,Fade(RED,0.15));
            float length = d.height;
            if (across==1) length = d.width;
            for (int k=0; k<4; k++)
            {
                float along = fmod(t*600*l4_u + k*length/4,length);
                Vector2 tip = {d.x+d.width/2,d.y+along};
                Vector2 tail = {tip.x,tip.y-14*l4_u};
                if (across==1)
                {
                    tip.x = d.x+along;
                    tip.y = d.y+d.height/2;
                    tail.x = tip.x-14*l4_u;
                    tail.y = tip.y;
                }
                DrawLineEx(tail,tip,2*l4_u,l4_wood_brown);
                DrawCircle(tail.x,tail.y,3*l4_u,RED);
            }
        }
    }
}


//one jungle plant clump, picture and size picked from k
void l4_draw_clump(float x, float y, int k)
{
    Rectangle source = {(k%3)*320,0,320,320};
    float size = (60 + (k*37)%25)*l4_u;
    Rectangle dest = {x,y,size,size};
    Vector2 origin = {size/2,size/2};
    DrawTexturePro(l4_foliage_texture,source,dest,origin,(k*53)%360,WHITE);
}


void l4_draw_foliage()
{
    int k = 0;

    //along the hedges
    for (int i=11; i<13; i++)
    {
        int clumps = l4_walls[i].width/(70*l4_u);
        for (int j=0; j<=clumps; j++)
        {
            l4_draw_clump(l4_walls[i].x+j*70*l4_u,l4_walls[i].y+l4_walls[i].height/2,k);
            k++;
        }
    }

    //along the l4_river banks and the temple's outer wall, not on the bridges or the gate
    for (int j=0; j<11; j++)
    {
        float y = (130 + j*92)*l4_u;
        int near_bridge = 0;
        for (int b=0; b<2; b++)
        {
            if (y>l4_plank[b][0].y-40*l4_u && y<l4_plank[b][0].y+l4_plank[b][0].height+40*l4_u) near_bridge = 1;
        }
        if (near_bridge==0)
        {
            l4_draw_clump(606*l4_u,y,k);
            l4_draw_clump(774*l4_u,y+40*l4_u,k+1);
        }
        if (y<l4_door_closed_y[0]-50*l4_u || y>l4_door_closed_y[0]+150*l4_u) l4_draw_clump(1150*l4_u,y,k+2);
        k = k + 3;
    }
}


void l4_draw_obstacles()
{
    float t = l4_animation_time;

    //golden altar under the l4_pot
    DrawCircleGradient(l4_pot,110*l4_u,Fade(l4_gold,0.35),BLANK);
    DrawRing(l4_pot,34*l4_u,40*l4_u,0,360,48,l4_gold);
    DrawRing(l4_pot,48*l4_u,50*l4_u,t*30,t*30+300,48,Fade(l4_gold_light,0.7));
    for (int j=0; j<6; j++)
    {
        Vector2 arm = {60*l4_u,0};
        Vector2 sparkle = Vector2Add(l4_pot,Vector2Rotate(arm,(t*50+j*60)*DEG2RAD));
        DrawCircle(sparkle.x,sparkle.y,(2+sin(t*5+j))*l4_u,l4_gold_light);
    }

    //boulders (the big asteroid picture, tinted brown), rolling
    for (int i=0; i<3; i++)
    {
        float size = l4_boulder_radius[i]*2.4;
        Rectangle source = {0,0,192,192};
        Rectangle dest = {l4_boulder[i].x,l4_boulder[i].y,size,size};
        Vector2 origin = {size/2,size/2};
        DrawCircle(l4_boulder[i].x+5*l4_u,l4_boulder[i].y+7*l4_u,l4_boulder_radius[i],Fade(BLACK,0.3));
        DrawTexturePro(l4_boulder_texture,source,dest,origin,l4_boulder_t[i]*360,GetColor(0xC89A6AFF));
    }

    //mushrooms, they get bigger for a moment when hit
    for (int i=0; i<6; i++)
    {
        Vector2 m = l4_bumper[i];
        float r = l4_bumper_radius*(1 + l4_bumper_hit[i]*1.5);
        DrawCircle(m.x+3*l4_u,m.y+5*l4_u,r,Fade(BLACK,0.3));
        DrawCircle(m.x,m.y,r,GetColor(0xC0392BFF));
        DrawCircle(m.x-6*l4_u,m.y-6*l4_u,r*0.3,WHITE);
        DrawCircle(m.x+8*l4_u,m.y+2*l4_u,r*0.2,WHITE);
        DrawCircle(m.x-2*l4_u,m.y+9*l4_u,r*0.15,WHITE);
        DrawCircleLines(m.x,m.y,r,GetColor(0x5E1A12FF));
    }

    //spinning totem (the level 1 l4_fan, with wooden arms)
    Rectangle blade = {l4_fan.x,l4_fan.y,l4_fan_blade_length,l4_fan_blade_thickness};
    Vector2 blade_origin = {l4_fan_blade_length/2,l4_fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,l4_fan_angle-24,Fade(l4_wood_brown,0.12));
    DrawRectanglePro(blade,blade_origin,l4_fan_angle+90-24,Fade(l4_wood_brown,0.12));
    DrawRectanglePro(blade,blade_origin,l4_fan_angle-12,Fade(l4_wood_brown,0.25));
    DrawRectanglePro(blade,blade_origin,l4_fan_angle+90-12,Fade(l4_wood_brown,0.25));
    DrawRectanglePro(blade,blade_origin,l4_fan_angle,l4_wood_brown);
    DrawRectanglePro(blade,blade_origin,l4_fan_angle+90,l4_wood_brown);
    DrawCircle(l4_fan.x,l4_fan.y,l4_fan_hub_radius,l4_steel_dark);
    DrawCircleLines(l4_fan.x,l4_fan.y,l4_fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*l4_u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(l4_fan,Vector2Rotate(bolt_arm,(l4_fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*l4_u,l4_gold);
    }

    //splash, 5 pictures in half a second
    if (l4_splash_timer>0)
    {
        int frame = (0.5-l4_splash_timer)/0.1;
        if (frame>4) frame = 4;
        Rectangle source = {frame*192,0,192,192};
        Rectangle dest = {l4_splash_position.x,l4_splash_position.y,96*l4_u,96*l4_u};
        Vector2 origin = {48*l4_u,48*l4_u};
        DrawTexturePro(l4_splash_texture,source,dest,origin,0,l4_river_tint);
    }
}


void l4_draw_ball_and_pot()
{
    //start l4_plate
    Rectangle l4_plate = {l4_start_position.x-50*l4_u,l4_start_position.y-30*l4_u,100*l4_u,60*l4_u};
    DrawRectangleRec(l4_plate,l4_steel);
    DrawRectangleLinesEx(l4_plate,3*l4_u,l4_steel_dark);
    Rectangle plate_band = {l4_plate.x,l4_plate.y+l4_plate.height-10*l4_u,l4_plate.width,10*l4_u};
    l4_draw_hazard_stripes(plate_band);
    DrawText("START",l4_start_position.x-MeasureText("START",18*l4_u)/2,l4_plate.y+5*l4_u,18*l4_u,WHITE);

    //l4_pot with hazard ring
    for (int i=0; i<8; i++)
    {
        Color colour = l4_hazard_yellow;
        if (i%2==1) colour = BLACK;
        DrawRing(l4_pot,l4_radius_pot+6*l4_u,l4_radius_pot+12*l4_u,i*45,i*45+45,6,colour);
    }
    DrawCircle(l4_pot.x,l4_pot.y,l4_radius_pot+6*l4_u,l4_steel_light);
    DrawRing(l4_pot,l4_radius_pot+1*l4_u,l4_radius_pot+5*l4_u,0,360,24,l4_steel);
    DrawCircle(l4_pot.x,l4_pot.y,l4_radius_pot,BLACK);

    //flag
    Vector2 pole_top = {l4_pot.x,l4_pot.y-60*l4_u};
    DrawLineEx(l4_pot,pole_top,3*l4_u,l4_steel_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(l4_animation_time*6 - i*0.8)*2.5*l4_u;
        DrawRectangle(l4_pot.x+1*l4_u+i*6*l4_u,pole_top.y+wave,6*l4_u,18*l4_u,l4_laser_red);
    }

    //l4_ball (it shrinks while sinking in l4_quicksand)
    float ball_size = l4_radius_ball*(1 - l4_sink_timer/4);
    DrawCircle(l4_ball.x+3*l4_u,l4_ball.y+4*l4_u,ball_size,Fade(BLACK,0.45));
    DrawCircle(l4_ball.x,l4_ball.y,ball_size,GetColor(0xEDEDEDFF));
    DrawCircleLines(l4_ball.x,l4_ball.y,ball_size,GRAY);
    DrawCircle(l4_ball.x-2*l4_u,l4_ball.y-2*l4_u,2*l4_u,WHITE);

    //aim line
    if (l4_aiming==1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        DrawLineEx(l4_ball,mouse,4*l4_u,Fade(WHITE,0.75));
        DrawCircle(mouse.x,mouse.y,5*l4_u,Fade(WHITE,0.75));
    }
}


void l4_draw_hud()
{
    //dark edges
    DrawRectangleGradientH(0,l4_hud_height,90*l4_u,l4_height-l4_hud_height,Fade(BLACK,0.45),BLANK);
    DrawRectangleGradientH(l4_width-90*l4_u,l4_hud_height,90*l4_u,l4_height-l4_hud_height,BLANK,Fade(BLACK,0.45));
    DrawRectangleGradientV(0,l4_height-70*l4_u,l4_width,70*l4_u,BLANK,Fade(BLACK,0.45));

    //l4_steel bar
    DrawRectangleGradientV(0,0,l4_width,l4_hud_height,l4_steel_light,l4_steel_dark);
    DrawRectangle(0,l4_hud_height-4*l4_u,l4_width,4*l4_u,BLACK);
    int rivets = l4_width/(40*l4_u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*l4_u+i*40*l4_u,8*l4_u,3*l4_u,l4_steel_dark);
        DrawCircle(20*l4_u+i*40*l4_u,l4_hud_height-12*l4_u,3*l4_u,l4_steel_dark);
    }

    //text
    DrawText("LEVEL 4 - LOST TEMPLE",32*l4_u,20*l4_u,36*l4_u,BLACK);
    DrawText("LEVEL 4 - LOST TEMPLE",30*l4_u,18*l4_u,36*l4_u,l4_hazard_yellow);
    DrawText(TextFormat("STROKES %d / %d",l4_stroke,l4_stroke_limit),l4_width/2-298*l4_u,21*l4_u,32*l4_u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",l4_stroke,l4_stroke_limit),l4_width/2-300*l4_u,19*l4_u,32*l4_u,WHITE);
    for (int i=0; i<l4_stroke_limit; i++)
    {
        Color pip = l4_steel_dark;
        if (i<l4_stroke) pip = l4_laser_red;
        DrawCircle(l4_width/2+10*l4_u+i*22*l4_u,35*l4_u,7*l4_u,pip);
        DrawCircleLines(l4_width/2+10*l4_u+i*22*l4_u,35*l4_u,7*l4_u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC menu",26*l4_u);
    DrawText("R restart   ESC menu",l4_width-help_width-28*l4_u,24*l4_u,26*l4_u,BLACK);
    DrawText("R restart   ESC menu",l4_width-help_width-30*l4_u,22*l4_u,26*l4_u,WHITE);

    //chomp, darted or sunk message
    if (l4_message_timer>0)
    {
        if (l4_message_type==1)
        {
            DrawText("CHOMP!",l4_width/2-MeasureText("CHOMP!",90*l4_u)/2+4*l4_u,l4_height/2-41*l4_u,90*l4_u,Fade(BLACK,l4_message_timer));
            DrawText("CHOMP!",l4_width/2-MeasureText("CHOMP!",90*l4_u)/2,l4_height/2-45*l4_u,90*l4_u,Fade(GREEN,l4_message_timer));
        }
        else if (l4_message_type==2)
        {
            DrawText("DARTED!",l4_width/2-MeasureText("DARTED!",90*l4_u)/2+4*l4_u,l4_height/2-41*l4_u,90*l4_u,Fade(BLACK,l4_message_timer));
            DrawText("DARTED!",l4_width/2-MeasureText("DARTED!",90*l4_u)/2,l4_height/2-45*l4_u,90*l4_u,Fade(l4_laser_red,l4_message_timer));
        }
        else
        {
            DrawText("SUNK!",l4_width/2-MeasureText("SUNK!",90*l4_u)/2+4*l4_u,l4_height/2-41*l4_u,90*l4_u,Fade(BLACK,l4_message_timer));
            DrawText("SUNK!",l4_width/2-MeasureText("SUNK!",90*l4_u)/2,l4_height/2-45*l4_u,90*l4_u,Fade(l4_gold,l4_message_timer));
        }
    }

    //level clear or failed
    if (l4_game_state!=0)
    {
        DrawRectangle(0,0,l4_width,l4_height,Fade(BLACK,0.6));
        Rectangle panel = {l4_width/2-340*l4_u,l4_height/2-170*l4_u,680*l4_u,340*l4_u};
        DrawRectangleRec(panel,l4_steel_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*l4_u};
        l4_draw_hazard_stripes(panel_band);
        DrawRectangleLinesEx(panel,6*l4_u,l4_steel_light);
        if (l4_game_state==1)
        {
            DrawText("LEVEL CLEAR!",l4_width/2-MeasureText("LEVEL CLEAR!",80*l4_u)/2,panel.y+60*l4_u,80*l4_u,l4_hazard_yellow);
        }
        else
        {
            DrawText("FAILED",l4_width/2-MeasureText("FAILED",80*l4_u)/2,panel.y+60*l4_u,80*l4_u,l4_laser_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",l4_stroke,l4_stroke_limit),40*l4_u);
        DrawText(TextFormat("Strokes: %d / %d",l4_stroke,l4_stroke_limit),l4_width/2-strokes_width/2,panel.y+170*l4_u,40*l4_u,WHITE);
        DrawText("R play again   ESC menu",l4_width/2-MeasureText("R play again   ESC menu",30*l4_u)/2,panel.y+250*l4_u,30*l4_u,l4_steel_light);
    }
}


//level 4: keep the obstacles moving with no ball (intro and menu)
void l4_background_step(float dt)
{
    l4_animation_time = l4_animation_time + dt;
    l4_update_obstacles(dt);
}


//level 4: draw everything except the scoreboard (intro and menu)
void l4_draw_scene()
{

    l4_draw_ground();
    l4_draw_doors();
    l4_draw_walls();
    l4_draw_darts();
    l4_draw_foliage();
    l4_draw_obstacles();
    l4_draw_ball_and_pot();

}


//level 4: set the screen size, load pictures, start the level
void l4_start(int screen_width, int screen_height)
{
    l4_width = screen_width;
    l4_height = screen_height;

    //pictures (made at 2x size; water, foam and splash come from the beach level, boulders from space)
    l4_ground_texture = LoadTexture("assets/jungle/jungle_ground.png");
    l4_temple_texture = LoadTexture("assets/jungle/jungle_temple_floor.png");
    l4_foliage_texture = LoadTexture("assets/jungle/jungle_foliage.png");
    l4_plank_texture = LoadTexture("assets/jungle/jungle_plank.png");
    l4_water_texture = LoadTexture("assets/beach/beach_water_tile.png");
    l4_foam_texture = LoadTexture("assets/beach/beach_foam_strip.png");
    l4_splash_texture = LoadTexture("assets/beach/beach_splash.png");
    l4_boulder_texture = LoadTexture("assets/space/space_asteroid.png");
    SetTextureWrap(l4_ground_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(l4_temple_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(l4_foam_texture,TEXTURE_WRAP_REPEAT);
    SetTextureFilter(l4_ground_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_temple_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_foliage_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_plank_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_water_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_foam_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_splash_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(l4_boulder_texture,TEXTURE_FILTER_BILINEAR);

    //every size is N*l4_u, so it looks the same on any screen
    l4_u = l4_height/1080.0;
    if (l4_width/1920.0 < l4_u) l4_u = l4_width/1920.0;
    l4_reset_level();

}


//level 4: one frame of input and movement
void l4_update()
{
    float dt = GetFrameTime();
    if (dt>1.0/30) dt = 1.0/30;
    l4_animation_time = l4_animation_time + dt;
    if (l4_message_timer>0) l4_message_timer = l4_message_timer - dt;

    //restart
    if (IsKeyPressed(KEY_R)) l4_reset_level();

    //shooting (the click has to start while the l4_ball is still)
    int ball_stopped = 0;
    if (l4_speed.x==0 && l4_speed.y==0) ball_stopped = 1;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && l4_game_state==0) l4_aiming = 1;
    if (l4_game_state!=0) l4_aiming = 0;
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && l4_aiming==1)
    {
        l4_aiming = 0;
        Vector2 mouse = {GetMouseX(),GetMouseY()};
        Vector2 drag = Vector2Subtract(l4_ball,mouse);
        if (ball_stopped==1 && l4_stroke<l4_stroke_limit && Vector2Length(drag)>=2*l4_radius_ball)
        {
            l4_last_shot_position = l4_ball;
            l4_speed = Vector2ClampValue(Vector2Scale(drag,3.125),0,l4_max_speed*l4_u);
            l4_stroke++;
        }
    }

    //moving everything in 4 small steps, so nothing jumps through anything
    for (int i=0; i<4; i++)
    {
        l4_update_obstacles(dt/4*obstacle_speed);
        if (l4_game_state==0) l4_update_ball(dt/4);
    }

    //out of strokes
    if (l4_game_state==0 && l4_stroke>=l4_stroke_limit && l4_speed.x==0 && l4_speed.y==0) l4_game_state = 2;


}


//level 4: one frame of drawing
void l4_draw()
{

    l4_draw_ground();
    l4_draw_doors();
    l4_draw_walls();
    l4_draw_darts();
    l4_draw_foliage();
    l4_draw_obstacles();
    l4_draw_ball_and_pot();
    draw_straight_preview(l4_ball,l4_aiming,l4_radius_ball,l4_max_speed*l4_u,l4_u);
    l4_draw_hud();

}


void l4_unload()
{
    UnloadTexture(l4_ground_texture);
    UnloadTexture(l4_temple_texture);
    UnloadTexture(l4_foliage_texture);
    UnloadTexture(l4_plank_texture);
    UnloadTexture(l4_water_texture);
    UnloadTexture(l4_foam_texture);
    UnloadTexture(l4_splash_texture);
    UnloadTexture(l4_boulder_texture);
}



//==================== THE GAME: INTRO, MENU, DIFFICULTY ====================
int screen_width;
int screen_height;
float su = 1;
int game_mode = 0;          //0 intro, 1 menu, 2 playing, 3 briefing, 4 name, 5 how to play, 6 leaderboard, 7 credits
int level = 1;
int quit = 0;
int brief_page = 0;
int current_difficulty = 1;

//intro: the ball mascot hops through the 4 worlds (1.6s each), then the title drops in
Texture2D mascot_texture;
Texture2D developer_photo[2];     //the two faces on the credits page
float intro_time = 0;

//menu: 4 live level cards, then bot / chad / goat
RenderTexture2D card_texture[4];
int card_turn = 0;
int chosen_level = 0;       //0 picking a level, 1-4 picking the difficulty for that level
Color card_colour[4] = {{232,185,35,255},{63,180,207,255},{190,90,255,255},{120,200,90,255}};
const char *level_name[4] = {"FOUNDRY","SHORELINE","EVENT HORIZON","LOST TEMPLE"};


//==================== THE PLAYER, THE SCORES AND THE SETTINGS ====================
//scores.txt sits next to the game: one line per player, NAME|level1|level2|level3|level4
//a level only writes a score when it is finished, and only when the score beats the old one
#define MAX_PLAYERS 50
char player_name[16] = "";
int name_length = 0;
int player_row = -1;              //which line of the table is this player, -1 = no line yet

char score_name[MAX_PLAYERS][16];
int score_points[MAX_PLAYERS][4];
int score_count = 0;

int sound_on = 1;              //the one switch for music AND sound effects
int score_saved = 0;              //the score for this run has already been written
int ui_press = 0;                 //a button on top of a level is held down, so no shot

int score_rate[3] = {10,25,50};        //points for every stroke you did NOT use
int score_bonus[3] = {100,250,500};    //for finishing the level at all


//the same sum for every level: what is left over, times the difficulty, plus the bonus
int score_for(int limit, int strokes, int difficulty)
{
    int left = limit - strokes;
    if (left<0) left = 0;
    return left*score_rate[difficulty] + score_bonus[difficulty];
}


int player_total(int row)
{
    int total = 0;
    for (int i=0; i<4; i++) total = total + score_points[row][i];
    return total;
}


//the level's own numbers, whichever level is being played
int level_state()
{
    if (level==1) return l1_game_state;
    if (level==2) return l2_game_state;
    if (level==3) return l3_game_state;
    return l4_game_state;
}


int level_strokes()
{
    if (level==1) return l1_stroke;
    if (level==2) return l2_stroke;
    if (level==3) return l3_stroke;
    return l4_stroke;
}


int level_limit()
{
    if (level==1) return l1_stroke_limit;
    if (level==2) return l2_stroke_limit;
    if (level==3) return l3_stroke_limit;
    return l4_stroke_limit;
}


//one line out of the file
void read_score_line(const char *line)
{
    if (score_count>=MAX_PLAYERS) return;
    char name[16] = "";
    int points[4] = {0,0,0,0};
    int part = 0;
    int letters = 0;
    int number = 0;
    for (int i=0; ; i++)
    {
        char c = line[i];
        if (c=='|' || c==0)
        {
            if (part>0 && part<5) points[part-1] = number;
            number = 0;
            part++;
            if (c==0) break;
        }
        else if (part==0)
        {
            if (letters<15)
            {
                name[letters] = c;
                letters++;
                name[letters] = 0;
            }
        }
        else if (c>='0' && c<='9') number = number*10 + (c-'0');
    }
    if (letters==0 || part<5) return;        //not a line this game wrote
    TextCopy(score_name[score_count],name);
    for (int i=0; i<4; i++) score_points[score_count][i] = points[i];
    score_count++;
}


void load_scores()
{
    score_count = 0;
    if (FileExists("scores.txt")==0) return;
    char *text = LoadFileText("scores.txt");
    if (text==0) return;
    char line[64] = "";
    int letters = 0;
    for (int i=0; ; i++)
    {
        char c = text[i];
        if (c=='\n' || c=='\r' || c==0)
        {
            line[letters] = 0;
            if (letters>0) read_score_line(line);
            letters = 0;
            if (c==0) break;
        }
        else if (letters<63)
        {
            line[letters] = c;
            letters++;
        }
    }
    UnloadFileText(text);
}


void save_scores()
{
    char text[MAX_PLAYERS*48+2] = "";
    int at = 0;
    for (int i=0; i<score_count; i++)
    {
        TextAppend(text,TextFormat("%s|%d|%d|%d|%d\n",score_name[i],score_points[i][0],score_points[i][1],score_points[i][2],score_points[i][3]),&at);
    }
    SaveFileText("scores.txt",text);
}


int find_player(const char *name)
{
    for (int i=0; i<score_count; i++)
    {
        if (TextIsEqual(score_name[i],name)) return i;
    }
    return -1;
}


//typing a name that is already in the file picks that line up again
void pick_player(const char *name)
{
    player_row = find_player(name);
    if (player_row<0 && score_count<MAX_PLAYERS)
    {
        player_row = score_count;
        TextCopy(score_name[player_row],name);
        for (int i=0; i<4; i++) score_points[player_row][i] = 0;
        score_count++;
    }
}


int player_best(int n)
{
    if (player_row<0) return 0;
    return score_points[player_row][n-1];
}


//a worse run never wipes out a better one
void record_score(int n, int points)
{
    if (player_row<0) return;
    if (points<=score_points[player_row][n-1]) return;
    score_points[player_row][n-1] = points;
    save_scores();
}


//the mascot, standing on its feet at "feet" (outfit 0-3 = level 1-4)
void draw_mascot(Vector2 feet, int outfit, int jumping, float size, float squash)
{
    Rectangle source = {outfit*320,jumping*320,320,320};
    float w = size*(1+squash);
    float h = size*(1-squash);
    Rectangle dest = {feet.x,feet.y,w,h};
    Vector2 origin = {w/2,h*150/160};
    DrawTexturePro(mascot_texture,source,dest,origin,0,WHITE);
}


void background_step(int n, float dt)
{
    if (n==1) l1_background_step(dt);
    if (n==2) l2_background_step(dt);
    if (n==3) l3_background_step(dt);
    if (n==4) l4_background_step(dt);
}


void draw_scene(int n)
{
    if (n==1) l1_draw_scene();
    if (n==2) l2_draw_scene();
    if (n==3) l3_draw_scene();
    if (n==4) l4_draw_scene();
}


void draw_intro()
{
    float t = intro_time;
    int part = t/1.6;
    if (part>3) part = 3;
    draw_scene(part+1);
    DrawRectangle(0,0,screen_width,screen_height,Fade(BLACK,0.35));

    float ground = screen_height*0.72;
    float p = (t - part*1.6)/1.6;
    Vector2 feet;
    float hop = 0;

    if (part<3)
    {
        //run across the world in hops (one big slow hop in space)
        float hops = 2;
        float hop_height = 220*su;
        if (part==2)
        {
            hops = 1;
            hop_height = 380*su;
        }
        hop = fabsf(sin(p*hops*PI));
        feet.x = -150*su + p*(screen_width+300*su);
        if (part==1) feet.x = screen_width+150*su - p*(screen_width+300*su);     //second world: right to left
        feet.y = ground - hop_height*hop;
    }
    else
    {
        //last world: hop in from the right to the middle and land
        float landing = (t - 3*1.6)/0.8;
        if (landing>1) landing = 1;
        hop = sin(landing*PI);
        feet.x = screen_width+150*su - landing*(screen_width/2+150*su);
        feet.y = ground - 260*su*hop;
    }

    int jumping = 0;
    if (hop>0.15) jumping = 1;
    float squash = 0;
    if (hop<0.15) squash = (0.15-hop)*1.2;
    //after the last landing the squash springs back to normal in a fifth of a second
    float after_landing = t - 3*1.6 - 0.8;
    if (part==3 && after_landing>0)
    {
        squash = 0.18 - after_landing*0.9;
        if (squash<0) squash = 0;
    }
    DrawEllipse(feet.x,ground,60*su*(1-hop*0.5),14*su,Fade(BLACK,0.35));
    draw_mascot(feet,part,jumping,240*su,squash);

    //white flash when the world changes
    float since_change = t - part*1.6;
    if (part>0 && since_change<0.15) DrawRectangle(0,0,screen_width,screen_height,Fade(WHITE,1-since_change/0.15));

    //title slams in: starts huge and see-through, hits full size at 0.15s, then the screen shakes
    float title_time = t - 3*1.6 - 0.8;
    if (title_time>0)
    {
        float grow = 1;
        float see = 1;
        if (title_time<0.15)
        {
            float left = 1 - title_time/0.15;
            grow = 1 + 2.5*left*left;
            see = title_time/0.15;
        }
        float shake_x = 0;
        float shake_y = 0;
        float since_hit = title_time - 0.15;
        if (since_hit>0 && since_hit<0.35)
        {
            float strength = 18*su*(1 - since_hit/0.35);
            shake_x = sin(since_hit*90)*strength;
            shake_y = cos(since_hit*70)*strength;
        }
        if (since_hit>0 && since_hit<0.12) DrawRectangle(0,0,screen_width,screen_height,Fade(WHITE,0.5*(1-since_hit/0.12)));
        int size = 130*su*grow;
        int title_width = MeasureText("THE ULTIMATE GOLF",size);
        float title_x = screen_width/2 - title_width/2 + shake_x;
        float title_y = screen_height*0.26 - size/2 + 65*su + shake_y;
        DrawText("THE ULTIMATE GOLF",title_x+6*su,title_y+6*su,size,Fade(BLACK,see));
        DrawText("THE ULTIMATE GOLF",title_x,title_y,size,Fade(GetColor(0xFFD34DFF),see));
        if (title_time>1 && fmod(t*2,2)<1.4)
        {
            int click_width = MeasureText("CLICK TO PLAY",50*su);
            DrawText("CLICK TO PLAY",screen_width/2-click_width/2,screen_height*0.86,50*su,WHITE);
        }
    }
}


//where card i sits: 2 x 2 under the title
Rectangle card_rect(int i)
{
    float gap = 40*su;
    float top = 150*su;
    float card_w = (screen_width - 3*gap)/2;
    float card_h = (screen_height - top - 2*gap)/2;
    Rectangle rec = {gap + (i%2)*(card_w+gap), top + (i/2)*(card_h+gap), card_w, card_h - gap/2};
    return rec;
}


//where difficulty button d (0 bot, 1 chad, 2 goat) sits on the panel
Rectangle difficulty_rect(int d)
{
    float panel_x = screen_width/2 - 620*su;
    float panel_y = screen_height/2 - 270*su;
    Rectangle rec = {panel_x + 60*su + d*390*su, panel_y + 150*su, 340*su, 320*su};
    return rec;
}


//==================== THE KEY BUTTONS ====================
Rectangle brief_panel();
Rectangle menu_button(int b);
//every keyboard shortcut in the game is also a button: a key cap, then what it does.
//the rectangle is worked out from the text, so the drawing and the click test agree.
float key_button_width(const char *key, const char *what, float h)
{
    float s = h*0.42;
    return h*0.22 + MeasureText(key,s)+s*0.7 + s*0.6 + MeasureText(what,s) + h*0.3;
}


Rectangle key_button(float x, float y, const char *key, const char *what, float h)
{
    Rectangle rec = {x,y,key_button_width(key,what,h),h};
    return rec;
}


//the same button, but placed by its right hand edge
Rectangle key_button_right(float right, float y, const char *key, const char *what, float h)
{
    float w = key_button_width(key,what,h);
    Rectangle rec = {right-w,y,w,h};
    return rec;
}


void draw_key_button(Rectangle r, const char *key, const char *what, Color colour, int hover)
{
    float s = r.height*0.42;
    DrawRectangleRounded(r,0.3,8,Fade(BLACK,0.8));
    DrawRectangleRounded(r,0.3,8,hover==1 ? Fade(colour,0.35) : Fade(colour,0.12));
    DrawRectangleRoundedLinesEx(r,0.3,8,2*su,hover==1 ? colour : Fade(colour,0.75));
    Rectangle cap = {r.x+r.height*0.22,r.y+r.height/2-s*0.78,MeasureText(key,s)+s*0.7,s*1.56};
    DrawRectangleRounded(cap,0.35,6,Fade(colour,0.28));
    DrawRectangleRoundedLinesEx(cap,0.35,6,2*su,Fade(colour,0.85));
    DrawText(key,cap.x+s*0.35,cap.y+s*0.28,s,colour);
    DrawText(what,cap.x+cap.width+s*0.6,r.y+r.height/2-s/2,s,hover==1 ? WHITE : Fade(WHITE,0.85));
}


int over(Rectangle r)
{
    return CheckCollisionPointRec(GetMousePosition(),r);
}


//--- where each one sits ---
//the scoreboard pair: 0 restart, 1 menu
Rectangle hud_key(int which)
{
    float h = 46*su;
    Rectangle esc = key_button_right(screen_width-24*su,12*su,"ESC","menu",h);
    if (which==1) return esc;
    return key_button_right(esc.x-10*su,12*su,"R","restart",h);
}


//the pair on the level clear / failed panel: 0 play again, 1 menu
Rectangle end_key(int which)
{
    float h = 54*su;
    float total = key_button_width("R","play again",h)+key_button_width("ESC","menu",h)+16*su;
    float left = screen_width/2-total/2;
    Rectangle again = key_button(left,screen_height/2+66*su,"R","play again",h);
    if (which==0) return again;
    return key_button(again.x+again.width+16*su,screen_height/2+66*su,"ESC","menu",h);
}


Rectangle menu_quit_key()
{
    return key_button_right(menu_button(0).x-14*su,46*su,"ESC","quit",62*su);
}


Rectangle difficulty_back_key()
{
    float x = screen_width/2+620*su-40*su;
    float y = screen_height/2+270*su-64*su;
    return key_button_right(x,y,"ESC","back",52*su);
}


Rectangle brief_menu_key()
{
    Rectangle p = brief_panel();
    return key_button(p.x+p.width/2-200*su,p.y+p.height-95*su,"ESC","menu",58*su);
}


//the name screen: 0 start, 1 delete a letter
Rectangle name_key(int which)
{
    float y = 150*su+470*su;
    float x = screen_width/2-760*su+80*su;
    Rectangle start = key_button(x,y,"ENTER","start playing",70*su);
    if (which==0) return start;
    return key_button(start.x+start.width+20*su,y,"BKSP","delete a letter",70*su);
}


//start level n with the chosen difficulty
void start_level(int n, int difficulty)
{
    float stroke_factor = 1.1;
    obstacle_speed = 1;
    preview_seconds = 0.3;
    if (difficulty==0)
    {
        stroke_factor = 1.5;
        obstacle_speed = 0.7;
        preview_seconds = 0.6;
    }
    if (difficulty==2)
    {
        stroke_factor = 0.75;
        obstacle_speed = 1.3;
        preview_seconds = 0;
    }
    if (n==1)
    {
        l1_stroke_limit = l1_stroke_base*stroke_factor + 0.5;
        l1_reset_level();
    }
    if (n==2)
    {
        l2_stroke_limit = l2_stroke_base*stroke_factor + 0.5;
        l2_reset_level();
    }
    if (n==3)
    {
        l3_stroke_limit = l3_stroke_base*stroke_factor + 0.5;
        l3_reset_level();
    }
    if (n==4)
    {
        l4_stroke_limit = l4_stroke_base*stroke_factor + 0.5;
        l4_reset_level();
    }
    level = n;
    current_difficulty = difficulty;
    brief_page = 0;
    game_mode = 3;
}


void menu_step(float dt)
{
    //keep all 4 worlds moving, redraw one card picture each frame
    for (int n=1; n<=4; n++) background_step(n,dt);
    BeginTextureMode(card_texture[card_turn]);
    ClearBackground(BLACK);
    draw_scene(card_turn+1);
    EndTextureMode();
    card_turn = (card_turn+1)%4;

    Vector2 mouse = GetMousePosition();
    if (chosen_level==0)
    {
        for (int i=0; i<4; i++)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse,card_rect(i))) chosen_level = i+1;
        }
        if (IsKeyPressed(KEY_ESCAPE)) quit = 1;
    }
    else
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int clicked = -1;
            for (int d=0; d<3; d++)
            {
                if (CheckCollisionPointRec(mouse,difficulty_rect(d))) clicked = d;
            }
            if (clicked>=0)
            {
                start_level(chosen_level,clicked);
                chosen_level = 0;
            }
        }
        if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && over(difficulty_back_key()))) chosen_level = 0;
    }
}


//the four buttons along the top of the menu: 0 how to play, 1 leaderboard, 2 credits
Rectangle menu_button(int b)
{
    float w = 250*su;
    float h = 62*su;
    float gap = 14*su;
    float right = screen_width - 40*su - 76*su - gap;
    Rectangle rec = {right - (3-b)*(w+gap) + gap, 46*su, w, h};
    return rec;
}


Rectangle sound_button()
{
    Rectangle rec = {screen_width-40*su-76*su,46*su,76*su,64*su};
    return rec;
}


//the small one that sits under the scoreboard while a level is being played
Rectangle game_sound_button()
{
    Rectangle rec = {screen_width-76*su,90*su,52*su,44*su};
    return rec;
}


//back to the menu, bottom left of any page
Rectangle page_back_button()
{
    Rectangle rec = {screen_width/2-760*su+40*su,screen_height-120*su,340*su,64*su};
    return rec;
}


void draw_button(Rectangle r, const char *text, Color colour, int hover)
{
    if (hover==1) DrawRectangleRounded(r,0.25,8,Fade(colour,0.35));
    else DrawRectangleRounded(r,0.25,8,Fade(colour,0.12));
    DrawRectangleRoundedLinesEx(r,0.25,8,3*su,colour);
    int w = MeasureText(text,26*su);
    DrawText(text,r.x+r.width/2-w/2,r.y+r.height/2-13*su,26*su,hover==1 ? WHITE : colour);
}


//a speaker with two waves, or a cross when the music is off
void draw_speaker(Rectangle r, int on, int hover)
{
    float k = r.height/64;                      //so the same drawing works small, in the corner of a level
    Color colour = on==1 ? GetColor(0x6FE7FFFF) : GetColor(0xB9C0CCFF);
    DrawRectangleRounded(r,0.25,8,Fade(BLACK,0.6));       //a dark plate, so it reads over a bright level too
    if (hover==1) DrawRectangleRounded(r,0.25,8,Fade(colour,0.3));
    else DrawRectangleRounded(r,0.25,8,Fade(colour,0.12));
    DrawRectangleRoundedLinesEx(r,0.25,8,3*su,colour);
    float cx = r.x+r.width/2-10*k;
    float cy = r.y+r.height/2;
    DrawRectangle(cx-17*k,cy-8*k,11*k,16*k,colour);                                             //the box
    DrawTriangle((Vector2){cx+2*k,cy+18*k},(Vector2){cx+2*k,cy-18*k},(Vector2){cx-8*k,cy},colour);   //the cone
    DrawRectangle(cx-8*k,cy-8*k,12*k,16*k,colour);
    if (on==1)
    {
        DrawRing((Vector2){cx+2*k,cy},13*k,16*k,-55,55,20,colour);
        DrawRing((Vector2){cx+2*k,cy},21*k,24*k,-55,55,20,Fade(colour,0.65));
    }
    else
    {
        DrawLineEx((Vector2){cx+10*k,cy-11*k},(Vector2){cx+28*k,cy+11*k},4*k,colour);
        DrawLineEx((Vector2){cx+28*k,cy-11*k},(Vector2){cx+10*k,cy+11*k},4*k,colour);
    }
}


void draw_menu_buttons()
{
    Vector2 mouse = GetMousePosition();
    const char *name[3] = {"HOW TO PLAY","LEADERBOARD","CREDITS"};
    Color colour[3] = {{110,220,120,255},{255,211,77,255},{190,140,255,255}};
    for (int b=0; b<3; b++)
    {
        Rectangle r = menu_button(b);
        draw_button(r,name[b],colour[b],CheckCollisionPointRec(mouse,r));
    }
    Rectangle s = sound_button();
    draw_speaker(s,sound_on,CheckCollisionPointRec(mouse,s));
    Rectangle q = menu_quit_key();
    draw_key_button(q,"ESC","quit",GetColor(0xFF7A6EFF),CheckCollisionPointRec(mouse,q));
}


void draw_menu()
{
    float t = GetTime();
    Vector2 mouse = GetMousePosition();
    DrawRectangleGradientV(0,0,screen_width,screen_height,GetColor(0x131B2AFF),GetColor(0x05070DFF));

    DrawText("THE ULTIMATE GOLF",40*su+4*su,44*su+4*su,64*su,BLACK);
    DrawText("THE ULTIMATE GOLF",40*su,44*su,64*su,GetColor(0xFFD34DFF));
    if (player_row>=0) DrawText(TextFormat("player: %s      total %d",player_name,player_total(player_row)),44*su,124*su,28*su,GRAY);
    draw_menu_buttons();

    for (int i=0; i<4; i++)
    {
        Rectangle r = card_rect(i);
        int hover = 0;
        if (chosen_level==0 && CheckCollisionPointRec(mouse,r)) hover = 1;
        float grow = 0;
        if (hover==1) grow = 10*su;
        Rectangle d = {r.x-grow,r.y-grow,r.width+2*grow,r.height+2*grow};

        //glow, live picture (cut to the card's shape), name bar, border
        Rectangle glow = {d.x-8*su,d.y-8*su,d.width+16*su,d.height+16*su};
        if (hover==1) DrawRectangleRec(glow,Fade(card_colour[i],0.45));
        else DrawRectangleRec(glow,Fade(card_colour[i],0.12));
        float source_height = screen_width*d.height/d.width;
        Rectangle source = {0,(screen_height-source_height)/2,screen_width,-source_height};
        Vector2 no_origin = {0,0};
        DrawTexturePro(card_texture[i].texture,source,d,no_origin,0,WHITE);
        DrawRectangle(d.x,d.y+d.height-74*su,d.width,74*su,Fade(BLACK,0.7));
        DrawText(TextFormat("LEVEL %d  -  %s",i+1,level_name[i]),d.x+24*su,d.y+d.height-58*su,46*su,card_colour[i]);
        if (player_best(i+1)>0)
        {
            const char *best = TextFormat("BEST %d",player_best(i+1));
            float best_w = MeasureText(best,30*su)+36*su;
            Rectangle tag = {d.x+d.width-best_w-16*su,d.y+16*su,best_w,46*su};
            DrawRectangleRounded(tag,0.4,8,Fade(BLACK,0.7));
            DrawText(best,tag.x+18*su,tag.y+8*su,30*su,card_colour[i]);
        }
        if (hover==1) DrawRectangleLinesEx(d,5*su,card_colour[i]);
        else DrawRectangleLinesEx(d,3*su,Fade(card_colour[i],0.7));

        //the mascot in this world's outfit, jumping when you hover
        Vector2 feet = {d.x+d.width-100*su,d.y+d.height-10*su};
        if (hover==1) feet.y = feet.y - fabsf(sin(t*6))*30*su;
        draw_mascot(feet,i,hover,150*su,0);
    }

    //difficulty panel
    if (chosen_level>0)
    {
        int c = chosen_level-1;
        DrawRectangle(0,0,screen_width,screen_height,Fade(BLACK,0.7));
        Rectangle panel = {screen_width/2-620*su,screen_height/2-270*su,1240*su,540*su};
        DrawRectangleRounded(panel,0.08,8,GetColor(0x151C2BFF));
        DrawRectangleRoundedLinesEx(panel,0.08,8,4*su,card_colour[c]);
        DrawText(TextFormat("%s  -  how good are you?",level_name[c]),panel.x+60*su,panel.y+50*su,56*su,card_colour[c]);
        Rectangle back = difficulty_back_key();
        draw_key_button(back,"ESC","back",card_colour[c],CheckCollisionPointRec(mouse,back));

        const char *name[3] = {"BOT","CHAD","GOAT"};
        const char *line1[3] = {"easy","medium","hard"};
        const char *line2[3] = {"+50% strokes","+10% strokes","-25% strokes"};
        const char *line3[3] = {"slow obstacles","normal obstacles","fast obstacles"};
        const char *line4[3] = {"long aim line","medium aim line","no aim line"};
        Color colour[3] = {{110,220,120,255},{255,200,60,255},{240,80,70,255}};
        for (int d=0; d<3; d++)
        {
            Rectangle b = difficulty_rect(d);
            int hover = CheckCollisionPointRec(mouse,b);
            if (hover==1) DrawRectangleRounded(b,0.1,8,Fade(colour[d],0.3));
            else DrawRectangleRounded(b,0.1,8,Fade(colour[d],0.1));
            DrawRectangleRoundedLinesEx(b,0.1,8,3*su,colour[d]);
            int name_width = MeasureText(name[d],80*su);
            DrawText(name[d],b.x+b.width/2-name_width/2,b.y+30*su,80*su,colour[d]);
            DrawText(line1[d],b.x+30*su,b.y+140*su,36*su,WHITE);
            DrawText(line2[d],b.x+30*su,b.y+185*su,30*su,LIGHTGRAY);
            DrawText(line3[d],b.x+30*su,b.y+225*su,30*su,LIGHTGRAY);
            DrawText(line4[d],b.x+30*su,b.y+265*su,30*su,LIGHTGRAY);
        }

        Vector2 feet = {panel.x+panel.width-120*su,panel.y+10*su - fabsf(sin(t*5))*40*su};
        draw_mascot(feet,c,1,170*su,0);
    }
}



//==================== NAME, HOW TO PLAY, CREDITS, LEADERBOARD ====================
//the same dark page every one of these screens is drawn on
Rectangle draw_page(const char *title, int with_back)
{
    DrawRectangleGradientV(0,0,screen_width,screen_height,GetColor(0x131B2AFF),GetColor(0x05070DFF));
    DrawText(title,40*su+4*su,40*su+4*su,70*su,BLACK);
    DrawText(title,40*su,40*su,70*su,GetColor(0xFFD34DFF));
    Rectangle panel = {screen_width/2-760*su,150*su,1520*su,screen_height-290*su};
    DrawRectangleRounded(panel,0.03,8,GetColor(0x151C2BFF));
    DrawRectangleRoundedLinesEx(panel,0.03,8,3*su,GetColor(0x2A3550FF));
    Vector2 mouse = GetMousePosition();
    if (with_back==1)
    {
        Rectangle back = page_back_button();
        draw_key_button(back,"ESC","back to the menu",GetColor(0xFFD34DFF),CheckCollisionPointRec(mouse,back));
    }
    Rectangle s = sound_button();
    draw_speaker(s,sound_on,CheckCollisionPointRec(mouse,s));
    return panel;
}


void draw_name_entry()
{
    Rectangle p = draw_page("WHO IS PLAYING?",0);
    float t = GetTime();
    DrawText("type your name, then press ENTER",p.x+80*su,p.y+90*su,40*su,LIGHTGRAY);
    DrawText("the same name again picks up the scores you already have",p.x+80*su,p.y+150*su,30*su,GRAY);

    Rectangle box = {p.x+80*su,p.y+240*su,900*su,120*su};
    DrawRectangleRounded(box,0.15,8,GetColor(0x0B1220FF));
    DrawRectangleRoundedLinesEx(box,0.15,8,4*su,GetColor(0xFFD34DFF));
    DrawText(player_name,box.x+30*su,box.y+30*su,64*su,WHITE);
    if (fmod(t,1.0)<0.5) DrawRectangle(box.x+40*su+MeasureText(player_name,64*su),box.y+28*su,4*su,64*su,WHITE);
    DrawText(TextFormat("%d / 12 letters",name_length),box.x,box.y+140*su,28*su,GRAY);
    Vector2 mouse = GetMousePosition();
    draw_key_button(name_key(0),"ENTER","start playing",GetColor(0xFFD34DFF),over(name_key(0)) && name_length>0);
    draw_key_button(name_key(1),"BKSP","delete a letter",GetColor(0xB9C0CCFF),over(name_key(1)));
    if (mouse.x<0) return;

    draw_mascot((Vector2){p.x+p.width-260*su,p.y+p.height-80*su},1,1,320*su,0);
}


void draw_how_to_play()
{
    Rectangle p = draw_page("HOW TO PLAY",1);
    float t = GetTime();

    //--- the picture: a little course inside its own frame ---
    Rectangle shot = {p.x+60*su,p.y+50*su,p.width-120*su,250*su};
    DrawRectangleRounded(shot,0.06,8,GetColor(0x101A2CFF));
    DrawRectangleRoundedLinesEx(shot,0.06,8,2*su,GetColor(0x2A3550FF));

    float y = shot.y+150*su;
    DrawRectangle(shot.x+20*su,y+26*su,shot.width-40*su,6*su,GetColor(0x1E4D2BFF));    //the ground
    Vector2 ball = {shot.x+shot.width*0.30,y};
    Vector2 hole = {shot.x+shot.width*0.86,y};

    //the hole and its flag
    DrawEllipse(hole.x,hole.y+16*su,30*su,14*su,BLACK);
    DrawLineEx((Vector2){hole.x,hole.y+14*su},(Vector2){hole.x,hole.y-86*su},4*su,LIGHTGRAY);
    DrawTriangle((Vector2){hole.x,hole.y-86*su},(Vector2){hole.x,hole.y-38*su},(Vector2){hole.x+62*su,hole.y-62*su},RED);

    //the drag: a line of dots going back from the ball, and the shot line going forward
    float pull = 80*su+34*su*fabsf(sin(t*1.4));
    for (int i=1; i<=6; i++)
    {
        float q = (float)i/6;
        DrawCircleV((Vector2){ball.x-34*su-pull*q,y},5*su-q*2*su,Fade(WHITE,0.75-q*0.45));
    }
    DrawCircleV((Vector2){ball.x-34*su-pull,y},13*su,Fade(GetColor(0xFFD34DFF),0.9));
    DrawLineEx((Vector2){ball.x+30*su,y},(Vector2){ball.x+30*su+pull*2.4,y},5*su,Fade(GetColor(0x6FE7FFFF),0.85));
    DrawTriangle((Vector2){ball.x+40*su+pull*2.4,y},(Vector2){ball.x+22*su+pull*2.4,y-11*su},(Vector2){ball.x+22*su+pull*2.4,y+11*su},GetColor(0x6FE7FFFF));
    DrawCircleV(ball,20*su,WHITE);
    DrawCircleLines(ball.x,ball.y,20*su,GRAY);

    //labels, each one centred on the thing it names
    const char *drag_text = "1.  pull back with the mouse";
    const char *go_text = "2.  let go, and it flies";
    DrawText(drag_text,ball.x-pull/2-MeasureText(drag_text,28*su)/2-30*su,y-104*su,28*su,GetColor(0xFFD34DFF));
    DrawText(go_text,ball.x+30*su+pull*1.2-MeasureText(go_text,28*su)/2,y+62*su,28*su,GetColor(0x6FE7FFFF));
    DrawText("your ball",ball.x-MeasureText("your ball",26*su)/2,y-58*su,26*su,GRAY);
    DrawText("the hole",hole.x-MeasureText("the hole",26*su)/2,y+62*su,26*su,GRAY);

    //--- the rules ---
    const char *step[6] = {
        "The white ball is you. The hole with the flag is where it has to go.",
        "Every shot counts as one stroke, and each level gives you a limited number of them.",
        "The counter at the top of the screen shows the strokes you have used and the limit.",
        "Water, lava, the void, quicksand and the traps put the ball back where you shot from.",
        "R plays the level again from the start. ESC leaves it and goes back to the menu.",
        "Before every level a briefing names each obstacle in it. Press SKIP once you know them."};
    for (int i=0; i<6; i++)
    {
        float ry = p.y+350*su+i*54*su;
        DrawCircle(p.x+94*su,ry+15*su,19*su,Fade(GetColor(0xFFD34DFF),0.22));
        DrawText(TextFormat("%d",i+1),p.x+86*su,ry+1*su,28*su,GetColor(0xFFD34DFF));
        DrawText(step[i],p.x+140*su,ry,28*su,LIGHTGRAY);
    }

    //--- the scoring, behind its own line ---
    DrawLineEx((Vector2){p.x+70*su,p.y+686*su},(Vector2){p.x+p.width-70*su,p.y+686*su},2*su,GetColor(0x2A3550FF));
    DrawText("SCORING",p.x+86*su,p.y+706*su,30*su,GetColor(0x6FE7FFFF));
    DrawText("each stroke you did NOT use is worth 10 points on BOT, 25 on CHAD, 50 on GOAT,",p.x+280*su,p.y+706*su,26*su,LIGHTGRAY);
    DrawText("plus 100 / 250 / 500 for finishing. Your best on each level is kept in scores.txt.",p.x+280*su,p.y+742*su,26*su,LIGHTGRAY);
}


//a photo of any shape, drawn as a square: the source rectangle takes the middle of it
void draw_developer(Texture2D photo, float centre_x, float y, float size, const char *name)
{
    Color colour = GetColor(0x6FE7FFFF);
    float side = photo.width;
    if (photo.height<side) side = photo.height;
    Rectangle source = {(photo.width-side)/2,(photo.height-side)/2,side,side};
    Rectangle dest = {centre_x-size/2,y,size,size};
    Rectangle glow = {dest.x-6*su,y-6*su,size+12*su,size+12*su};
    Vector2 no_origin = {0,0};
    DrawRectangleRec(glow,Fade(colour,0.15));
    DrawTexturePro(photo,source,dest,no_origin,0,WHITE);
    DrawRectangleLinesEx(dest,3*su,Fade(colour,0.7));
    DrawText(name,centre_x-MeasureText(name,32*su)/2,y+size+16*su,32*su,WHITE);
}


void draw_credits()
{
    Rectangle p = draw_page("CREDITS",1);
    float x = p.x+100*su;
    draw_mascot((Vector2){p.x+p.width-150*su,p.y+195*su},3,0,180*su,0);
    DrawText("THE ULTIMATE GOLF",x,p.y+60*su,64*su,GetColor(0xFFD34DFF));

    DrawText("DEVELOPERS",x,p.y+170*su,32*su,GetColor(0x6FE7FFFF));
    draw_developer(developer_photo[0],p.x+410*su,p.y+220*su,160*su,"Abdullah Al Nafi  -  2505093");
    draw_developer(developer_photo[1],p.x+1030*su,p.y+220*su,160*su,"Syed Abdul Fahim  -  2505114");

    DrawText("SPRITES AND ARTWORK",x,p.y+465*su,32*su,GetColor(0x6FE7FFFF));
    DrawText("All sprites and textures in this game were generated with AI.",x,p.y+510*su,30*su,LIGHTGRAY);

    DrawText("SOUND AND MUSIC",x,p.y+575*su,32*su,GetColor(0x6FE7FFFF));
    DrawText("All sound effects and music tracks are from freesound.org.",x,p.y+620*su,30*su,LIGHTGRAY);

    DrawText("SPECIAL THANKS",x,p.y+685*su,32*su,GetColor(0x6FE7FFFF));
    DrawText("raylib, by Ramon Santamaria and its contributors - the library this game is built on.",x,p.y+730*su,30*su,LIGHTGRAY);


}


void draw_leaderboard()
{
    Rectangle p = draw_page("LEADERBOARD",1);
    if (score_count==0)
    {
        DrawText("no scores yet - go and finish a level",p.x+100*su,p.y+120*su,44*su,GRAY);
        return;
    }

    //sort the rows by total, biggest first
    int order[MAX_PLAYERS];
    for (int i=0; i<score_count; i++) order[i] = i;
    for (int i=0; i<score_count; i++)
    {
        for (int j=i+1; j<score_count; j++)
        {
            if (player_total(order[j])>player_total(order[i]))
            {
                int keep = order[i];
                order[i] = order[j];
                order[j] = keep;
            }
        }
    }

    float name_x = p.x+180*su;
    float first = p.x+640*su;
    float step = 160*su;
    float total_x = p.x+1330*su;
    float head = p.y+70*su;
    DrawText("#",p.x+90*su,head,32*su,GRAY);
    DrawText("NAME",name_x,head,32*su,GRAY);
    for (int i=0; i<4; i++) DrawText(TextFormat("L%d",i+1),first+i*step,head,32*su,card_colour[i]);
    DrawText("TOTAL",total_x,head,32*su,GetColor(0xFFD34DFF));
    DrawLineEx((Vector2){p.x+80*su,head+46*su},(Vector2){p.x+p.width-80*su,head+46*su},2*su,GetColor(0x2A3550FF));

    int rows = score_count;
    if (rows>10) rows = 10;
    for (int r=0; r<rows; r++)
    {
        int i = order[r];
        float y = head+76*su+r*62*su;
        if (i==player_row) DrawRectangleRounded((Rectangle){p.x+70*su,y-10*su,p.width-140*su,58*su},0.4,8,Fade(GetColor(0xFFD34DFF),0.15));
        Color shade = r==0 ? GetColor(0xFFD34DFF) : WHITE;
        DrawText(TextFormat("%d",r+1),p.x+90*su,y,36*su,shade);
        DrawText(score_name[i],name_x,y,36*su,shade);
        for (int n=0; n<4; n++)
        {
            if (score_points[i][n]>0) DrawText(TextFormat("%d",score_points[i][n]),first+n*step,y,36*su,LIGHTGRAY);
            else DrawText("-",first+n*step,y,36*su,DARKGRAY);
        }
        DrawText(TextFormat("%d",player_total(i)),total_x,y,36*su,shade);
    }
    DrawText("kept in scores.txt, next to the game",p.x+90*su,p.y+p.height-70*su,26*su,GRAY);
}


//the R key lives inside each level, so the button needs the same door
void restart_level()
{
    if (level==1) l1_reset_level();
    if (level==2) l2_reset_level();
    if (level==3) l3_reset_level();
    if (level==4) l4_reset_level();
    score_saved = 0;
}


//the two buttons drawn over the level's own "R restart  ESC menu" text
void draw_level_keys()
{
    //the level draws "R restart  ESC menu" itself; this plate hides it and the buttons take its place
    DrawRectangle(hud_key(0).x-32*su,6*su,screen_width-hud_key(0).x+32*su,58*su,GetColor(0x1A1E28FF));
    draw_key_button(hud_key(0),"R","restart",GetColor(0xFFD34DFF),over(hud_key(0)));
    draw_key_button(hud_key(1),"ESC","menu",GetColor(0xB9C0CCFF),over(hud_key(1)));
    if (level_state()!=0)
    {
        DrawRectangle(screen_width/2-330*su,screen_height/2+56*su,660*su,74*su,GetColor(0x1A1E28FF));
        draw_key_button(end_key(0),"R","play again",GetColor(0xFFD34DFF),over(end_key(0)));
        draw_key_button(end_key(1),"ESC","menu",GetColor(0xB9C0CCFF),over(end_key(1)));
    }
}


//the points for the run that has just finished, under the level's own panel
void draw_score_panel()
{
    int d = current_difficulty;
    int left = level_limit()-level_strokes();
    if (left<0) left = 0;
    int points = score_for(level_limit(),level_strokes(),d);
    Rectangle panel = {screen_width/2-340*su,screen_height/2+200*su,680*su,180*su};
    DrawRectangleRounded(panel,0.08,8,GetColor(0x151C2BFF));
    DrawRectangleRoundedLinesEx(panel,0.08,8,3*su,GetColor(0xFFD34DFF));
    DrawText(TextFormat("%d strokes left  x  %d",left,score_rate[d]),panel.x+36*su,panel.y+28*su,30*su,LIGHTGRAY);
    DrawText(TextFormat("finish bonus  + %d",score_bonus[d]),panel.x+36*su,panel.y+72*su,30*su,LIGHTGRAY);
    DrawText(TextFormat("your best: %d",player_best(level)),panel.x+36*su,panel.y+122*su,28*su,GRAY);
    const char *big = TextFormat("%d",points);
    DrawText(big,panel.x+panel.width-MeasureText(big,80*su)-40*su,panel.y+40*su,80*su,GetColor(0xFFD34DFF));
    DrawText("POINTS",panel.x+panel.width-MeasureText("POINTS",26*su)-40*su,panel.y+126*su,26*su,GRAY);
}


//==================== SOUNDS ====================
Sound snd_shot, snd_bounce, snd_pot, snd_fanfare, snd_game_over, snd_reset, snd_blip;
Sound snd_hop, snd_slam;
Sound snd_clang, snd_fan, snd_laser, snd_lava;
Sound snd_splash, snd_crab, snd_boing;
Sound snd_warp, snd_whoosh, snd_ufo;
Sound snd_chomp, snd_dart, snd_plank, snd_door, snd_plate, snd_gloop;
Music music[7];            //0 menu, 1-4 level ambience, 5 crabs walking (level 2), 6 extra intro track
float music_base[7] = {0.5,0.4,0.4,2.5,0.4,0.35,1};   //music[3] is a deep quiet hum, so it gets a boost


//each stream's own level, set once
void set_music_volumes()
{
    for (int i=0; i<7; i++) SetMusicVolume(music[i],music_base[i]);
}


//the speaker button and the M key: one call silences the whole audio device,
//music and sound effects together, without touching a single volume of its own
void apply_sound_switch()
{
    SetMasterVolume(sound_on ? 1.0 : 0.0);
}
int music_playing = -1;
float bounce_wait = 0;
float intro_last_hop = 0;
int last_hover = -1;

//what the level looked like before this frame, to spot what changed
Vector2 old_speed;
int old_stroke;
int old_state;
float old_message;
float old_extra[20];


//a quiet sound made louder while loading (gain 2 = twice as loud)
Sound load_sound_louder(const char *file, float gain)
{
    Wave wave = LoadWave(file);
    WaveFormat(&wave,wave.sampleRate,16,wave.channels);
    short *samples = (short *)wave.data;
    for (unsigned int i=0; i<wave.frameCount*wave.channels; i++)
    {
        float louder = samples[i]*gain;
        if (louder>32767) louder = 32767;
        if (louder<-32768) louder = -32768;
        samples[i] = louder;
    }
    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}


void load_sounds()
{
    snd_shot = LoadSound("assets/sounds-shared/shot_hit.wav");
    snd_bounce = LoadSound("assets/sounds-shared/wall_bounce.wav");
    snd_pot = load_sound_louder("assets/sounds-shared/ball_in_pot.wav",8);
    snd_fanfare = LoadSound("assets/sounds-shared/game-success-fanfare.wav");
    snd_game_over = LoadSound("assets/sounds-shared/game-over.wav");
    snd_reset = LoadSound("assets/sounds-shared/hazard_reset.wav");
    snd_blip = LoadSound("assets/sounds-shared/hover_blip.wav");
    snd_hop = LoadSound("assets/sounds-intro/mascot_hop.wav");
    snd_slam = load_sound_louder("assets/sounds-intro/title_slam.wav",1.6);
    snd_clang = LoadSound("assets/sounds-foundry/metal_clang.wav");
    snd_fan = load_sound_louder("assets/sounds-foundry/fan_whirr.wav",8);
    snd_laser = LoadSound("assets/sounds-foundry/laser_zap.wav");
    snd_lava = LoadSound("assets/sounds-foundry/lava_sizzle.mp3");
    snd_splash = load_sound_louder("assets/sounds-shoreline/water_splash.wav",5);
    snd_crab = LoadSound("assets/sounds-shoreline/crab_hit.wav");
    snd_boing = LoadSound("assets/sounds-shoreline/ball_hit.wav");
    snd_warp = LoadSound("assets/sounds-event_horizon/wormhole_warp.wav");
    snd_whoosh = LoadSound("assets/sounds-event_horizon/commet_whoosh.wav");
    snd_ufo = LoadSound("assets/sounds-event_horizon/UFO.ogg");
    snd_chomp = LoadSound("assets/sounds-lost_temple/piranha.wav");
    snd_dart = LoadSound("assets/sounds-lost_temple/dart_thwip.wav");
    snd_plank = LoadSound("assets/sounds-lost_temple/plank_break.wav");
    snd_door = LoadSound("assets/sounds-lost_temple/stone_door_open.wav");
    snd_plate = LoadSound("assets/sounds-lost_temple/pressure_plate.wav");
    snd_gloop = LoadSound("assets/sounds-lost_temple/quicksand_gallop.wav");

    //music and ambience stream from the file instead of loading it all
    music[0] = LoadMusicStream("assets/sounds-intro/menu_music.mp3");
    music[1] = LoadMusicStream("assets/sounds-foundry/factory_ambience.wav");
    music[2] = LoadMusicStream("assets/sounds-shoreline/beach_ambience.wav");
    music[3] = LoadMusicStream("assets/sounds-event_horizon/space_ambience.wav");
    music[4] = LoadMusicStream("assets/sounds-lost_temple/jungle_ambience.wav");
    music[5] = LoadMusicStream("assets/sounds-shoreline/crab_walking.wav");
    music[6] = LoadMusicStream("assets/sounds-intro/additional_music_for_intro.wav");
    set_music_volumes();
    apply_sound_switch();
}


void unload_sounds()
{
    Sound all[25] = {snd_shot,snd_bounce,snd_pot,snd_fanfare,snd_game_over,snd_reset,snd_blip,snd_hop,snd_slam,snd_clang,snd_fan,snd_laser,snd_lava,
                     snd_splash,snd_crab,snd_boing,snd_warp,snd_whoosh,snd_ufo,snd_chomp,snd_dart,snd_plank,snd_door,snd_plate,snd_gloop};
    for (int i=0; i<25; i++) UnloadSound(all[i]);
    for (int i=0; i<7; i++) UnloadMusicStream(music[i]);
}


void play(Sound sound, float volume)
{
    SetSoundVolume(sound,volume);
    PlaySound(sound);
}


//switch the background music (0 menu, 1-4 levels), level 2 also gets the crabs walking
void play_music(int m)
{
    if (m==music_playing) return;
    for (int i=0; i<6; i++) StopMusicStream(music[i]);
    PlayMusicStream(music[m]);
    if (m==2) PlayMusicStream(music[5]);
    music_playing = m;
}


void update_music()
{
    if (music_playing>=0) UpdateMusicStream(music[music_playing]);
    if (music_playing==2) UpdateMusicStream(music[5]);
}


//how high the intro mascot is (0 on the ground, 1 top of the hop), same maths as draw_intro
float intro_hop(float t)
{
    int part = t/1.6;
    if (part>3) part = 3;
    if (part<3)
    {
        float hops = 2;
        if (part==2) hops = 1;
        return fabsf(sin((t - part*1.6)/1.6*hops*PI));
    }
    float landing = (t - 3*1.6)/0.8;
    if (landing>1) landing = 1;
    return sin(landing*PI);
}


void remember_level()
{
    if (level==1)
    {
        old_speed = l1_speed;
        old_stroke = l1_stroke;
        old_state = l1_game_state;
        old_message = l1_message_timer;
        old_extra[0] = l1_laser_timer;
    }
    if (level==2)
    {
        old_speed = l2_speed;
        old_stroke = l2_stroke;
        old_state = l2_game_state;
        old_message = l2_message_timer;
    }
    if (level==3)
    {
        old_speed = l3_speed;
        old_stroke = l3_stroke;
        old_state = l3_game_state;
        old_message = l3_message_timer;
        old_extra[0] = l3_wormhole_cooldown;
        old_extra[1] = l3_event_timer;
        old_extra[2] = l3_laser_clock[0];
        old_extra[3] = l3_laser_clock[1];
    }
    if (level==4)
    {
        old_speed = l4_speed;
        old_stroke = l4_stroke;
        old_state = l4_game_state;
        old_message = l4_message_timer;
        for (int i=0; i<5; i++)
        {
            old_extra[i] = l4_plank_timer[0][i];
            old_extra[5+i] = l4_plank_timer[1][i];
        }
        old_extra[10] = l4_gate_timer;
        old_extra[11] = l4_plate_down[1] + l4_plate_down[2];
        old_extra[12] = l4_door_velocity[0].y;
        old_extra[13] = l4_door_velocity[1].y;
        for (int i=0; i<4; i++) old_extra[14+i] = l4_dart_clock[i];
    }
}


//a bounce sound that fits what was hit, louder when the ball was faster
void bounce_sound(Vector2 at, float impact)
{
    float volume = impact/(650*su);
    if (volume<0.25) volume = 0.25;
    if (volume>1) volume = 1;
    Sound sound = snd_bounce;
    if (level==1) sound = snd_clang;
    if (level==2)
    {
        for (int i=0; i<4; i++)
        {
            if (Vector2Distance(at,l2_beach_ball[i]) < l2_beach_ball_radius+l2_radius_ball+12*su) sound = snd_boing;
            if (CheckCollisionCircleRec(at,l2_radius_ball+12*su,l2_crab[i])) sound = snd_crab;
        }
    }
    if (level==3)
    {
        for (int i=0; i<4; i++)
        {
            if (Vector2Distance(at,l3_bumper[i]) < l3_bumper_radius+l3_radius_ball+12*su) sound = snd_boing;
        }
    }
    if (level==4)
    {
        for (int i=0; i<6; i++)
        {
            if (Vector2Distance(at,l4_bumper[i]) < l4_bumper_radius+l4_radius_ball+12*su) sound = snd_boing;
        }
    }
    play(sound,volume);
}


//a timed trap (laser or darts) just switched on: only heard when it is close to the ball
void trap_sound(float old_clock, float new_clock, Rectangle trap, Vector2 at, Sound sound)
{
    Vector2 middle = {trap.x+trap.width/2,trap.y+trap.height/2};
    if (old_clock<1.6 && new_clock>=1.6 && Vector2Distance(middle,at)<450*su) play(sound,0.35);
}


//compare the level with how it was before this frame and play what happened
void level_sounds()
{
    Vector2 speed_now = l1_speed;
    Vector2 ball_now = l1_ball;
    int stroke_now = l1_stroke;
    int state_now = l1_game_state;
    float message_now = l1_message_timer;
    int type_now = l1_message_type;
    int teleported = 0;
    if (level==2)
    {
        speed_now = l2_speed;
        ball_now = l2_ball;
        stroke_now = l2_stroke;
        state_now = l2_game_state;
        message_now = l2_message_timer;
        type_now = 1;
    }
    if (level==3)
    {
        speed_now = l3_speed;
        ball_now = l3_ball;
        stroke_now = l3_stroke;
        state_now = l3_game_state;
        message_now = l3_message_timer;
        type_now = l3_message_type;
        if (l3_wormhole_cooldown>old_extra[0]+0.2)
        {
            teleported = 1;
            play(snd_warp,0.9);
        }
    }
    if (level==4)
    {
        speed_now = l4_speed;
        ball_now = l4_ball;
        stroke_now = l4_stroke;
        state_now = l4_game_state;
        message_now = l4_message_timer;
        type_now = l4_message_type;
    }

    //shot
    if (stroke_now>old_stroke) play(snd_shot,0.9);

    //bounce: the ball was moving and suddenly turned (not a shot, not a wormhole)
    bounce_wait = bounce_wait - GetFrameTime();
    float before = Vector2Length(old_speed);
    float after = Vector2Length(speed_now);
    if (bounce_wait<=0 && teleported==0 && stroke_now==old_stroke && before>40*su && after>20*su)
    {
        float turn = Vector2DotProduct(Vector2Normalize(old_speed),Vector2Normalize(speed_now));
        if (turn<0.6)
        {
            bounce_sound(ball_now,before);
            bounce_wait = 0.08;
        }
    }

    //hazards: whoosh back plus the level's own sound
    if (message_now>old_message+0.5)
    {
        play(snd_reset,0.6);
        if (level==1 && type_now==1) play(snd_lava,0.9);
        if (level==1 && type_now==2) play(snd_laser,0.9);
        if (level==2) play(snd_splash,1);
        if (level==3 && type_now==1) play(snd_whoosh,0.5);
        if (level==3 && type_now==2) play(snd_laser,0.9);
        if (level==3 && type_now==3) play(snd_ufo,0.9);
        if (level==4 && type_now==1) play(snd_chomp,1);
        if (level==4 && type_now==2) play(snd_dart,1);
        if (level==4 && type_now==3) play(snd_gloop,1);
    }

    //win and lose
    if (state_now==1 && old_state==0)
    {
        play(snd_pot,1);
        play(snd_fanfare,0.8);
    }
    if (state_now==2 && old_state==0) play(snd_game_over,0.8);

    //level 1: laser switching on nearby
    if (level==1) trap_sound(old_extra[0],l1_laser_timer,l1_laser,ball_now,snd_laser);

    //level 3: comet / meteors / ufo arriving, lasers switching on nearby
    if (level==3)
    {
        if (l3_event_type!=0 && old_extra[1]<1.5 && l3_event_timer>=1.5)
        {
            if (l3_event_type==3) play(snd_ufo,0.7);
            else play(snd_whoosh,0.8);
        }
        for (int i=0; i<2; i++) trap_sound(old_extra[2+i],l3_laser_clock[i],l3_laser_gate[i],ball_now,snd_laser);
    }

    //level 4: planks cracking, plates, doors starting to move, darts firing nearby
    if (level==4)
    {
        int cracked = 0;
        for (int i=0; i<5; i++)
        {
            if (old_extra[i]==0 && l4_plank_timer[0][i]>0) cracked = 1;
            if (old_extra[5+i]==0 && l4_plank_timer[1][i]>0) cracked = 1;
        }
        if (cracked==1) play(snd_plank,0.7);
        if (l4_gate_timer>old_extra[10]+1 || l4_plate_down[1]+l4_plate_down[2]>old_extra[11]) play(snd_plate,1);
        for (int i=0; i<2; i++)
        {
            if (old_extra[12+i]==0 && l4_door_velocity[i].y!=0) play(snd_door,0.8);
        }
        for (int i=0; i<4; i++) trap_sound(old_extra[14+i],l4_dart_clock[i],l4_dart_gate[i],ball_now,snd_dart);
    }
}


//==================== LEVEL BRIEFING ====================
//every obstacle: name, where to cut its live picture from the level (design units), what it does
//picture x = -1 draws the ufo sprite, -2 a comet, -3 a meteor shower (they have no fixed place)
int brief_count[4] = {13,9,13,11};
const char *brief_name[4][13] = {
    {"Valve wheel","Steel crate","Push-down belt","Warning diamond","Steel girders","Oil slick","Spinning fan","Crane beam","Hydraulic piston","Magnet","Push-up belt","Molten pit","Laser gate"},
    {"The sea","Soft sand","Wet sand","Tidal sand bar","Rip current","Whirlpool","Crab","Palm tree","Beach ball"},
    {"The void","Wormhole","Black hole","Planet","Asteroid","Laser gate","Satellite","Energy bumper","Vacuum strip","Solar wind","Comet","Meteor shower","UFO"},
    {"Piranha river","Plank bridge","Mud","Moss","Quicksand","Boulder","Mushroom","Gate plate","Dart trap","Altar plates","Totem"}};
float brief_x[4][13] = {
    {226,126,171,486,418,486,801,801,1116,1116,1116,1376,1431},
    {390,225,465,520,915,730,1200,1180,1000},
    {800,1840,960,600,700,1810,1200,1330,675,110,-2,-3,-1},
    {690,690,340,835,950,330,250,1080,1280,1810,1810}};
float brief_y[4][13] = {
    {692,842,460,428,607,882,478,731,568,818,380,722,358},
    {520,515,165,490,630,820,140,225,225},
    {250,700,640,720,150,454,990,580,990,490,0,0,0},
    {600,210,920,350,795,570,220,570,334,200,570}};
const char *brief_text[4][13] = {
    {"Round bumper. Bounces the ball\nstraight back off it.",
     "Solid block. Use its flat sides\nfor bank shots.",
     "Drags the ball down while it's on it.\nHit it hard to get across.",
     "Pointy block: sends the ball\noff at an angle.",
     "Fixed bars sticking out of the walls.\nAim around them.",
     "Almost no friction: the ball\nslides much further.",
     "Blades whack the ball away.\nTime your shot between them.",
     "Slides left and right and\nknocks the ball.",
     "Punches out of the wall every few\nseconds. Red light = incoming.",
     "Pulls a moving ball in. Touch the\ncore and it sticks.",
     "Pushes the ball back up this lane.\nNeeds a strong shot.",
     "Roll into the lava = MELTED.\nBack to your last shot.",
     "Deadly while red. Blinking yellow\nmeans it's about to fire."},
    {"Ball's centre in the water = SPLASH.\nBack to your last shot.",
     "Kills the ball's speed fast.",
     "Slippery: the ball slides\nmuch further.",
     "Dry at low tide, sea at high tide.\nThe rising tide pushes you back.",
     "Shallow water that drags the ball\nsideways into the sea.",
     "Pulls the ball off the sand\ninto the water.",
     "Walks side to side and kicks\nthe ball away.",
     "Solid trunk. The leaves hide\nyour ball underneath.",
     "Super bouncy: sends the ball\naway faster than it came."},
    {"Roll off a walkway = LOST IN SPACE.\nBack to your last shot.",
     "Enter a ring, pop out of its twin going\nthe way the arrow points. Orange = trap!",
     "Bends moving balls toward it.\nThe pot is right next to it.",
     "Its gravity bends your shot.\nThe core is solid.",
     "Drifts across the walkway\nand knocks you off.",
     "Deadly while red. Blinking\nmeans it's about to fire.",
     "Spinning solar panels\nwhack the ball.",
     "Bounces the ball back faster\nthan it came.",
     "Almost no friction: the ball\nslides much further.",
     "Blows the ball toward the void.",
     "Streaks across without stopping.\nA red arrow warns you first.",
     "Once per game a pack of 5 rocks flies\nthrough together. Each one knocks you.",
     "Flies over once. Sit still under it and\nit beams your ball somewhere else."},
    {"Ball in the river = CHOMP.\nBack to your last shot.",
     "Planks crack when you roll on them and\nfall a moment later. Keep rolling!",
     "Slows the ball a lot.",
     "Slippery: the ball slides\nmuch further.",
     "Stop in it and you sink in\n3 seconds = SUNK.",
     "Rolls back and forth and\nknocks the ball.",
     "Bouncy: sends the ball away\nfaster than it came.",
     "Roll over it: the temple gate\nopens for 6 seconds.",
     "Fires darts while its eyes are red\n= DARTED.",
     "Press BOTH corner plates to open the\naltar door. Falling un-presses them.",
     "Spinning log arms guard\nthe altar door."}};
Color brief_accent[4] = {{232,185,35,255},{255,214,110,255},{80,230,255,255},{232,184,64,255}};
Color brief_text_colour[4] = {{235,235,235,255},{250,235,205,255},{210,240,255,255},{240,236,220,255}};


Rectangle brief_panel()
{
    Rectangle panel = {screen_width/2-760*su,screen_height/2-470*su,1520*su,940*su};
    return panel;
}


//buttons: 0 skip, 1 back, 2 next / play
Rectangle brief_button(int b)
{
    Rectangle p = brief_panel();
    Rectangle rec = {p.x+p.width-230*su,p.y+40*su,190*su,60*su};
    if (b==1)
    {
        rec.x = p.x+40*su;
        rec.y = p.y+p.height-100*su;
        rec.width = 220*su;
        rec.height = 70*su;
    }
    if (b==2)
    {
        rec.x = p.x+p.width-300*su;
        rec.y = p.y+p.height-100*su;
        rec.width = 260*su;
        rec.height = 70*su;
    }
    return rec;
}


int brief_pages()
{
    return (brief_count[level-1]+4)/5;
}


void brief_step(float dt)
{
    //the level keeps moving behind the page and in the pictures
    background_step(level,dt);
    BeginTextureMode(card_texture[level-1]);
    ClearBackground(BLACK);
    draw_scene(level);
    EndTextureMode();

    Vector2 mouse = GetMousePosition();
    int last_page = brief_pages()-1;
    int next = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(mouse,brief_button(0)))
        {
            play(snd_blip,0.9);
            game_mode = 2;
            return;
        }
        if (brief_page>0 && CheckCollisionPointRec(mouse,brief_button(1)))
        {
            play(snd_blip,0.9);
            brief_page--;
        }
        if (CheckCollisionPointRec(mouse,brief_button(2))) next = 1;
    }
    if (next)
    {
        play(snd_blip,0.9);
        if (brief_page<last_page) brief_page++;
        else game_mode = 2;
    }
    if (IsKeyPressed(KEY_ESCAPE) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && over(brief_menu_key()))) game_mode = 1;
}


//the page itself, in the level's own style
void draw_brief_panel(Rectangle p, int n)
{
    if (n==1)
    {
        //riveted steel with hazard stripes
        DrawRectangleGradientV(p.x,p.y,p.width,p.height,GetColor(0x5A5F66FF),GetColor(0x2E3136FF));
        Rectangle band = {p.x,p.y,p.width,18*su};
        l1_draw_hazard_stripes(band);
        band.y = p.y+p.height-18*su;
        l1_draw_hazard_stripes(band);
        for (int i=0; i<p.width/(60*su); i++)
        {
            DrawCircle(p.x+30*su+i*60*su,p.y+30*su,4*su,GetColor(0x8A9099FF));
            DrawCircle(p.x+30*su+i*60*su,p.y+p.height-30*su,4*su,GetColor(0x8A9099FF));
        }
        DrawRectangleLinesEx(p,5*su,BLACK);
    }
    if (n==2)
    {
        //driftwood planks with a rope edge
        DrawRectangleRec(p,GetColor(0x8B5E34FF));
        for (int i=1; i<p.height/(64*su); i++) DrawLine(p.x,p.y+i*64*su,p.x+p.width,p.y+i*64*su,GetColor(0x6E4526FF));
        for (int i=0; i<p.width/(24*su); i++)
        {
            DrawCircle(p.x+i*24*su,p.y,5*su,GetColor(0xD8B878FF));
            DrawCircle(p.x+i*24*su,p.y+p.height,5*su,GetColor(0xD8B878FF));
        }
        for (int i=0; i<p.height/(24*su); i++)
        {
            DrawCircle(p.x,p.y+i*24*su,5*su,GetColor(0xD8B878FF));
            DrawCircle(p.x+p.width,p.y+i*24*su,5*su,GetColor(0xD8B878FF));
        }
    }
    if (n==3)
    {
        //neon hologram with scanlines
        Rectangle glow = {p.x-8*su,p.y-8*su,p.width+16*su,p.height+16*su};
        DrawRectangleLinesEx(glow,8*su,Fade(brief_accent[2],0.2));
        DrawRectangleRec(p,Fade(GetColor(0x0A1030FF),0.9));
        for (int i=0; i<p.height/(4*su); i++) DrawLine(p.x,p.y+i*4*su,p.x+p.width,p.y+i*4*su,Fade(brief_accent[2],0.04));
        DrawRectangleLinesEx(p,2*su,brief_accent[2]);
        //corner brackets
        Vector2 top_left = {p.x,p.y};
        Vector2 top_right = {p.x+40*su,p.y};
        Vector2 left_down = {p.x,p.y+40*su};
        Vector2 bottom_right = {p.x+p.width,p.y+p.height};
        Vector2 bottom_left = {p.x+p.width-40*su,p.y+p.height};
        Vector2 right_up = {p.x+p.width,p.y+p.height-40*su};
        DrawLineEx(top_left,top_right,6*su,brief_accent[2]);
        DrawLineEx(top_left,left_down,6*su,brief_accent[2]);
        DrawLineEx(bottom_right,bottom_left,6*su,brief_accent[2]);
        DrawLineEx(bottom_right,right_up,6*su,brief_accent[2]);
    }
    if (n==4)
    {
        //carved stone tablet with gold trim and moss
        DrawRectangleGradientV(p.x,p.y,p.width,p.height,GetColor(0x7C8078FF),GetColor(0x4E534AFF));
        DrawRectangleLinesEx(p,6*su,brief_accent[3]);
        Rectangle inner = {p.x+14*su,p.y+14*su,p.width-28*su,p.height-28*su};
        DrawRectangleLinesEx(inner,2*su,Fade(brief_accent[3],0.7));
        Vector2 corner[4] = {{p.x,p.y},{p.x+p.width,p.y},{p.x,p.y+p.height},{p.x+p.width,p.y+p.height}};
        for (int i=0; i<4; i++)
        {
            DrawCircle(corner[i].x,corner[i].y,22*su,brief_accent[3]);
            DrawCircle(corner[i].x,corner[i].y,12*su,GetColor(0x4E534AFF));
        }
        for (int i=0; i<9; i++) DrawCircle(p.x+fmod(i*397*su,p.width),p.y+p.height-10*su-fmod(i*53*su,40*su),(10+i%4*4)*su,Fade(GetColor(0x5E7F3AFF),0.6));
    }
}


void draw_brief()
{
    float t = GetTime();
    int n = level;
    Color accent = brief_accent[n-1];
    Color text = brief_text_colour[n-1];
    Vector2 mouse = GetMousePosition();

    //the live level, dimmed, behind the page
    Rectangle whole = {0,0,screen_width,-screen_height};
    Rectangle screen = {0,0,screen_width,screen_height};
    Vector2 no_origin = {0,0};
    DrawTexturePro(card_texture[n-1].texture,whole,screen,no_origin,0,WHITE);
    DrawRectangle(0,0,screen_width,screen_height,Fade(BLACK,0.6));

    Rectangle p = brief_panel();
    draw_brief_panel(p,n);

    //title, difficulty and strokes, page number
    const char *difficulty_name[3] = {"BOT","CHAD","GOAT"};
    int strokes = l1_stroke_limit;
    if (n==2) strokes = l2_stroke_limit;
    if (n==3) strokes = l3_stroke_limit;
    if (n==4) strokes = l4_stroke_limit;
    DrawText(TextFormat("%s  -  what's waiting for you",level_name[n-1]),p.x+50*su+4*su,p.y+44*su+4*su,56*su,BLACK);
    DrawText(TextFormat("%s  -  what's waiting for you",level_name[n-1]),p.x+50*su,p.y+44*su,56*su,accent);
    DrawText(TextFormat("%s   |   %d strokes to sink it   |   page %d / %d",difficulty_name[current_difficulty],strokes,brief_page+1,brief_pages()),p.x+52*su,p.y+112*su,30*su,text);

    //up to 5 obstacles: live picture and name on the left, what it does on the right
    for (int row=0; row<5; row++)
    {
        int i = brief_page*5 + row;
        if (i>=brief_count[n-1]) break;
        float y = p.y + 170*su + row*128*su;
        Rectangle box = {p.x+50*su,y,184*su,115*su};
        DrawRectangleRec(box,BLACK);
        if (brief_x[n-1][i]>=0)
        {
            //cut the obstacle's spot out of the live level picture (it is stored upside down)
            float crop_w = 240*su;
            float crop_h = 150*su;
            float crop_x = Clamp(brief_x[n-1][i]*su - crop_w/2,0,screen_width-crop_w);
            float crop_y = Clamp(brief_y[n-1][i]*su - crop_h/2,0,screen_height-crop_h);
            Rectangle source = {crop_x,screen_height-crop_y-crop_h,crop_w,-crop_h};
            DrawTexturePro(card_texture[n-1].texture,source,box,no_origin,0,WHITE);
        }
        else if (brief_x[n-1][i]==-1)
        {
            //ufo sprite, lights chasing
            Rectangle source = {((int)(t*6)%3)*320,0,320,320};
            Rectangle dest = {box.x+box.width/2,box.y+box.height/2,105*su,105*su};
            Vector2 origin = {52.5*su,52.5*su};
            DrawRectangleRec(box,GetColor(0x06081AFF));
            DrawTexturePro(l3_ufo_texture,source,dest,origin,0,WHITE);
        }
        else if (brief_x[n-1][i]==-3)
        {
            //a pack of little rocks with orange tails flying through the box
            DrawRectangleRec(box,GetColor(0x06081AFF));
            BeginScissorMode(box.x,box.y,box.width,box.height);
            for (int j=0; j<5; j++)
            {
                float along = fmod(t*110*su + j*30*su,box.width+80*su) - 40*su;
                Vector2 rock = {box.x+along - j*12*su,box.y+18*su+j*20*su};
                Vector2 tail = {rock.x-26*su,rock.y-10*su};
                DrawLineEx(tail,rock,4*su,Fade(GetColor(0xFF9628FF),0.6));
                Rectangle source = {(j%3)*192,0,192,192};
                Rectangle dest = {rock.x,rock.y,20*su,20*su};
                Vector2 origin = {10*su,10*su};
                DrawTexturePro(l3_asteroid_texture,source,dest,origin,t*200+j*40,WHITE);
            }
            EndScissorMode();
        }
        else
        {
            //comet streaking across the little box
            DrawRectangleRec(box,GetColor(0x06081AFF));
            Vector2 head = {box.x+fmod(t*120*su,box.width+60*su)-20*su,box.y+box.height/2};
            BeginScissorMode(box.x,box.y,box.width,box.height);
            for (int j=10; j>0; j--) DrawCircle(head.x-j*9*su,head.y,(9-j*0.7)*su,Fade(brief_accent[2],0.5-j*0.04));
            DrawCircle(head.x,head.y,9*su,WHITE);
            EndScissorMode();
        }
        DrawRectangleLinesEx(box,3*su,accent);
        DrawText(brief_name[n-1][i],box.x+box.width+24*su,y+40*su,36*su,accent);
        DrawText(brief_text[n-1][i],p.x+640*su,y+22*su,30*su,text);
        if (row<4 && i+1<brief_count[n-1]) DrawLine(p.x+50*su,y+122*su,p.x+p.width-50*su,y+122*su,Fade(accent,0.3));
    }

    //buttons
    const char *label[3] = {"SKIP","BACK","NEXT"};
    if (brief_page==brief_pages()-1) label[2] = "PLAY!";
    for (int b=0; b<3; b++)
    {
        if (b==1 && brief_page==0) continue;
        Rectangle r = brief_button(b);
        if (CheckCollisionPointRec(mouse,r)) DrawRectangleRounded(r,0.3,8,Fade(accent,0.55));
        else DrawRectangleRounded(r,0.3,8,Fade(accent,0.25));
        DrawRectangleRoundedLinesEx(r,0.3,8,3*su,accent);
        int w = MeasureText(label[b],40*su);
        DrawText(label[b],r.x+r.width/2-w/2,r.y+r.height/2-20*su,40*su,text);
    }
    Rectangle menu_key = brief_menu_key();
    draw_key_button(menu_key,"ESC","menu",text,over(menu_key));
    DrawText("ENTER  next page",p.x+p.width/2+60*su,p.y+p.height-78*su,26*su,Fade(text,0.7));
}


int main()
{
    InitWindow(1280,720,"the ultimate golf");
    int monitor = GetCurrentMonitor();
    screen_width = GetMonitorWidth(monitor);
    screen_height = GetMonitorHeight(monitor);
    ToggleBorderlessWindowed();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    su = screen_height/1080.0;
    if (screen_width/1920.0 < su) su = screen_width/1920.0;

    l1_start(screen_width,screen_height);
    l2_start(screen_width,screen_height);
    l3_start(screen_width,screen_height);
    l4_start(screen_width,screen_height);
    mascot_texture = LoadTexture("assets/intro/intro_ball.png");
    SetTextureFilter(mascot_texture,TEXTURE_FILTER_BILINEAR);
    developer_photo[0] = LoadTexture("assets/abdullah.png");
    developer_photo[1] = LoadTexture("assets/fahim.png");
    for (int i=0; i<2; i++) SetTextureFilter(developer_photo[i],TEXTURE_FILTER_BILINEAR);
    for (int i=0; i<4; i++) card_texture[i] = LoadRenderTexture(screen_width,screen_height);
    InitAudioDevice();
    load_sounds();
    load_scores();

    while(!WindowShouldClose() && quit==0)
    {
        float dt = GetFrameTime();
        if (dt>1.0/30) dt = 1.0/30;

        //the sound switch: M anywhere, the big speaker on the menu pages, the small one in a level
        Rectangle speaker = game_mode==2 ? game_sound_button() : sound_button();
        int on_speaker = CheckCollisionPointRec(GetMousePosition(),speaker) && game_mode!=3;
        int sound_clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && on_speaker;
        if ((IsKeyPressed(KEY_M) && game_mode!=4) || sound_clicked)
        {
            sound_on = !sound_on;
            apply_sound_switch();
            play(snd_blip,0.9);
        }
        //the buttons drawn on top of a level: the same things R and ESC do
        if (game_mode==2 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int restart = over(hud_key(0)) || (level_state()!=0 && over(end_key(0)));
            int to_menu = over(hud_key(1)) || (level_state()!=0 && over(end_key(1)));
            if (restart==1 || to_menu==1)
            {
                play(snd_blip,0.9);
                ui_press = 1;
                if (restart==1) restart_level();
                if (to_menu==1) game_mode = 1;
            }
        }
        //a press on any of them must not also become a golf shot, so the level is left
        //alone until that press is let go again
        if (sound_clicked && game_mode==2) ui_press = 1;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) ui_press = 0;

        if (game_mode==0)
        {
            intro_time = intro_time + dt;
            int part = intro_time/1.6;
            if (part>3) part = 3;
            background_step(part+1,dt);

            //boing when the mascot lands, bang when the title lands
            float hop = intro_hop(intro_time);
            if (intro_last_hop>=0.05 && hop<0.05) play(snd_hop,0.7);
            intro_last_hop = hop;
            float title_time = intro_time - 3*1.6 - 0.8;
            if (title_time>=0.15 && title_time-dt<0.15) play(snd_slam,1);

            //click or any key skips to the menu
            if (intro_time>0.3 && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || GetKeyPressed()!=0))
            {
                game_mode = 4;
                while (GetCharPressed()>0);     //the key that skipped the intro must not land in the name box
            }
        }
        else if (game_mode==1)
        {
            //blip when the mouse moves onto a card or button, and on a click that does something
            int chosen_before = chosen_level;
            Vector2 mouse = GetMousePosition();
            int hover = -1;
            for (int i=0; i<4; i++)
            {
                if (chosen_level==0 && CheckCollisionPointRec(mouse,card_rect(i))) hover = i;
            }
            for (int d=0; d<3; d++)
            {
                if (chosen_level>0 && CheckCollisionPointRec(mouse,difficulty_rect(d))) hover = 10+d;
            }
            if (hover>=0 && hover!=last_hover) play(snd_blip,0.4);
            last_hover = hover;

            if (chosen_level==0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && over(menu_quit_key())) quit = 1;
            if (chosen_level==0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                for (int b=0; b<3; b++)
                {
                    if (CheckCollisionPointRec(mouse,menu_button(b)))
                    {
                        play(snd_blip,0.9);
                        game_mode = 5+b;
                    }
                }
            }
            if (game_mode!=1) continue;

            menu_step(dt);
            if (chosen_level!=chosen_before || game_mode!=1) play(snd_blip,0.9);
        }
        else if (game_mode==4)
        {
            //typing the name: letters, digits and spaces only, so the '|' in the file is safe
            int key = GetCharPressed();
            while (key>0)
            {
                if (key>='a' && key<='z') key = key-32;
                int allowed = (key>='A' && key<='Z') || (key>='0' && key<='9') || key==' ';
                if (allowed==1 && name_length<12)
                {
                    player_name[name_length] = key;
                    name_length++;
                    player_name[name_length] = 0;
                    play(snd_blip,0.4);
                }
                key = GetCharPressed();
            }
            int click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
            if ((IsKeyPressed(KEY_BACKSPACE) || (click==1 && over(name_key(1)))) && name_length>0)
            {
                name_length--;
                player_name[name_length] = 0;
                play(snd_blip,0.4);
            }
            if ((IsKeyPressed(KEY_ENTER) || (click==1 && over(name_key(0)))) && name_length>0)
            {
                pick_player(player_name);
                play(snd_blip,0.9);
                game_mode = 1;
            }
        }
        else if (game_mode>=5 && game_mode<=7)
        {
            int back = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(),page_back_button());
            if (IsKeyPressed(KEY_ESCAPE) || back==1)
            {
                play(snd_blip,0.9);
                game_mode = 1;
            }
        }
        else if (game_mode==3) brief_step(dt);
        else
        {
            if (IsKeyPressed(KEY_ESCAPE)) game_mode = 1;
            else if (ui_press==0)
            {
                remember_level();
                if (level==1) l1_update();
                if (level==2) l2_update();
                if (level==3) l3_update();
                if (level==4) l4_update();
                level_sounds();
                if (level_state()==0) score_saved = 0;
                if (level_state()==1 && score_saved==0)
                {
                    record_score(level,score_for(level_limit(),level_strokes(),current_difficulty));
                    score_saved = 1;
                }
            }
        }

        //music only belongs to a level: the intro, the menu and the pages are quiet
        if (game_mode==2 || game_mode==3) play_music(level);
        else if (music_playing>=0)
        {
            StopMusicStream(music[music_playing]);
            StopMusicStream(music[5]);
            music_playing = -1;
        }
        update_music();
        if (game_mode==2 && level==1)
        {
            if (IsSoundPlaying(snd_fan)==0) play(snd_fan,0.5);
        }
        else if (IsSoundPlaying(snd_fan)) StopSound(snd_fan);

        BeginDrawing();
        if (game_mode==0) draw_intro();
        if (game_mode==1) draw_menu();
        if (game_mode==3) draw_brief();
        if (game_mode==4) draw_name_entry();
        if (game_mode==5) draw_how_to_play();
        if (game_mode==6) draw_leaderboard();
        if (game_mode==7) draw_credits();
        if (game_mode==2)
        {
            if (level==1) l1_draw();
            if (level==2) l2_draw();
            if (level==3) l3_draw();
            if (level==4) l4_draw();
            if (level_state()==1) draw_score_panel();
            draw_level_keys();
            Rectangle small = game_sound_button();
            draw_speaker(small,sound_on,CheckCollisionPointRec(GetMousePosition(),small));
        }
        EndDrawing();
    }

    unload_sounds();
    CloseAudioDevice();
    for (int i=0; i<4; i++) UnloadRenderTexture(card_texture[i]);
    UnloadTexture(mascot_texture);
    for (int i=0; i<2; i++) UnloadTexture(developer_photo[i]);
    l2_unload();
    l3_unload();
    l4_unload();
    CloseWindow();
    return 0;
}
