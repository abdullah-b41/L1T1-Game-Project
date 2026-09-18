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
Color stone = {120,126,112,255};
Color stone_dark = {84,90,78,255};
Color stone_mortar = {52,58,48,255};
Color moss_green = {94,127,58,255};
Color jungle_dark = {24,60,32,255};
Color gold = {232,184,64,255};
Color gold_light = {255,224,130,255};
Color river_tint = {150,215,170,255};
Color wood_brown = {120,80,40,255};

//pictures
Texture2D ground_texture;
Texture2D temple_texture;
Texture2D foliage_texture;
Texture2D plank_texture;
Texture2D water_texture;
Texture2D foam_texture;
Texture2D splash_texture;
Texture2D boulder_texture;

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
float hud_height;
Vector2 no_speed = {0,0};

//walls: outer frame, temple walls, altar room walls, and the 2 jungle hedges (last two)
Rectangle walls[13];
Rectangle temple_area;

//river (fall in = chomp) and the plank bridges: 0 normal one at the top, 1 risky shortcut at the bottom
Rectangle river;
Rectangle plank[2][5];
float plank_timer[2][5];
float crack_time[2];
Vector2 bridge_bank[2];

//mud (slow), moss (slippery), quicksand (slow, and a ball that stays still sinks)
Rectangle mud;
Rectangle moss[2];
Rectangle quicksand[2];
float sink_timer = 0;

//boulders rolling back and forth (the level 3 asteroids)
Vector2 boulder[3];
Vector2 boulder_start[3];
Vector2 boulder_end[3];
Vector2 boulder_velocity[3];
float boulder_t[3];
float boulder_time[3];
float boulder_radius[3];
int boulder_direction[3];

//dart traps (the level 1 laser): off 1.2s, warning 0.4s, on 1.4s
Rectangle dart_gate[4];
float dart_clock[4];

//spinning totem (the level 1 fan)
Vector2 fan;
float fan_angle = 0;
float fan_spin = 100;
float fan_blade_length;
float fan_blade_thickness;
float fan_hub_radius;

//mushrooms (the level 3 bumpers)
Vector2 bumper[6];
float bumper_radius;
float bumper_hit[6];

