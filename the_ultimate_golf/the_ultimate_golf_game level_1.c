#include "raylib.h"
#include "raymath.h"
#include <math.h>

//full global
int width = 1920;
int height = 1080;
float u = 1;
#define stroke_limit 18
#define max_speed 520

//colours
Color brick = {139,58,43,255};
Color brick_dark = {110,44,32,255};
Color mortar = {58,51,48,255};
Color steel = {90,95,102,255};
Color steel_light = {138,144,153,255};
Color steel_dark = {46,49,54,255};
Color floor_colour = {74,74,72,255};
Color hazard_yellow = {232,185,35,255};
Color rust = {181,84,28,255};
Color rust_dark = {107,46,14,255};
Color molten_orange = {255,106,0,255};
Color molten_yellow = {255,208,0,255};
Color molten_dark = {139,26,0,255};
Color laser_red = {255,32,48,255};
Color magnet_red = {192,40,45,255};

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
int message_type = 0;
float animation_time = 0;

//lanes
float hud_height;
float wall;
float gap;
float lane_width;
float lane_x[6];
float course_top;
float course_bottom;
Rectangle walls[9];
Vector2 no_speed = {0,0};

//valve wheel bumpers (one in lane 1, two next to the pot)
Vector2 bumper[3];
float bumper_radius;

//steel crate
Rectangle crate;

//conveyor belts (0 pushes down in lane 1, 1 pushes up in lane 4)
Rectangle belt[2];
float belt_push[2];

//warning diamond
Vector2 diamond;
float diamond_radius;

//steel girders
Rectangle girder[2];

//oil slick
Vector2 oil;
float oil_radius;

//spinning fan
Vector2 fan;
float fan_angle = 0;
float fan_spin = 120;
float fan_blade_length;
float fan_blade_thickness;
float fan_hub_radius;

//crane beams (0 in lane 3, 1 and 2 in lane 6)
Rectangle beam[3];
float beam_speed[3];
float beam_left_limit[3];
float beam_right_limit[3];

//magnet
Vector2 magnet;
float magnet_core;
float magnet_field;
float magnet_pull;

//hydraulic piston
Rectangle piston;
float piston_head;
float piston_length = 0;
float piston_max_length;
float piston_speed = 0;
float piston_timer = 0;
int piston_state = 0;

//molten pits
Vector2 pit[3];
float pit_radius;

//laser gate
Rectangle laser;
float laser_timer = 0;