//pressure plates (0 opens the temple gate for 6s, 1 and 2 open the altar door) and the 2 stone doors
Vector2 plate[3];
float plate_radius;
int plate_down[3];
float gate_timer = 0;
Rectangle door[2];
float door_closed_y[2];
float door_open[2];
Vector2 door_velocity[2];

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

    //walls
    walls[0] = make_rect(0,70,1920,28);        //frame top
    walls[1] = make_rect(0,1052,1920,28);      //frame bottom
    walls[2] = make_rect(0,70,28,1010);        //frame left
    walls[3] = make_rect(1892,70,28,1010);     //frame right
    walls[4] = make_rect(1152,98,28,422);      //temple outer wall, above the gate
    walls[5] = make_rect(1152,620,28,432);     //temple outer wall, below the gate
    walls[6] = make_rect(1380,300,348,28);     //altar room top
    walls[7] = make_rect(1380,822,348,28);     //altar room bottom
    walls[8] = make_rect(1380,300,28,550);     //altar room left
    walls[9] = make_rect(1700,300,28,220);     //altar room right, above the door
    walls[10] = make_rect(1700,620,28,230);    //altar room right, below the door
    walls[11] = make_rect(190,700,430,40);     //jungle hedge, low
    walls[12] = make_rect(28,400,430,40);      //jungle hedge, high
    temple_area = make_rect(1152,98,740,954);

    //river and bridges
    river = make_rect(620,98,140,954);
    for (int i=0; i<5; i++)
    {
        plank[0][i] = make_rect(620+i*28,165,28,90);
        plank[1][i] = make_rect(620+i*28,915,28,90);
        plank_timer[0][i] = 0;
        plank_timer[1][i] = 0;
    }
    crack_time[0] = 0.5;
    crack_time[1] = 0.2;
    bridge_bank[0] = make_point(590,210);
    bridge_bank[1] = make_point(590,960);

    //ground patches
    mud = make_rect(250,850,180,140);
    moss[0] = make_rect(770,260,130,180);
    moss[1] = make_rect(1740,330,140,170);
    quicksand[0] = make_rect(850,720,200,150);
    quicksand[1] = make_rect(1200,120,160,160);
    sink_timer = 0;

    //boulders
    boulder_start[0] = make_point(100,570);
    boulder_end[0] = make_point(560,570);
    boulder_radius[0] = 30*u;
    boulder_time[0] = 3.0;
    boulder_start[1] = make_point(1070,420);
    boulder_end[1] = make_point(830,420);
    boulder_radius[1] = 28*u;
    boulder_time[1] = 2.2;
    boulder_start[2] = make_point(1280,380);
    boulder_end[2] = make_point(1280,760);
    boulder_radius[2] = 30*u;
    boulder_time[2] = 2.6;
    for (int i=0; i<3; i++)
    {
        boulder_t[i] = 0;
        boulder_direction[i] = 1;
        boulder[i] = boulder_start[i];
        boulder_velocity[i] = no_speed;
    }

    //dart traps
    dart_gate[0] = make_rect(1180,330,200,8);
    dart_gate[1] = make_rect(1180,820,200,8);
    dart_gate[2] = make_rect(1540,98,8,202);
    dart_gate[3] = make_rect(1540,850,8,202);
    for (int i=0; i<4; i++) dart_clock[i] = i*0.75;

    //totem in front of the altar door
    fan = make_point(1810,570);
    fan_blade_length = 80*u;
    fan_blade_thickness = 12*u;
    fan_hub_radius = 12*u;
    fan_angle = 0;

    //mushrooms
    bumper_radius = 22*u;
    bumper[0] = make_point(250,220);
    bumper[1] = make_point(430,300);
    bumper[2] = make_point(450,820);
    bumper[3] = make_point(1060,960);
    bumper[4] = make_point(1480,470);
    bumper[5] = make_point(1620,690);
    for (int i=0; i<6; i++) bumper_hit[i] = 0;

    //plates and doors
    plate_radius = 26*u;
    plate[0] = make_point(1000,570);
    plate[1] = make_point(1810,200);
    plate[2] = make_point(1810,950);
    for (int i=0; i<3; i++) plate_down[i] = 0;
    gate_timer = 0;
    door[0] = make_rect(1152,520,28,100);
    door[1] = make_rect(1700,520,28,100);
    for (int i=0; i<2; i++)
    {
        door_closed_y[i] = door[i].y;
        door_open[i] = 0;
        door_velocity[i] = no_speed;
    }

    //ball and pot
    start_position = make_point(120,980);
    ball = start_position;
    last_shot_position = start_position;
    speed.x = 0;
    speed.y= 0;
    pot = make_point(1560,575);
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

//bounce off a round thing: bounce is 1 for normal and more for bouncy, circle_speed is how fast it moves
int bounce_off_circle(Vector2 center, float radius, float bounce, Vector2 circle_speed)
{
    Vector2 normal = Vector2Subtract(ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + radius_ball)
    {
        if (distance==0) normal.y = -1;
        normal = Vector2Normalize(normal);
        ball = Vector2Add(center,Vector2Scale(normal,radius + radius_ball));
        Vector2 relative_speed = Vector2Subtract(speed,circle_speed);
        if (Vector2DotProduct(relative_speed,normal)<0)
        {
            relative_speed = Vector2Scale(Vector2Reflect(relative_speed,normal),bounce);
            speed = Vector2ClampValue(Vector2Add(relative_speed,circle_speed),0,max_speed*u);
            return 1;
        }
    }
    return 0;
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

//is this point in the river, and not on a plank that is still there
int in_the_river(Vector2 point)
{
    if (CheckCollisionPointRec(point,river)==0) return 0;
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (CheckCollisionPointRec(point,plank[b][i]) && plank_timer[b][i]<crack_time[b]) return 0;
        }
    }
    return 1;
}