void reset_level()
{
    //sizes
    hud_height = 70*u;
    wall = 28*u;
    gap = 230*u;
    lane_width = (width - 7*wall)/6;
    course_top = hud_height + wall;
    course_bottom = height - wall;
    radius_ball = 7*u;
    radius_pot = 11*u;

    float centre[6];
    for (int i=0; i<6; i++)
    {
        lane_x[i] = wall + i*(lane_width + wall);
        centre[i] = lane_x[i] + lane_width/2;
    }

    //frame walls
    Rectangle top_wall = {0,hud_height,width,wall};
    Rectangle bottom_wall = {0,height-wall,width,wall};
    Rectangle left_wall = {0,hud_height,wall,height-hud_height};
    Rectangle right_wall = {width-wall,hud_height,wall,height-hud_height};
    walls[0] = top_wall;
    walls[1] = bottom_wall;
    walls[2] = left_wall;
    walls[3] = right_wall;

    //divider walls, the turn gap is at the top after lane 1, 3, 5 and at the bottom after lane 2, 4
    for (int i=0; i<5; i++)
    {
        Rectangle divider = {lane_x[i]+lane_width,course_top,wall,course_bottom-course_top-gap};
        if (i%2==0) divider.y = course_top + gap;
        walls[4+i] = divider;
    }

    //ball and pot
    start_position.x = centre[0];
    start_position.y = course_bottom - 70*u;
    ball = start_position;
    last_shot_position = start_position;
    speed.x = 0;
    speed.y= 0;
    pot.x = centre[5];
    pot.y = course_bottom - 110*u;
    stroke = 0;
    game_state = 0;
    aiming = 0;
    message_timer = 0;

    //lane 1: crate, valve wheel, conveyor pushing down
    crate.x = centre[0] - 68*u;
    crate.y = course_bottom - 233*u;
    crate.width = 46*u;
    crate.height = 46*u;
    bumper_radius = 26*u;
    bumper[0].x = centre[0] + 55*u;
    bumper[0].y = course_bottom - 360*u;
    belt[0].x = lane_x[0];
    belt[0].y = course_top + 330*u;
    belt[0].width = lane_width;
    belt[0].height = 64*u;
    belt_push[0] = 750*u;

    //lane 2: diamond, two girders, oil slick
    diamond_radius = 34*u;
    diamond.x = centre[1];
    diamond.y = course_top + 330*u;
    girder[0].x = lane_x[1];
    girder[0].y = course_top + 500*u;
    girder[0].width = 150*u;
    girder[0].height = 18*u;
    girder[1].x = lane_x[1] + lane_width - 150*u;
    girder[1].y = course_top + 640*u;
    girder[1].width = 150*u;
    girder[1].height = 18*u;
    oil_radius = 70*u;
    oil.x = centre[1];
    oil.y = course_bottom - 170*u;

    //lane 3: crane beam, spinning fan
    beam[0].x = lane_x[2] + 42*u;
    beam[0].y = course_bottom - 330*u;
    fan_blade_length = 190*u;
    fan_blade_thickness = 12*u;
    fan_hub_radius = 16*u;
    fan.x = centre[2];
    fan.y = course_top + 380*u;
    fan_angle = 0;

    //lane 4: conveyor pushing up, piston, magnet
    belt[1].x = lane_x[3];
    belt[1].y = course_top + 250*u;
    belt[1].width = lane_width;
    belt[1].height = 64*u;
    belt_push[1] = -750*u;
    piston_head = 30*u;
    piston.x = lane_x[3];
    piston.y = course_top + 435*u;
    piston.width = piston_head;
    piston.height = 70*u;
    piston_max_length = lane_width - piston_head - 42*u;
    piston_length = 0;
    piston_speed = 0;
    piston_timer = 0;
    piston_state = 0;
    magnet_core = 22*u;
    magnet_field = 170*u;
    magnet_pull = 450*u;
    magnet.x = centre[3];
    magnet.y = course_top + 720*u;

    //lane 5: two molten pits, laser gate
    pit_radius = 38*u;
    pit[0].x = centre[4] - 55*u;
    pit[0].y = course_bottom - 330*u;
    pit[1].x = centre[4] + 55*u;
    pit[1].y = course_top + 400*u;
    laser.x = lane_x[4];
    laser.y = course_top + 256*u;
    laser.width = lane_width;
    laser.height = 8*u;
    laser_timer = 0;

    //lane 6: two crane beams, molten pit, two bumpers guarding the pot
    beam[1].x = lane_x[5] + 42*u;
    beam[1].y = course_top + 300*u;
    beam[2].x = lane_x[5] + lane_width - 42*u - 110*u;
    beam[2].y = course_top + 440*u;
    pit[2].x = centre[5];
    pit[2].y = course_bottom - 300*u;
    bumper[1].x = centre[5] - 75*u;
    bumper[1].y = course_bottom - 190*u;
    bumper[2].x = centre[5] + 75*u;
    bumper[2].y = course_bottom - 190*u;

    //same size and speed for all crane beams
    for (int i=0; i<3; i++)
    {
        beam[i].width = 110*u;
        beam[i].height = 18*u;
        int lane = 2;
        if (i>0) lane = 5;
        beam_left_limit[i] = lane_x[lane] + 42*u;
        beam_right_limit[i] = lane_x[lane] + lane_width - 42*u - beam[i].width;
    }
    beam_speed[0] = 110*u;
    beam_speed[1] = 110*u;
    beam_speed[2] = -110*u;
}

//bounce off a straight rectangle, rec_speed is how fast the rectangle itself is moving
void bounce_off_rectangle(Rectangle rec, Vector2 rec_speed)
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

            //moving beam or piston side: also knock the ball out of its row, or it gets hit again and again
            if (rec_speed.x!=0 && normal.y==0)
            {
                if (ball.y < rec.y+rec.height/2) speed.y = speed.y - fabsf(rec_speed.x)/2;
                else speed.y = speed.y + fabsf(rec_speed.x)/2;
            }
            speed = Vector2ClampValue(speed,0,max_speed*u);
        }
    }
}

//bounce off a round thing that doesn't move
void bounce_off_circle(Vector2 center, float radius)
{
    Vector2 normal = Vector2Subtract(ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        ball = Vector2Add(center,Vector2Scale(normal,radius + radius_ball));
        if (Vector2DotProduct(speed,normal)<0) speed = Vector2Reflect(speed,normal);
    }
}

//bounce off a turned rectangle (diamond, fan blades), spin is in degrees per second
void bounce_off_rotated_rectangle(Vector2 center, float rec_width, float rec_height, float angle, float spin)
{
    //look at the ball as if the rectangle was not turned
    Vector2 local_ball = Vector2Rotate(Vector2Subtract(ball,center),-angle*DEG2RAD);
    Rectangle rec = {-rec_width/2,-rec_height/2,rec_width,rec_height};
    if (CheckCollisionCircleRec(local_ball,radius_ball,rec))
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
        ball = Vector2Add(collision_point,Vector2Scale(normal,radius_ball));

        //speed of the blade at the point it touches the ball
        Vector2 arm = Vector2Subtract(collision_point,center);
        Vector2 rec_speed = {-arm.y*spin*DEG2RAD, arm.x*spin*DEG2RAD};

        Vector2 relative_speed = Vector2Subtract(speed,rec_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Reflect(relative_speed,normal);
            speed = Vector2ClampValue(Vector2Add(relative_speed,rec_speed),0,max_speed*u);
        }
    }
}


void update_obstacles(float dt)
{
    //crane beams moving
    for (int i=0; i<3; i++)
    {
        beam[i].x = beam[i].x + beam_speed[i]*dt;
        if (beam[i].x < beam_left_limit[i])
        {
            beam[i].x = beam_left_limit[i];
            beam_speed[i] = fabsf(beam_speed[i]);
        }
        if (beam[i].x > beam_right_limit[i])
        {
            beam[i].x = beam_right_limit[i];
            beam_speed[i] = -fabsf(beam_speed[i]);
        }
    }

    //fan spinning
    fan_angle = fan_angle + fan_spin*dt;
    if (fan_angle>=360) fan_angle = fan_angle - 360;

    //laser: off 1.2s, warning 0.4s, on 1.4s
    laser_timer = laser_timer + dt;
    if (laser_timer>=3.0) laser_timer = laser_timer - 3.0;

    //piston: 0 waiting, 1 going out, 2 holding, 3 going back
    piston_timer = piston_timer + dt;
    piston_speed = 0;
    if (piston_state==0)
    {
        if (piston_timer>=1.2)
        {
            piston_state = 1;
            piston_timer = 0;
        }
    }
    else if (piston_state==1)
    {
        piston_length = piston_length + 900*u*dt;
        piston_speed = 900*u;
        if (piston_length>=piston_max_length)
        {
            piston_length = piston_max_length;
            piston_state = 2;
            piston_timer = 0;
        }
    }
    else if (piston_state==2)
    {
        if (piston_timer>=0.3)
        {
            piston_state = 3;
            piston_timer = 0;
        }
    }
    else if (piston_state==3)
    {
        piston_length = piston_length - 250*u*dt;
        piston_speed = -250*u;
        if (piston_length<=0)
        {
            piston_length = 0;
            piston_state = 0;
            piston_timer = 0;
        }
    }
    piston.width = piston_head + piston_length;
}


//after lava or laser, the ball goes back to where it was shot from
void send_ball_back()
{
    ball = last_shot_position;

    //not inside the laser
    if (CheckCollisionCircleRec(ball,radius_ball,laser)) ball = start_position;

    //not right next to the piston's path, or it gets hit into the lava again and again
    if (ball.x>piston.x && ball.x<piston.x+piston_head+piston_max_length+radius_ball && ball.y>piston.y-radius_ball-1*u && ball.y<piston.y+piston.height+radius_ball+1*u)
    {
        if (ball.y < piston.y+piston.height/2) ball.y = piston.y - radius_ball - 2*u;
        else ball.y = piston.y + piston.height + radius_ball + 2*u;
    }

    speed.x = 0;
    speed.y = 0;
}