void update_obstacles(float dt)
{
    //boulders rolling back and forth (the level 3 asteroids)
    for (int i=0; i<3; i++)
    {
        boulder_t[i] = boulder_t[i] + boulder_direction[i]*dt/boulder_time[i];
        if (boulder_t[i]>=1)
        {
            boulder_t[i] = 1;
            boulder_direction[i] = -1;
        }
        if (boulder_t[i]<=0)
        {
            boulder_t[i] = 0;
            boulder_direction[i] = 1;
        }
        boulder[i] = Vector2Lerp(boulder_start[i],boulder_end[i],boulder_t[i]);
        boulder_velocity[i] = Vector2Scale(Vector2Subtract(boulder_end[i],boulder_start[i]),boulder_direction[i]/boulder_time[i]);
    }

    //totem spinning (the level 1 fan)
    fan_angle = fan_angle + fan_spin*dt;
    if (fan_angle>=360) fan_angle = fan_angle - 360;

    //dart clocks
    for (int i=0; i<4; i++)
    {
        dart_clock[i] = dart_clock[i] + dt;
        if (dart_clock[i]>=3.0) dart_clock[i] = dart_clock[i] - 3.0;
    }

    //planks: cracking, then gone for 5 seconds, then back
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (plank_timer[b][i]>0) plank_timer[b][i] = plank_timer[b][i] + dt;
            if (plank_timer[b][i]>=crack_time[b]+5) plank_timer[b][i] = 0;
        }
    }

    //doors slide down into the wall in half a second, and never close on the ball (it jams)
    if (gate_timer>0) gate_timer = gate_timer - dt;
    for (int i=0; i<2; i++)
    {
        int want_open = 0;
        if (i==0 && gate_timer>0) want_open = 1;
        if (i==1 && plate_down[1]==1 && plate_down[2]==1) want_open = 1;
        Rectangle doorway = {door[i].x,door_closed_y[i],door[i].width,100*u};
        door_velocity[i] = no_speed;
        if (want_open==1 && door_open[i]<1)
        {
            door_open[i] = door_open[i] + 2*dt;
            if (door_open[i]>1) door_open[i] = 1;
            door_velocity[i].y = 200*u;
        }
        if (want_open==0 && door_open[i]>0 && CheckCollisionCircleRec(ball,radius_ball+2*u,doorway)==0)
        {
            door_open[i] = door_open[i] - 2*dt;
            if (door_open[i]<0) door_open[i] = 0;
            door_velocity[i].y = -200*u;
        }
        door[i].y = door_closed_y[i] + door_open[i]*100*u;
    }

    for (int i=0; i<6; i++)
    {
        if (bumper_hit[i]>0) bumper_hit[i] = bumper_hit[i] - dt;
    }
    if (splash_timer>0) splash_timer = splash_timer - dt;
}


//after the river, darts or quicksand, the ball goes back to where it was shot from
void send_ball_back()
{
    ball = last_shot_position;

    //not right in an boulder's path, or it gets knocked off again and again
    for (int i=0; i<3; i++)
    {
        Vector2 path = Vector2Subtract(boulder_end[i],boulder_start[i]);
        float along = Vector2DotProduct(Vector2Subtract(ball,boulder_start[i]),path)/Vector2DotProduct(path,path);
        along = Clamp(along,0,1);
        Vector2 closest = Vector2Add(boulder_start[i],Vector2Scale(path,along));
        Vector2 away = Vector2Subtract(ball,closest);
        float gap = boulder_radius[i] + radius_ball + 2*u;
        if (Vector2Length(away)<gap)
        {
            Vector2 side = {-path.y,path.x};
            side = Vector2Normalize(side);
            if (Vector2DotProduct(away,side)<0) side = Vector2Negate(side);
            ball = Vector2Add(closest,Vector2Scale(side,gap));
        }
    }

    //not where the totem arms sweep, or it gets knocked off again and again (the level 1 piston rule)
    float sweep = fan_blade_length/2 + radius_ball + 2*u;
    if (Vector2Distance(ball,fan)<sweep)
    {
        if (ball.y<fan.y) ball.y = fan.y - sweep;
        else ball.y = fan.y + sweep;
    }

    //not inside a dart trap (level 1 rule)
    for (int i=0; i<4; i++)
    {
        if (CheckCollisionCircleRec(ball,radius_ball,dart_gate[i])) ball = start_position;
    }

    //the plank it was on has fallen: back to that bridge's bank
    if (in_the_river(ball))
    {
        Vector2 target = ball;
        ball = start_position;
        for (int b=0; b<2; b++)
        {
            if (target.y>=plank[b][0].y && target.y<=plank[b][0].y+plank[b][0].height) ball = bridge_bank[b];
        }
    }

    //falling in or getting hit lets both altar plates back up
    plate_down[1] = 0;
    plate_down[2] = 0;
    sink_timer = 0;

    speed.x = 0;
    speed.y = 0;
}