void update_ball(float dt)
{
    int pushed = 0;

    //conveyor belts
    for (int i=0; i<2; i++)
    {
        if (ball.x>belt[i].x && ball.x<belt[i].x+belt[i].width && ball.y>belt[i].y && ball.y<belt[i].y+belt[i].height)
        {
            speed.y = speed.y + belt_push[i]*dt;
            pushed = 1;
        }
    }

    //magnet pull
    Vector2 to_magnet = Vector2Subtract(magnet,ball);
    float magnet_distance = Vector2Length(to_magnet);
    if (magnet_distance<magnet_field && magnet_distance>magnet_core+radius_ball+1*u)
    {
        speed = Vector2Add(speed,Vector2Scale(Vector2Normalize(to_magnet),magnet_pull*dt));
        pushed = 1;
    }

    //moving
    ball = Vector2Add(ball,Vector2Scale(speed,dt));

    //proportional deceleration (slows the whole speed, so the direction stays the same)
    float friction = 110*u;
    if (CheckCollisionPointCircle(ball,oil,oil_radius)) friction = 20*u;
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

    //molten pits (checked before the collisions, so if a moving thing is on the old spot it pushes the ball out)
    for (int i=0; i<3; i++)
    {
        if (Vector2Length(Vector2Subtract(ball,pit[i])) < pit_radius - 5*u)
        {
            send_ball_back();
            message_type = 1;
            message_timer = 1;
        }
    }

    //laser gate
    if (laser_timer>=1.6 && CheckCollisionCircleRec(ball,radius_ball,laser))
    {
        send_ball_back();
        message_type = 2;
        message_timer = 1;
    }

    //collision with things that don't move
    for (int i=0; i<3; i++)
    {
        bounce_off_circle(bumper[i],bumper_radius);
    }
    bounce_off_rectangle(crate,no_speed);
    for (int i=0; i<2; i++)
    {
        bounce_off_rectangle(girder[i],no_speed);
    }
    bounce_off_rotated_rectangle(diamond,diamond_radius*sqrt(2),diamond_radius*sqrt(2),45,0);

    //collision with things that move
    bounce_off_circle(fan,fan_hub_radius);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle,fan_spin);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle+90,fan_spin);
    for (int i=0; i<3; i++)
    {
        Vector2 beam_velocity = {beam_speed[i],0};
        bounce_off_rectangle(beam[i],beam_velocity);
    }
    Vector2 piston_velocity = {piston_speed,0};
    bounce_off_rectangle(piston,piston_velocity);

    //magnet core, the ball sticks to it
    Vector2 away_from_magnet = Vector2Subtract(ball,magnet);
    float distance = Vector2Length(away_from_magnet);
    if (distance < magnet_core + radius_ball)
    {
        if (distance==0) away_from_magnet.y = -1;
        away_from_magnet = Vector2Normalize(away_from_magnet);
        ball = Vector2Add(magnet,Vector2Scale(away_from_magnet,magnet_core + radius_ball));
        if (Vector2DotProduct(speed,away_from_magnet)<0)
        {
            speed.x = 0;
            speed.y = 0;
        }
    }

    //wall bounce (last, so the ball never ends inside a wall)
    for (int i=0; i<9; i++)
    {
        bounce_off_rectangle(walls[i],no_speed);
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


//yellow and black bands, across the long side
void draw_hazard_stripes(Rectangle rec)
{
    DrawRectangleRec(rec,hazard_yellow);
    float band = 6*u;
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


void draw_background()
{
    ClearBackground(steel_dark);

    //lane floor
    DrawRectangle(wall,course_top,width-2*wall,course_bottom-course_top,floor_colour);

    //diamond plate tread marks
    int columns = (width-2*wall)/(34*u);
    int rows = (course_bottom-course_top)/(34*u);
    for (int i=0; i<=columns; i++)
    {
        for (int j=0; j<=rows; j++)
        {
            float x = wall + i*34*u + 17*u;
            float y = course_top + j*34*u + 17*u;
            Vector2 mark_start = {x-6*u,y-6*u};
            Vector2 mark_end = {x+6*u,y+6*u};
            if ((i+j)%2==1)
            {
                mark_start.y = y+6*u;
                mark_end.y = y-6*u;
            }
            DrawLineEx(mark_start,mark_end,4*u,Fade(BLACK,0.15));
            mark_start.y = mark_start.y - 1*u;
            mark_end.y = mark_end.y - 1*u;
            DrawLineEx(mark_start,mark_end,2*u,Fade(steel_light,0.25));
        }
    }

    //painted centre dashes and shadows next to the walls
    for (int i=0; i<6; i++)
    {
        float centre = lane_x[i] + lane_width/2;
        int dashes = (course_bottom-course_top)/(60*u);
        for (int j=0; j<dashes; j++)
        {
            DrawRectangle(centre-2*u,course_top+j*60*u+17*u,4*u,26*u,Fade(hazard_yellow,0.18));
        }
        DrawRectangleGradientH(lane_x[i],course_top,20*u,course_bottom-course_top,Fade(BLACK,0.35),BLANK);
        DrawRectangleGradientH(lane_x[i]+lane_width-20*u,course_top,20*u,course_bottom-course_top,BLANK,Fade(BLACK,0.35));
    }
    DrawRectangleGradientV(wall,course_top,width-2*wall,20*u,Fade(BLACK,0.35),BLANK);
}


void draw_walls()
{
    float brick_height = 14*u;
    float brick_length = 28*u;
    for (int i=0; i<9; i++)
    {
        Rectangle rec = walls[i];
        DrawRectangleRec(rec,mortar);

        //bricks, every second row moved by half a brick
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
                if (right-left>3*u && bottom-y>3*u)
                {
                    Color colour = brick;
                    if ((row*3 + count*7)%5==0) colour = brick_dark;
                    DrawRectangle(left+1*u,y+1*u,right-left-2*u,bottom-y-2*u,colour);
                    DrawRectangle(left+1*u,bottom-3*u,right-left-2*u,2*u,Fade(BLACK,0.25));
                }
                x = x + brick_length;
                count++;
            }
        }

        //steel edge and rivets
        DrawRectangleLinesEx(rec,3*u,steel_dark);
        if (rec.height>rec.width)
        {
            int rivets = rec.height/(40*u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+5*u,rec.y+20*u+j*40*u,2.5*u,steel_light);
                DrawCircle(rec.x+rec.width-5*u,rec.y+20*u+j*40*u,2.5*u,steel_light);
            }
        }
        else
        {
            int rivets = rec.width/(40*u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+20*u+j*40*u,rec.y+5*u,2.5*u,steel_light);
                DrawCircle(rec.x+20*u+j*40*u,rec.y+rec.height-5*u,2.5*u,steel_light);
            }
        }
    }

    //hazard caps at the end of each divider
    for (int i=0; i<5; i++)
    {
        Rectangle cap = {walls[4+i].x,walls[4+i].y,wall,22*u};
        if (i%2==1) cap.y = walls[4+i].y + walls[4+i].height - 22*u;
        draw_hazard_stripes(cap);
        DrawRectangleLinesEx(cap,2*u,BLACK);
    }
}