void update_ball(float dt)
{
    int pushed = 0;

    //moving
    ball = Vector2Add(ball,Vector2Scale(speed,dt));

    //proportional deceleration (mud and quicksand are slow, moss is slippery)
    float friction = 110*u;
    int in_quicksand = 0;
    if (CheckCollisionPointRec(ball,mud)) friction = 400*u;
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(ball,moss[i])) friction = 25*u;
        if (CheckCollisionPointRec(ball,quicksand[i]))
        {
            friction = 400*u;
            in_quicksand = 1;
        }
    }
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

    //planks start cracking when the ball rolls onto them
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            if (plank_timer[b][i]==0 && CheckCollisionPointRec(ball,plank[b][i])) plank_timer[b][i] = 0.001;
        }
    }

    //pressure plates
    if (Vector2Distance(ball,plate[0])<plate_radius) gate_timer = 6;
    for (int i=1; i<3; i++)
    {
        if (Vector2Distance(ball,plate[i])<plate_radius) plate_down[i] = 1;
    }

    //river (checked before the collisions, so if something is on the old spot it pushes the ball out)
    if (in_the_river(ball))
    {
        splash_position = ball;
        splash_timer = 0.5;
        send_ball_back();
        message_type = 1;
        message_timer = 1;
    }

    //quicksand: a ball that stays still sinks in 3 seconds
    if (in_quicksand==1 && speed.x==0 && speed.y==0)
    {
        sink_timer = sink_timer + dt;
        if (sink_timer>=3)
        {
            send_ball_back();
            message_type = 3;
            message_timer = 1;
        }
    }
    else sink_timer = 0;

    //dart traps (the level 1 laser)
    for (int i=0; i<4; i++)
    {
        if (dart_clock[i]>=1.6 && CheckCollisionCircleRec(ball,radius_ball,dart_gate[i]))
        {
            send_ball_back();
            message_type = 2;
            message_timer = 1;
        }
    }

    //mushrooms and boulders
    for (int i=0; i<6; i++)
    {
        if (bounce_off_circle(bumper[i],bumper_radius,1.3,no_speed)) bumper_hit[i] = 0.15;
    }
    for (int i=0; i<3; i++)
    {
        bounce_off_circle(boulder[i],boulder_radius[i],1,boulder_velocity[i]);
    }

    //spinning totem (the level 1 fan)
    bounce_off_circle(fan,fan_hub_radius,1,no_speed);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle,fan_spin);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle+90,fan_spin);

    //stone doors
    for (int i=0; i<2; i++)
    {
        bounce_off_rectangle(door[i],door_velocity[i]);
    }

    //walls (last, so the ball never ends inside a wall)
    for (int i=0; i<13; i++)
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

//a picture that repeats (made at 2x size), lined up with the screen so pieces join without seams
void draw_tiled(Texture2D texture, Rectangle rec, Color tint)
{
    Rectangle source = {rec.x/u*2,rec.y/u*2,rec.width/u*2,rec.height/u*2};
    Vector2 no_origin = {0,0};
    DrawTexturePro(texture,source,rec,no_origin,0,tint);
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

    //jungle floor everywhere, temple floor inside the temple
    Rectangle everything = {0,0,width,height};
    draw_tiled(ground_texture,everything,WHITE);

    //river: the beach water tinted green, foam along both banks
    int water_frame = (int)(t*2)%2;
    Rectangle water_source = {water_frame*512,0,512,512};
    BeginScissorMode(river.x,river.y,river.width,river.height);
    int rows = river.height/(256*u) + 1;
    for (int j=0; j<rows; j++)
    {
        Rectangle tile = {river.x,river.y+j*256*u,256*u,256*u};
        DrawTexturePro(water_texture,water_source,tile,no_origin,0,river_tint);
    }
    EndScissorMode();
    Rectangle left_bank = make_rect(28,98,592,954);
    Rectangle right_bank = make_rect(760,98,392,954);
    draw_foam(left_bank,Fade(WHITE,0.8));
    draw_foam(right_bank,Fade(WHITE,0.8));
    draw_tiled(temple_texture,temple_area,WHITE);

    //mud, with ripples
    draw_tiled(ground_texture,mud,GetColor(0x8C6440FF));
    for (int j=1; j<mud.height/(16*u); j++)
    {
        Vector2 a = {mud.x+10*u,mud.y+j*16*u};
        Vector2 b = {mud.x+mud.width-10*u,a.y+3*u};
        DrawLineEx(a,b,2*u,Fade(BLACK,0.18));
    }
    DrawRectangleLinesEx(mud,2*u,Fade(BLACK,0.25));

    //moss, green and shiny
    for (int i=0; i<2; i++)
    {
        DrawRectangleRec(moss[i],Fade(moss_green,0.55));
        for (int k=0; k<12; k++)
        {
            float shine_x = moss[i].x + fmod(k*47*u,moss[i].width);
            float shine_y = moss[i].y + fmod(k*31*u + 9*u,moss[i].height);
            DrawCircle(shine_x,shine_y,2.5*u,Fade(WHITE,0.2+0.2*sin(t*3+k)));
        }
    }

    //quicksand, slowly turning
    for (int i=0; i<2; i++)
    {
        Rectangle q = quicksand[i];
        Vector2 middle = {q.x+q.width/2,q.y+q.height/2};
        DrawRectangleRec(q,GetColor(0xC8A56EFF));
        for (int j=0; j<4; j++)
        {
            float ring = 12*u + j*16*u;
            float turn = t*40;
            if (j%2==1) turn = -t*40;
            DrawRing(middle,ring,ring+3*u,turn+j*50,turn+j*50+220,24,Fade(GetColor(0x8A6A3AFF),0.6));
        }
        DrawRectangleLinesEx(q,3*u,GetColor(0x8A6A3AFF));
    }

    //plank bridges with rope rails: cracked picture while cracking, gone for 5 seconds
    for (int b=0; b<2; b++)
    {
        for (int i=0; i<5; i++)
        {
            float timer = plank_timer[b][i];
            if (timer>=crack_time[b]) continue;
            Rectangle p = plank[b][i];
            int frame = 0;
            float shake = 0;
            if (timer>0)
            {
                frame = 1;
                shake = sin(t*60)*2*u;
            }
            Rectangle source = {frame*256+8,12,240,56};
            Rectangle dest = {p.x+p.width/2+shake,p.y+p.height/2,p.height,p.width-2*u};
            Vector2 origin = {p.height/2,(p.width-2*u)/2};
            DrawTexturePro(plank_texture,source,dest,origin,90,WHITE);
        }
        Vector2 rope1 = {river.x-6*u,plank[b][0].y};
        Vector2 rope2 = {river.x+river.width+6*u,plank[b][0].y};
        DrawLineEx(rope1,rope2,3*u,GetColor(0xC9A66BFF));
        rope1.y = rope1.y + plank[b][0].height;
        rope2.y = rope2.y + plank[b][0].height;
        DrawLineEx(rope1,rope2,3*u,GetColor(0xC9A66BFF));
    }

    //pressure plates, gold when pressed (the gate plate shows how much time is left)
    for (int i=0; i<3; i++)
    {
        int down = plate_down[i];
        if (i==0 && gate_timer>0) down = 1;
        DrawCircle(plate[i].x,plate[i].y,plate_radius+4*u,stone_mortar);
        if (down==1)
        {
            DrawCircleGradient(plate[i],plate_radius*2,Fade(gold,0.35),BLANK);
            DrawCircle(plate[i].x,plate[i].y,plate_radius,gold);
            DrawRing(plate[i],plate_radius*0.45,plate_radius*0.6,0,360,24,gold_light);
        }
        else
        {
            DrawCircle(plate[i].x,plate[i].y,plate_radius,stone);
            DrawRing(plate[i],plate_radius*0.45,plate_radius*0.6,0,360,24,stone_dark);
        }
        if (i==0 && gate_timer>0) DrawRing(plate[i],plate_radius+6*u,plate_radius+10*u,-90,-90+gate_timer/6*360,32,gold_light);
    }
}