void draw_path_arrows()
{
    float spacing = 90*u;
    float offset = fmod(animation_time*45*u,spacing);
    Color arrow_colour = Fade(hazard_yellow,0.3);

    //along the lanes
    int arrows = (course_bottom-course_top-gap)/spacing;
    for (int i=0; i<6; i++)
    {
        Vector2 arrow;
        arrow.x = lane_x[i] + lane_width/2;
        for (int j=0; j<arrows; j++)
        {
            if (i%2==0)
            {
                arrow.y = course_bottom - 115*u - j*spacing - offset;
                DrawPoly(arrow,3,10*u,270,arrow_colour);
            }
            else
            {
                arrow.y = course_top + 115*u + j*spacing + offset;
                DrawPoly(arrow,3,10*u,90,arrow_colour);
            }
        }
    }

    //across the turns
    for (int i=0; i<5; i++)
    {
        Vector2 arrow;
        arrow.y = course_top + 115*u;
        if (i%2==1) arrow.y = course_bottom - 115*u;
        for (int j=0; j<3; j++)
        {
            arrow.x = lane_x[i] + lane_width/2 + 30*u + j*spacing + offset;
            DrawPoly(arrow,3,10*u,0,arrow_colour);
        }
    }
}


void draw_obstacles()
{
    float t = animation_time;

    //oil slick
    DrawCircle(oil.x,oil.y,oil_radius*0.75,Fade(BLACK,0.55));
    DrawCircle(oil.x-30*u,oil.y+15*u,oil_radius*0.5,Fade(BLACK,0.5));
    DrawCircle(oil.x+32*u,oil.y-12*u,oil_radius*0.5,Fade(BLACK,0.5));
    DrawCircle(oil.x+8*u,oil.y+35*u,oil_radius*0.45,Fade(BLACK,0.45));
    DrawCircle(oil.x-18*u,oil.y-35*u,oil_radius*0.4,Fade(BLACK,0.45));
    DrawRing(oil,oil_radius*0.3,oil_radius*0.34,t*25,t*25+110,16,Fade(PURPLE,0.4));
    DrawRing(oil,oil_radius*0.5,oil_radius*0.54,90-t*18,200-t*18,16,Fade(SKYBLUE,0.3));
    DrawRing(oil,oil_radius*0.66,oil_radius*0.69,200+t*12,290+t*12,16,Fade(VIOLET,0.35));

    //conveyor belts
    for (int i=0; i<2; i++)
    {
        Rectangle b = belt[i];
        DrawRectangleRec(b,GetColor(0x1E1E1EFF));
        float stripe = 16*u;
        float move = fmod(t*80*u,stripe);
        if (belt_push[i]<0) move = stripe - move;
        int stripes = b.height/stripe;
        for (int j=0; j<stripes; j++)
        {
            float y = b.y + j*stripe + move;
            if (y+4*u < b.y+b.height) DrawRectangle(b.x,y,b.width,4*u,GetColor(0x3C3C3CFF));
        }
        //rollers
        DrawRectangleGradientV(b.x,b.y-4*u,b.width,8*u,steel_light,steel_dark);
        DrawRectangleGradientV(b.x,b.y+b.height-4*u,b.width,8*u,steel_light,steel_dark);
        //arrows
        for (int j=1; j<=3; j++)
        {
            Vector2 arrow = {b.x + j*b.width/4, b.y + b.height/2};
            if (belt_push[i]>0) DrawPoly(arrow,3,12*u,90,hazard_yellow);
            else DrawPoly(arrow,3,12*u,270,hazard_yellow);
        }
    }

    //steel crate
    DrawRectangle(crate.x+4*u,crate.y+5*u,crate.width,crate.height,Fade(BLACK,0.4));
    DrawRectangleRec(crate,rust);
    DrawRectangleLinesEx(crate,5*u,rust_dark);
    Vector2 crate_corner1 = {crate.x+5*u,crate.y+5*u};
    Vector2 crate_corner2 = {crate.x+crate.width-5*u,crate.y+crate.height-5*u};
    Vector2 crate_corner3 = {crate.x+crate.width-5*u,crate.y+5*u};
    Vector2 crate_corner4 = {crate.x+5*u,crate.y+crate.height-5*u};
    DrawLineEx(crate_corner1,crate_corner2,4*u,rust_dark);
    DrawLineEx(crate_corner3,crate_corner4,4*u,rust_dark);
    DrawCircle(crate_corner1.x,crate_corner1.y,2.5*u,steel_light);
    DrawCircle(crate_corner2.x,crate_corner2.y,2.5*u,steel_light);
    DrawCircle(crate_corner3.x,crate_corner3.y,2.5*u,steel_light);
    DrawCircle(crate_corner4.x,crate_corner4.y,2.5*u,steel_light);

    //valve wheel bumpers
    for (int i=0; i<3; i++)
    {
        Vector2 b = bumper[i];
        DrawCircle(b.x+3*u,b.y+4*u,bumper_radius,Fade(BLACK,0.4));
        DrawCircle(b.x,b.y,bumper_radius,steel);
        DrawRing(b,bumper_radius-6*u,bumper_radius,0,360,32,magnet_red);
        Vector2 spoke = {bumper_radius-6*u,0};
        for (int j=0; j<4; j++)
        {
            Vector2 turned = Vector2Rotate(spoke,j*45*DEG2RAD);
            DrawLineEx(Vector2Add(b,turned),Vector2Subtract(b,turned),4*u,steel_dark);
        }
        DrawCircle(b.x,b.y,7*u,steel_dark);
        DrawCircle(b.x,b.y,3*u,steel_light);
    }

    //warning diamond
    Vector2 diamond_shadow = {diamond.x+3*u,diamond.y+4*u};
    DrawPoly(diamond_shadow,4,diamond_radius,0,Fade(BLACK,0.4));
    DrawPoly(diamond,4,diamond_radius,0,hazard_yellow);
    DrawPolyLinesEx(diamond,4,diamond_radius,0,4*u,BLACK);
    DrawText("!",diamond.x-MeasureText("!",30*u)/2,diamond.y-14*u,30*u,BLACK);

    //steel girders
    for (int i=0; i<2; i++)
    {
        Rectangle g = girder[i];
        DrawRectangle(g.x,g.y+4*u,g.width,g.height,Fade(BLACK,0.35));
        DrawRectangleRec(g,steel);
        DrawRectangle(g.x,g.y,g.width,4*u,steel_light);
        DrawRectangle(g.x,g.y+g.height-4*u,g.width,4*u,steel_dark);
        int rivets = g.width/(20*u);
        for (int j=1; j<rivets; j++)
        {
            DrawCircle(g.x+j*20*u,g.y+g.height/2,2*u,steel_light);
        }
        Rectangle girder_end = {g.x+g.width-20*u,g.y,20*u,g.height};
        if (i==1) girder_end.x = g.x;
        draw_hazard_stripes(girder_end);
        DrawRectangleLinesEx(g,2*u,BLACK);
    }

    //crane beams
    for (int i=0; i<3; i++)
    {
        DrawRectangle(beam[i].x+5*u,beam[i].y+6*u,beam[i].width,beam[i].height,Fade(BLACK,0.4));
        draw_hazard_stripes(beam[i]);
        DrawRectangleLinesEx(beam[i],3*u,BLACK);
        DrawCircle(beam[i].x+beam[i].width/2,beam[i].y+beam[i].height/2,4*u,steel_dark);
    }

    //spinning fan
    DrawCircle(fan.x,fan.y,fan_blade_length/2+10*u,Fade(BLACK,0.3));
    DrawRing(fan,fan_blade_length/2+4*u,fan_blade_length/2+10*u,0,360,48,steel_dark);
    DrawRing(fan,fan_blade_length/2+8*u,fan_blade_length/2+10*u,0,360,48,steel_light);
    Rectangle blade = {fan.x,fan.y,fan_blade_length,fan_blade_thickness};
    Vector2 blade_origin = {fan_blade_length/2,fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,fan_angle-24,Fade(steel_light,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-24,Fade(steel_light,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle-12,Fade(steel_light,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-12,Fade(steel_light,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle,steel_light);
    DrawRectanglePro(blade,blade_origin,fan_angle+90,steel_light);
    DrawCircle(fan.x,fan.y,fan_hub_radius,steel_dark);
    DrawCircleLines(fan.x,fan.y,fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(fan,Vector2Rotate(bolt_arm,(fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*u,steel_light);
    }

    //hydraulic piston
    Rectangle housing = {piston.x-wall,piston.y-8*u,wall,piston.height+16*u};
    DrawRectangleRec(housing,steel_dark);
    for (int j=0; j<4; j++)
    {
        DrawRectangle(housing.x,housing.y+8*u+j*20*u,housing.width,5*u,steel);
    }
    DrawRectangleLinesEx(housing,2*u,BLACK);
    DrawRectangleGradientV(piston.x,piston.y,piston_length,piston.height,steel,steel_dark);
    DrawRectangleGradientV(piston.x,piston.y+piston.height/2-10*u,piston_length,20*u,RAYWHITE,steel_light);
    Rectangle head = {piston.x+piston_length,piston.y,piston_head,piston.height};
    draw_hazard_stripes(head);
    DrawRectangleLinesEx(head,3*u,BLACK);
    DrawRectangleLinesEx(piston,2*u,BLACK);
    Color lamp = GetColor(0x4A3A20FF);
    if (piston_state==0 && piston_timer>=0.8 && fmod(t*8,2)<1) lamp = hazard_yellow;
    if (piston_state==1 || piston_state==2) lamp = laser_red;
    DrawCircle(housing.x+wall/2,housing.y-12*u,9*u,Fade(lamp,0.35));
    DrawCircle(housing.x+wall/2,housing.y-12*u,6*u,lamp);

    //molten pits
    for (int i=0; i<3; i++)
    {
        Vector2 p = pit[i];
        float pulse = sin(t*3 + i*2);
        DrawCircleGradient(p,pit_radius*1.9,Fade(molten_orange,0.3+0.1*pulse),BLANK);
        DrawCircle(p.x,p.y,pit_radius+5*u,GetColor(0x2A1A12FF));
        DrawCircleGradient(p,pit_radius,molten_orange,molten_dark);
        DrawCircleGradient(p,pit_radius*(0.55+0.08*pulse),molten_yellow,Fade(molten_orange,0));
        Vector2 lump_arm = {pit_radius+1*u,0};
        for (int j=0; j<8; j++)
        {
            Vector2 lump = Vector2Add(p,Vector2Rotate(lump_arm,(j*45+i*20)*DEG2RAD));
            DrawCircle(lump.x,lump.y,3.5*u,GetColor(0x3B2418FF));
        }
        //bubbles
        for (int j=0; j<4; j++)
        {
            float life = fmod(t*0.8 + j*0.25 + i*0.13,1.0);
            float bubble_x = p.x + sin(j*2.1 + i)*pit_radius*0.5;
            float bubble_y = p.y + cos(j*1.7 + i)*pit_radius*0.5;
            DrawCircle(bubble_x,bubble_y,(1.5+life*3.5)*u,Fade(molten_yellow,(1-life)*0.9));
        }
    }

    //laser gate
    float beam_y = laser.y + laser.height/2;
    Rectangle left_emitter = {laser.x-wall+4*u,beam_y-14*u,wall-4*u,28*u};
    Rectangle right_emitter = {laser.x+laser.width,beam_y-14*u,wall-4*u,28*u};
    DrawRectangleRec(left_emitter,steel_dark);
    DrawRectangleRec(right_emitter,steel_dark);
    DrawRectangleLinesEx(left_emitter,2*u,BLACK);
    DrawRectangleLinesEx(right_emitter,2*u,BLACK);
    Color laser_lamp = GetColor(0x4A3A20FF);
    int dashes = laser.width/(16*u);
    if (laser_timer<1.6)
    {
        float dash_alpha = 0.3;
        if (laser_timer>=1.2)
        {
            dash_alpha = 0.55;
            if (fmod(t*10,2)<1) laser_lamp = hazard_yellow;
        }
        for (int j=0; j<dashes; j++)
        {
            DrawRectangle(laser.x+j*16*u+4*u,beam_y-1*u,8*u,2*u,Fade(laser_red,dash_alpha));
        }
    }
    else
    {
        laser_lamp = laser_red;
        DrawRectangle(laser.x,beam_y-14*u,laser.width,28*u,Fade(laser_red,0.15));
        DrawRectangle(laser.x,beam_y-7*u,laser.width,14*u,Fade(laser_red,0.3));
        DrawRectangleRec(laser,laser_red);
        DrawRectangle(laser.x,beam_y-1*u,laser.width,2*u,WHITE);
    }
    DrawCircle(left_emitter.x+left_emitter.width/2,beam_y,5*u,laser_lamp);
    DrawCircle(right_emitter.x+right_emitter.width/2,beam_y,5*u,laser_lamp);

    //magnet field rings
    for (int j=0; j<3; j++)
    {
        float ring = magnet_field - fmod(t*50*u + j*(magnet_field-magnet_core)/3,magnet_field-magnet_core);
        DrawRing(magnet,ring-1.5*u,ring+1.5*u,0,360,48,Fade(SKYBLUE,0.05+0.35*(1-ring/magnet_field)));
    }
    DrawCircle(magnet.x,magnet.y,magnet_field,Fade(SKYBLUE,0.04));

    //magnet
    Vector2 magnet_shadow = {magnet.x+3*u,magnet.y+4*u};
    DrawRing(magnet_shadow,12*u,28*u,180,360,24,Fade(BLACK,0.4));
    DrawRing(magnet,12*u,28*u,180,360,24,magnet_red);
    DrawRectangle(magnet.x-28*u,magnet.y,16*u,8*u,magnet_red);
    DrawRectangle(magnet.x+12*u,magnet.y,16*u,8*u,magnet_red);
    DrawRectangle(magnet.x-28*u,magnet.y+8*u,16*u,10*u,steel_light);
    DrawRectangle(magnet.x+12*u,magnet.y+8*u,16*u,10*u,steel_light);
}


void draw_ball_and_pot()
{
    //start plate
    Rectangle plate = {start_position.x-50*u,start_position.y-30*u,100*u,60*u};
    DrawRectangleRec(plate,steel);
    DrawRectangleLinesEx(plate,3*u,steel_dark);
    Rectangle plate_band = {plate.x,plate.y+plate.height-10*u,plate.width,10*u};
    draw_hazard_stripes(plate_band);
    DrawText("START",start_position.x-MeasureText("START",18*u)/2,plate.y+5*u,18*u,WHITE);

    //pot with hazard ring
    for (int i=0; i<8; i++)
    {
        Color colour = hazard_yellow;
        if (i%2==1) colour = BLACK;
        DrawRing(pot,radius_pot+6*u,radius_pot+12*u,i*45,i*45+45,6,colour);
    }
    DrawCircle(pot.x,pot.y,radius_pot+6*u,steel_light);
    DrawRing(pot,radius_pot+1*u,radius_pot+5*u,0,360,24,steel);
    DrawCircle(pot.x,pot.y,radius_pot,BLACK);

    //flag
    Vector2 pole_top = {pot.x,pot.y-60*u};
    DrawLineEx(pot,pole_top,3*u,steel_light);
    for (int i=0; i<5; i++)
    {
        float wave = sin(animation_time*6 - i*0.8)*2.5*u;
        DrawRectangle(pot.x+1*u+i*6*u,pole_top.y+wave,6*u,18*u,laser_red);
    }

    //ball
    DrawCircle(ball.x+3*u,ball.y+4*u,radius_ball,Fade(BLACK,0.45));
    DrawCircle(ball.x,ball.y,radius_ball,GetColor(0xEDEDEDFF));
    DrawCircleLines(ball.x,ball.y,radius_ball,GRAY);
    DrawCircle(ball.x-2*u,ball.y-2*u,2*u,WHITE);

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
    DrawRectangleGradientH(0,hud_height,90*u,height-hud_height,Fade(BLACK,0.45),BLANK);
    DrawRectangleGradientH(width-90*u,hud_height,90*u,height-hud_height,BLANK,Fade(BLACK,0.45));
    DrawRectangleGradientV(0,height-70*u,width,70*u,BLANK,Fade(BLACK,0.45));

    //steel bar
    DrawRectangleGradientV(0,0,width,hud_height,steel_light,steel_dark);
    DrawRectangle(0,hud_height-4*u,width,4*u,BLACK);
    int rivets = width/(40*u);
    for (int i=0; i<rivets; i++)
    {
        DrawCircle(20*u+i*40*u,8*u,3*u,steel_dark);
        DrawCircle(20*u+i*40*u,hud_height-12*u,3*u,steel_dark);
    }

    //text
    DrawText("LEVEL 1 - FOUNDRY",32*u,20*u,36*u,BLACK);
    DrawText("LEVEL 1 - FOUNDRY",30*u,18*u,36*u,hazard_yellow);
    DrawText(TextFormat("STROKES %d / %d",stroke,stroke_limit),width/2-298*u,21*u,32*u,BLACK);
    DrawText(TextFormat("STROKES %d / %d",stroke,stroke_limit),width/2-300*u,19*u,32*u,WHITE);
    for (int i=0; i<stroke_limit; i++)
    {
        Color pip = steel_dark;
        if (i<stroke) pip = laser_red;
        DrawCircle(width/2+10*u+i*22*u,35*u,7*u,pip);
        DrawCircleLines(width/2+10*u+i*22*u,35*u,7*u,BLACK);
    }
    int help_width = MeasureText("R restart   ESC quit",26*u);
    DrawText("R restart   ESC quit",width-help_width-28*u,24*u,26*u,BLACK);
    DrawText("R restart   ESC quit",width-help_width-30*u,22*u,26*u,WHITE);

    //melted or zapped message
    if (message_timer>0)
    {
        if (message_type==1)
        {
            DrawText("MELTED!",width/2-MeasureText("MELTED!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("MELTED!",width/2-MeasureText("MELTED!",90*u)/2,height/2-45*u,90*u,Fade(molten_orange,message_timer));
        }
        else
        {
            DrawText("ZAPPED!",width/2-MeasureText("ZAPPED!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("ZAPPED!",width/2-MeasureText("ZAPPED!",90*u)/2,height/2-45*u,90*u,Fade(laser_red,message_timer));
        }
    }

    //level clear or failed
    if (game_state!=0)
    {
        DrawRectangle(0,0,width,height,Fade(BLACK,0.6));
        Rectangle panel = {width/2-340*u,height/2-170*u,680*u,340*u};
        DrawRectangleRec(panel,steel_dark);
        Rectangle panel_band = {panel.x,panel.y,panel.width,24*u};
        draw_hazard_stripes(panel_band);
        DrawRectangleLinesEx(panel,6*u,steel_light);
        if (game_state==1)
        {
            DrawText("LEVEL CLEAR!",width/2-MeasureText("LEVEL CLEAR!",80*u)/2,panel.y+60*u,80*u,hazard_yellow);
        }
        else
        {
            DrawText("FAILED",width/2-MeasureText("FAILED",80*u)/2,panel.y+60*u,80*u,laser_red);
        }
        int strokes_width = MeasureText(TextFormat("Strokes: %d / %d",stroke,stroke_limit),40*u);
        DrawText(TextFormat("Strokes: %d / %d",stroke,stroke_limit),width/2-strokes_width/2,panel.y+170*u,40*u,WHITE);
        DrawText("Press R to play again",width/2-MeasureText("Press R to play again",30*u)/2,panel.y+250*u,30*u,steel_light);
    }
}


int main()
{
    InitWindow(1280,720,"the ultimate golf - level 1");
    int monitor = GetCurrentMonitor();
    width = GetMonitorWidth(monitor);
    height = GetMonitorHeight(monitor);
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

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

        draw_background();
        draw_path_arrows();
        draw_walls();
        draw_obstacles();
        draw_ball_and_pot();
        draw_hud();

        EndDrawing();
    }
    CloseWindow();


    return 0;
}