//stone doors (drawn before the walls, so the part that slid into the wall is hidden)
void draw_doors()
{
    for (int i=0; i<2; i++)
    {
        DrawRectangleRec(door[i],stone_dark);
        DrawRectangleLinesEx(door[i],3*u,gold);
        DrawCircle(door[i].x+door[i].width/2,door[i].y+door[i].height/2,6*u,gold);
    }
}


//mossy stone walls (the level 1 brick walls)
void draw_walls()
{
    float brick_height = 14*u;
    float brick_length = 28*u;
    for (int i=0; i<13; i++)
    {
        Rectangle rec = walls[i];

        //jungle hedges are leaves, not stone
        if (i>=11)
        {
            DrawRectangleRec(rec,jungle_dark);
            DrawRectangleLinesEx(rec,3*u,Fade(BLACK,0.3));
            continue;
        }
        DrawRectangleRec(rec,stone_mortar);

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
                    Color colour = stone;
                    if ((row*3 + count*7)%5==0) colour = stone_dark;
                    DrawRectangle(left+1*u,y+1*u,right-left-2*u,bottom-y-2*u,colour);
                    DrawRectangle(left+1*u,bottom-3*u,right-left-2*u,2*u,Fade(BLACK,0.25));
                }
                x = x + brick_length;
                count++;
            }
        }

        //dark edge and moss
        DrawRectangleLinesEx(rec,3*u,Fade(BLACK,0.35));
        if (rec.height>rec.width)
        {
            int rivets = rec.height/(40*u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+5*u,rec.y+20*u+j*40*u,2.5*u,moss_green);
                DrawCircle(rec.x+rec.width-5*u,rec.y+20*u+j*40*u,2.5*u,moss_green);
            }
        }
        else
        {
            int rivets = rec.width/(40*u);
            for (int j=0; j<rivets; j++)
            {
                DrawCircle(rec.x+20*u+j*40*u,rec.y+5*u,2.5*u,moss_green);
                DrawCircle(rec.x+20*u+j*40*u,rec.y+rec.height-5*u,2.5*u,moss_green);
            }
        }
    }
}


void draw_darts()
{
    float t = animation_time;

    //dart traps: stone heads at both ends, eyes blink red before firing, darts fly while on
    for (int i=0; i<4; i++)
    {
        Rectangle d = dart_gate[i];
        int across = 0;
        if (d.width>d.height) across = 1;
        Vector2 head1 = {d.x+d.width/2,d.y-10*u};
        Vector2 head2 = {d.x+d.width/2,d.y+d.height+10*u};
        if (across==1)
        {
            head1.x = d.x-10*u;
            head1.y = d.y+d.height/2;
            head2.x = d.x+d.width+10*u;
            head2.y = d.y+d.height/2;
        }
        Color eye = Fade(BLACK,0.7);
        if (dart_clock[i]>=1.2 && dart_clock[i]<1.6 && fmod(t*10,2)<1) eye = RED;
        if (dart_clock[i]>=1.6) eye = RED;
        DrawCircle(head1.x,head1.y,13*u,stone_dark);
        DrawCircle(head2.x,head2.y,13*u,stone_dark);
        DrawCircleLines(head1.x,head1.y,13*u,gold);
        DrawCircleLines(head2.x,head2.y,13*u,gold);
        DrawCircle(head1.x,head1.y,4*u,eye);
        DrawCircle(head2.x,head2.y,4*u,eye);

        if (dart_clock[i]>=1.6)
        {
            DrawRectangleRec(d,Fade(RED,0.15));
            float length = d.height;
            if (across==1) length = d.width;
            for (int k=0; k<4; k++)
            {
                float along = fmod(t*600*u + k*length/4,length);
                Vector2 tip = {d.x+d.width/2,d.y+along};
                Vector2 tail = {tip.x,tip.y-14*u};
                if (across==1)
                {
                    tip.x = d.x+along;
                    tip.y = d.y+d.height/2;
                    tail.x = tip.x-14*u;
                    tail.y = tip.y;
                }
                DrawLineEx(tail,tip,2*u,wood_brown);
                DrawCircle(tail.x,tail.y,3*u,RED);
            }
        }
    }
}


//one jungle plant clump, picture and size picked from k
void draw_clump(float x, float y, int k)
{
    Rectangle source = {(k%3)*320,0,320,320};
    float size = (60 + (k*37)%25)*u;
    Rectangle dest = {x,y,size,size};
    Vector2 origin = {size/2,size/2};
    DrawTexturePro(foliage_texture,source,dest,origin,(k*53)%360,WHITE);
}


void draw_foliage()
{
    int k = 0;

    //along the hedges
    for (int i=11; i<13; i++)
    {
        int clumps = walls[i].width/(70*u);
        for (int j=0; j<=clumps; j++)
        {
            draw_clump(walls[i].x+j*70*u,walls[i].y+walls[i].height/2,k);
            k++;
        }
    }

    //along the river banks and the temple's outer wall, not on the bridges or the gate
    for (int j=0; j<11; j++)
    {
        float y = (130 + j*92)*u;
        int near_bridge = 0;
        for (int b=0; b<2; b++)
        {
            if (y>plank[b][0].y-40*u && y<plank[b][0].y+plank[b][0].height+40*u) near_bridge = 1;
        }
        if (near_bridge==0)
        {
            draw_clump(606*u,y,k);
            draw_clump(774*u,y+40*u,k+1);
        }
        if (y<door_closed_y[0]-50*u || y>door_closed_y[0]+150*u) draw_clump(1150*u,y,k+2);
        k = k + 3;
    }
}


void draw_obstacles()
{
    float t = animation_time;

    //golden altar under the pot
    DrawCircleGradient(pot,110*u,Fade(gold,0.35),BLANK);
    DrawRing(pot,34*u,40*u,0,360,48,gold);
    DrawRing(pot,48*u,50*u,t*30,t*30+300,48,Fade(gold_light,0.7));
    for (int j=0; j<6; j++)
    {
        Vector2 arm = {60*u,0};
        Vector2 sparkle = Vector2Add(pot,Vector2Rotate(arm,(t*50+j*60)*DEG2RAD));
        DrawCircle(sparkle.x,sparkle.y,(2+sin(t*5+j))*u,gold_light);
    }

    //boulders (the big asteroid picture, tinted brown), rolling
    for (int i=0; i<3; i++)
    {
        float size = boulder_radius[i]*2.4;
        Rectangle source = {0,0,192,192};
        Rectangle dest = {boulder[i].x,boulder[i].y,size,size};
        Vector2 origin = {size/2,size/2};
        DrawCircle(boulder[i].x+5*u,boulder[i].y+7*u,boulder_radius[i],Fade(BLACK,0.3));
        DrawTexturePro(boulder_texture,source,dest,origin,boulder_t[i]*360,GetColor(0xC89A6AFF));
    }

    //mushrooms, they get bigger for a moment when hit
    for (int i=0; i<6; i++)
    {
        Vector2 m = bumper[i];
        float r = bumper_radius*(1 + bumper_hit[i]*1.5);
        DrawCircle(m.x+3*u,m.y+5*u,r,Fade(BLACK,0.3));
        DrawCircle(m.x,m.y,r,GetColor(0xC0392BFF));
        DrawCircle(m.x-6*u,m.y-6*u,r*0.3,WHITE);
        DrawCircle(m.x+8*u,m.y+2*u,r*0.2,WHITE);
        DrawCircle(m.x-2*u,m.y+9*u,r*0.15,WHITE);
        DrawCircleLines(m.x,m.y,r,GetColor(0x5E1A12FF));
    }

    //spinning totem (the level 1 fan, with wooden arms)
    Rectangle blade = {fan.x,fan.y,fan_blade_length,fan_blade_thickness};
    Vector2 blade_origin = {fan_blade_length/2,fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,fan_angle-24,Fade(wood_brown,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-24,Fade(wood_brown,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle-12,Fade(wood_brown,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-12,Fade(wood_brown,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle,wood_brown);
    DrawRectanglePro(blade,blade_origin,fan_angle+90,wood_brown);
    DrawCircle(fan.x,fan.y,fan_hub_radius,steel_dark);
    DrawCircleLines(fan.x,fan.y,fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(fan,Vector2Rotate(bolt_arm,(fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*u,gold);
    }

    //splash, 5 pictures in half a second
    if (splash_timer>0)
    {
        int frame = (0.5-splash_timer)/0.1;
        if (frame>4) frame = 4;
        Rectangle source = {frame*192,0,192,192};
        Rectangle dest = {splash_position.x,splash_position.y,96*u,96*u};
        Vector2 origin = {48*u,48*u};
        DrawTexturePro(splash_texture,source,dest,origin,0,river_tint);
    }
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

    //ball (it shrinks while sinking in quicksand)
    float ball_size = radius_ball*(1 - sink_timer/4);
    DrawCircle(ball.x+3*u,ball.y+4*u,ball_size,Fade(BLACK,0.45));
    DrawCircle(ball.x,ball.y,ball_size,GetColor(0xEDEDEDFF));
    DrawCircleLines(ball.x,ball.y,ball_size,GRAY);
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
    DrawText("LEVEL 4 - LOST TEMPLE",32*u,20*u,36*u,BLACK);
    DrawText("LEVEL 4 - LOST TEMPLE",30*u,18*u,36*u,hazard_yellow);
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

    //chomp, darted or sunk message
    if (message_timer>0)
    {
        if (message_type==1)
        {
            DrawText("CHOMP!",width/2-MeasureText("CHOMP!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("CHOMP!",width/2-MeasureText("CHOMP!",90*u)/2,height/2-45*u,90*u,Fade(GREEN,message_timer));
        }
        else if (message_type==2)
        {
            DrawText("DARTED!",width/2-MeasureText("DARTED!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("DARTED!",width/2-MeasureText("DARTED!",90*u)/2,height/2-45*u,90*u,Fade(laser_red,message_timer));
        }
        else
        {
            DrawText("SUNK!",width/2-MeasureText("SUNK!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("SUNK!",width/2-MeasureText("SUNK!",90*u)/2,height/2-45*u,90*u,Fade(gold,message_timer));
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
    InitWindow(1280,720,"the ultimate golf - level 4");
    int monitor = GetCurrentMonitor();
    width = GetMonitorWidth(monitor);
    height = GetMonitorHeight(monitor);
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

    //pictures (made at 2x size; water, foam and splash come from the beach level, boulders from space)
    ground_texture = LoadTexture("assets/jungle/jungle_ground.png");
    temple_texture = LoadTexture("assets/jungle/jungle_temple_floor.png");
    foliage_texture = LoadTexture("assets/jungle/jungle_foliage.png");
    plank_texture = LoadTexture("assets/jungle/jungle_plank.png");
    water_texture = LoadTexture("assets/beach/beach_water_tile.png");
    foam_texture = LoadTexture("assets/beach/beach_foam_strip.png");
    splash_texture = LoadTexture("assets/beach/beach_splash.png");
    boulder_texture = LoadTexture("assets/space/space_asteroid.png");
    SetTextureWrap(ground_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(temple_texture,TEXTURE_WRAP_REPEAT);
    SetTextureWrap(foam_texture,TEXTURE_WRAP_REPEAT);
    SetTextureFilter(ground_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(temple_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(foliage_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(plank_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(water_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(foam_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(splash_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(boulder_texture,TEXTURE_FILTER_BILINEAR);

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
        draw_doors();
        draw_walls();
        draw_darts();
        draw_foliage();
        draw_obstacles();
        draw_ball_and_pot();
        draw_hud();

        EndDrawing();
    }
    UnloadTexture(ground_texture);
    UnloadTexture(temple_texture);
    UnloadTexture(foliage_texture);
    UnloadTexture(plank_texture);
    UnloadTexture(water_texture);
    UnloadTexture(foam_texture);
    UnloadTexture(splash_texture);
    UnloadTexture(boulder_texture);
    CloseWindow();


    return 0;
}
