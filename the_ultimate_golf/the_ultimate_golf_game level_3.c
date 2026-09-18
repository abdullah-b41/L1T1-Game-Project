#include "raylib.h"
#include "raymath.h"
#include <math.h>

//full global
int width = 1920;
int height = 1080;
float u = 1;
#define stroke_limit 20
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
Color space_dark = {6,8,22,255};
Color neon_cyan = {80,230,255,255};
Color neon_purple = {190,90,255,255};
Color neon_orange = {255,150,40,255};
Color neon_green = {110,255,120,255};
Color panel_blue = {50,100,210,255};

//pictures
Texture2D asteroid_texture;
Texture2D ufo_texture;

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

//walkways in path order: standing on one is safe, falling off is lost in space
Rectangle platform[11];

//vacuum strips (almost no friction) and solar wind (the old rip current, pushes towards the void)
Rectangle vacuum[2];
Rectangle solar_wind;
Vector2 solar_wind_push;

//gravity: planets (solid core) and the black hole, it only bends a moving ball
Vector2 planet[2];
float planet_core[2];
float planet_field;
float planet_pull;
Vector2 black_hole;
float black_hole_field;
float black_hole_pull;

//wormholes, one way from in to out, the ball leaves the way the exit faces
Vector2 wormhole_in[3];
Vector2 wormhole_out[3];
float wormhole_angle[3];
float wormhole_radius;
float wormhole_cooldown = 0;

//asteroids drifting back and forth between two points
Vector2 asteroid[4];
Vector2 asteroid_start[4];
Vector2 asteroid_end[4];
Vector2 asteroid_velocity[4];
float asteroid_t[4];
float asteroid_time[4];
float asteroid_radius[4];
int asteroid_direction[4];

//laser gates (from level 1): off 1.2s, warning 0.4s, on 1.4s
Rectangle laser_gate[2];
float laser_clock[2];

//spinning satellite (the level 1 fan)
Vector2 fan;
float fan_angle = 0;
float fan_spin = 90;
float fan_blade_length;
float fan_blade_thickness;
float fan_hub_radius;

//energy bumpers
Vector2 bumper[4];
float bumper_radius;
float bumper_hit[4];

//lost in space effect
Vector2 lost_position;
float lost_timer = 0;

//fly-through events: 0 nothing, 1 comet, 2 meteor shower, 3 ufo (the first 1.5s of each is a warning)
int event_type = 0;
float event_timer = 0;
float next_event = 10;
int shower_done = 0;
int ufo_done = 0;
Vector2 event_start;
Vector2 event_direction;
float event_speed;
Vector2 comet;
Vector2 meteor[5];
Vector2 ufo;
float beam_timer = 0;
int beam_used = 0;
int abducted = 0;
float carry_timer = 0;
int abducted_section = 0;

//stars
Vector2 star[150];
float star_size[150];


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


//stars at random places, 3 sizes (bigger ones drift faster)
void make_stars()
{
    for (int i=0; i<150; i++)
    {
        star[i].x = GetRandomValue(0,width);
        star[i].y = GetRandomValue(0,height);
        star_size[i] = GetRandomValue(1,3)*u;
    }
}


void reset_level()
{
    //sizes
    hud_height = 70*u;
    radius_ball = 7*u;
    radius_pot = 11*u;

    //walkways, spiralling in to the black hole
    platform[0] = make_rect(60,940,1800,100);    //outer ring bottom (start)
    platform[1] = make_rect(1760,110,100,930);   //outer ring right
    platform[2] = make_rect(60,110,1800,100);    //outer ring top
    platform[3] = make_rect(60,110,100,750);     //outer ring left
    platform[4] = make_rect(60,760,1620,100);    //inner ring bottom
    platform[5] = make_rect(1580,290,100,570);   //inner ring right
    platform[6] = make_rect(240,290,1440,100);   //inner ring top
    platform[7] = make_rect(240,290,100,390);    //inner ring left
    platform[8] = make_rect(240,470,560,210);    //centre left
    platform[9] = make_rect(800,470,320,60);     //narrow bridge past the black hole
    platform[10] = make_rect(1120,470,380,210);  //pot platform

    vacuum[0] = make_rect(500,940,350,100);
    vacuum[1] = make_rect(500,290,350,100);
    solar_wind = make_rect(60,400,100,180);
    solar_wind_push = make_point(-280,0);

    //gravity
    planet[0] = make_point(600,720);
    planet[1] = make_point(1450,250);
    planet_core[0] = 28*u;
    planet_core[1] = 28*u;
    planet_field = 200*u;
    planet_pull = 700*u;
    black_hole = make_point(960,640);
    black_hole_field = 240*u;
    black_hole_pull = 500*u;

    //wormholes: purple shortcut, cyan to the pot, orange trap
    wormhole_radius = 26*u;
    wormhole_in[0] = make_point(1840,700);
    wormhole_out[0] = make_point(1300,340);
    wormhole_angle[0] = 180;
    wormhole_in[1] = make_point(700,600);
    wormhole_out[1] = make_point(1170,600);
    wormhole_angle[1] = 0;
    wormhole_in[2] = make_point(1300,810);
    wormhole_out[2] = make_point(960,250);
    wormhole_angle[2] = 90;
    wormhole_cooldown = 0;

    //asteroids
    asteroid_start[0] = make_point(700,50);
    asteroid_end[0] = make_point(700,250);
    asteroid_radius[0] = 30*u;
    asteroid_time[0] = 2.4;
    asteroid_start[1] = make_point(1250,250);
    asteroid_end[1] = make_point(1250,50);
    asteroid_radius[1] = 26*u;
    asteroid_time[1] = 2.0;
    asteroid_start[2] = make_point(1000,250);
    asteroid_end[2] = make_point(1000,430);
    asteroid_radius[2] = 22*u;
    asteroid_time[2] = 1.8;
    asteroid_start[3] = make_point(420,430);
    asteroid_end[3] = make_point(420,720);
    asteroid_radius[3] = 30*u;
    asteroid_time[3] = 3.0;
    for (int i=0; i<4; i++)
    {
        asteroid_t[i] = 0;
        asteroid_direction[i] = 1;
        asteroid[i] = asteroid_start[i];
        asteroid_velocity[i] = no_speed;
        bumper_hit[i] = 0;
    }

    //laser gates across the two right hand walkways
    laser_gate[0] = make_rect(1760,450,100,8);
    laser_gate[1] = make_rect(1580,600,100,8);
    laser_clock[0] = 0;
    laser_clock[1] = 1.5;

    //satellite
    fan = make_point(1200,990);
    fan_blade_length = 150*u;
    fan_blade_thickness = 12*u;
    fan_hub_radius = 16*u;
    fan_angle = 0;

    //energy bumpers
    bumper_radius = 20*u;
    bumper[0] = make_point(110,700);
    bumper[1] = make_point(1630,420);
    bumper[2] = make_point(1330,520);
    bumper[3] = make_point(1330,640);

    //fly-through events
    event_type = 0;
    next_event = 10;
    shower_done = 0;
    ufo_done = 0;
    abducted = 0;
    beam_timer = 0;
    beam_used = 0;

    //ball and pot
    start_position = make_point(140,990);
    ball = start_position;
    last_shot_position = start_position;
    speed.x = 0;
    speed.y= 0;
    pot = make_point(1400,575);
    stroke = 0;
    game_state = 0;
    aiming = 0;
    message_timer = 0;
    lost_timer = 0;
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


//pull towards a centre, stronger the closer the ball is
Vector2 gravity_pull(Vector2 center, float field, float strength, float dt)
{
    Vector2 pull = {0,0};
    Vector2 to_center = Vector2Subtract(center,ball);
    float distance = Vector2Length(to_center);
    if (distance<field && distance>1*u)
    {
        float closeness = 1 - distance/field;
        pull = Vector2Scale(Vector2Normalize(to_center),strength*closeness*closeness*dt);
    }
    return pull;
}


//is this point on a walkway
int on_safe_ground(Vector2 point)
{
    for (int i=0; i<11; i++)
    {
        if (CheckCollisionPointRec(point,platform[i])) return 1;
    }
    return 0;
}


//a random point on a walkway, 20 units inside its edges
Vector2 random_point_on(Rectangle rec)
{
    Vector2 point;
    point.x = rec.x + 20*u + GetRandomValue(0,1000)/1000.0*(rec.width-40*u);
    point.y = rec.y + 20*u + GetRandomValue(0,1000)/1000.0*(rec.height-40*u);
    return point;
}


//which walkway a point is on (the later one if two overlap)
int section_of(Vector2 point)
{
    int section = 0;
    for (int i=0; i<11; i++)
    {
        if (CheckCollisionPointRec(point,platform[i])) section = i;
    }
    return section;
}


//a spot where a dropped ball won't land in trouble
int spot_is_clear(Vector2 spot)
{
    if (on_safe_ground(spot)==0) return 0;
    for (int i=0; i<2; i++)
    {
        if (Vector2Distance(spot,planet[i]) < planet_core[i]+30*u) return 0;
        if (CheckCollisionCircleRec(spot,30*u,laser_gate[i])) return 0;
    }
    for (int i=0; i<4; i++)
    {
        if (Vector2Distance(spot,bumper[i]) < bumper_radius+30*u) return 0;
        if (Vector2Distance(spot,asteroid[i]) < asteroid_radius[i]+30*u) return 0;
    }
    for (int i=0; i<3; i++)
    {
        if (Vector2Distance(spot,wormhole_in[i]) < wormhole_radius+30*u) return 0;
    }
    if (Vector2Distance(spot,fan) < fan_blade_length/2+30*u) return 0;
    if (CheckCollisionPointRec(spot,solar_wind)) return 0;
    if (Vector2Distance(spot,pot) < 40*u) return 0;
    return 1;
}


void update_obstacles(float dt)
{
    //asteroids drifting back and forth
    for (int i=0; i<4; i++)
    {
        asteroid_t[i] = asteroid_t[i] + asteroid_direction[i]*dt/asteroid_time[i];
        if (asteroid_t[i]>=1)
        {
            asteroid_t[i] = 1;
            asteroid_direction[i] = -1;
        }
        if (asteroid_t[i]<=0)
        {
            asteroid_t[i] = 0;
            asteroid_direction[i] = 1;
        }
        asteroid[i] = Vector2Lerp(asteroid_start[i],asteroid_end[i],asteroid_t[i]);
        asteroid_velocity[i] = Vector2Scale(Vector2Subtract(asteroid_end[i],asteroid_start[i]),asteroid_direction[i]/asteroid_time[i]);
        if (bumper_hit[i]>0) bumper_hit[i] = bumper_hit[i] - dt;
    }

    //fan spinning
    fan_angle = fan_angle + fan_spin*dt;
    if (fan_angle>=360) fan_angle = fan_angle - 360;

    //laser clocks
    for (int i=0; i<2; i++)
    {
        laser_clock[i] = laser_clock[i] + dt;
        if (laser_clock[i]>=3.0) laser_clock[i] = laser_clock[i] - 3.0;
    }

    if (wormhole_cooldown>0) wormhole_cooldown = wormhole_cooldown - dt;
    if (lost_timer>0) lost_timer = lost_timer - dt;
}


//after falling off or getting zapped, the ball goes back to where it was shot from
void send_ball_back()
{
    ball = last_shot_position;

    //not right in an asteroid's path, or it gets knocked off again and again
    for (int i=0; i<4; i++)
    {
        Vector2 path = Vector2Subtract(asteroid_end[i],asteroid_start[i]);
        float along = Vector2DotProduct(Vector2Subtract(ball,asteroid_start[i]),path)/Vector2DotProduct(path,path);
        along = Clamp(along,0,1);
        Vector2 closest = Vector2Add(asteroid_start[i],Vector2Scale(path,along));
        Vector2 away = Vector2Subtract(ball,closest);
        float gap = asteroid_radius[i] + radius_ball + 2*u;
        if (Vector2Length(away)<gap)
        {
            Vector2 side = {-path.y,path.x};
            side = Vector2Normalize(side);
            if (Vector2DotProduct(away,side)<0) side = Vector2Negate(side);
            ball = Vector2Add(closest,Vector2Scale(side,gap));
        }
    }

    //not where the satellite blades sweep, or it gets knocked off again and again (the level 1 piston rule)
    float sweep = fan_blade_length/2 + radius_ball + 2*u;
    if (Vector2Distance(ball,fan)<sweep)
    {
        if (ball.x<fan.x) ball.x = fan.x - sweep;
        else ball.x = fan.x + sweep;
    }

    //not inside a laser gate (level 1 rule), not off a walkway
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionCircleRec(ball,radius_ball,laser_gate[i])) ball = start_position;
    }
    if (on_safe_ground(ball)==0) ball = start_position;

    speed.x = 0;
    speed.y = 0;
}


//a comet or meteor hits the ball and knocks it along
void knock_ball(Vector2 center, float radius)
{
    if (game_state!=0 || abducted==1) return;
    Vector2 normal = Vector2Subtract(ball,center);
    float distance = Vector2Length(normal);
    if (distance < radius + radius_ball)
    {
        if (distance==0) normal = event_direction;
        normal = Vector2Normalize(normal);
        ball = Vector2Add(center,Vector2Scale(normal,radius + radius_ball));
        speed = Vector2ClampValue(Vector2Add(Vector2Scale(event_direction,400*u),Vector2Scale(normal,150*u)),0,max_speed*u);
    }
}


//the ufo lets go: a random clear spot on the walkway before or after the one the ball was on
void drop_ball()
{
    int section = abducted_section - 1;
    if (GetRandomValue(0,1)==1) section = abducted_section + 1;
    if (section<0) section = 1;
    if (section>10) section = 9;
    if (section==9) section = 8 + 2*GetRandomValue(0,1);    //never the narrow bridge
    ball = last_shot_position;
    for (int tries=0; tries<30; tries++)
    {
        Vector2 spot = random_point_on(platform[section]);
        if (spot_is_clear(spot))
        {
            ball = spot;
            break;
        }
    }
    speed.x = 0;
    speed.y = 0;
    abducted = 0;
    beam_timer = 0;
}


//pick a new fly-through, the meteor shower and the ufo only come once
void start_event()
{
    event_type = GetRandomValue(1,3);
    if (event_type==2 && shower_done==1) event_type = 1;
    if (event_type==3 && ufo_done==1) event_type = 1;
    event_timer = 0;

    if (event_type==3)
    {
        //the ufo flies straight across, over one of the long walkways
        ufo_done = 1;
        beam_timer = 0;
        beam_used = 0;
        int lane = GetRandomValue(0,3)*2;
        event_start.y = platform[lane].y + platform[lane].height/2;
        event_start.x = -120*u;
        event_direction.x = 1;
        event_direction.y = 0;
        if (GetRandomValue(0,1)==1)
        {
            event_start.x = width + 120*u;
            event_direction.x = -1;
        }
        event_speed = 170*u;
        ufo = event_start;
        return;
    }

    //comet or meteor shower: aimed at a random walkway spot from a random side, starting just off the screen
    if (event_type==2) shower_done = 1;
    Vector2 target = random_point_on(platform[GetRandomValue(0,10)]);
    float angle = GetRandomValue(0,359)*DEG2RAD;
    event_direction.x = cos(angle);
    event_direction.y = sin(angle);
    float back_x = 10000;
    float back_y = 10000;
    if (event_direction.x>0.01) back_x = target.x/event_direction.x;
    if (event_direction.x<-0.01) back_x = (width-target.x)/(-event_direction.x);
    if (event_direction.y>0.01) back_y = target.y/event_direction.y;
    if (event_direction.y<-0.01) back_y = (height-target.y)/(-event_direction.y);
    float back = back_x;
    if (back_y<back) back = back_y;
    event_start = Vector2Subtract(target,Vector2Scale(event_direction,back+60*u));
    event_speed = 900*u;
    if (event_type==2) event_speed = 650*u;
    comet = event_start;
}


void update_events(float dt)
{
    //waiting for the next one
    if (event_type==0)
    {
        next_event = next_event - dt;
        if (next_event<=0) start_event();
        return;
    }

    event_timer = event_timer + dt;
    float flying = event_timer - 1.5;
    if (flying<0) return;

    //comet
    if (event_type==1)
    {
        comet = Vector2Add(event_start,Vector2Scale(event_direction,event_speed*flying));
        knock_ball(comet,12*u);
    }

    //meteor shower, 5 rocks side by side, a quarter of a second apart
    if (event_type==2)
    {
        for (int i=0; i<5; i++)
        {
            Vector2 side = {-event_direction.y*(i-2)*45*u, event_direction.x*(i-2)*45*u};
            float travelled = event_speed*(flying - i*0.25);
            meteor[i] = Vector2Add(Vector2Add(event_start,side),Vector2Scale(event_direction,travelled));
            if (travelled>0) knock_ball(meteor[i],10*u);
        }
    }

    //ufo, beams up a ball that sits still under it
    if (event_type==3)
    {
        ufo = Vector2Add(event_start,Vector2Scale(event_direction,event_speed*flying));
        if (abducted==0 && beam_used==0 && game_state==0)
        {
            if (speed.x==0 && speed.y==0 && Vector2Distance(ball,ufo)<80*u)
            {
                beam_timer = beam_timer + dt;
                if (beam_timer>=0.8)
                {
                    abducted = 1;
                    beam_used = 1;
                    carry_timer = 1;
                    aiming = 0;
                    abducted_section = section_of(ball);
                    message_type = 3;
                    message_timer = 1;
                }
            }
            else beam_timer = 0;
        }
        if (abducted==1)
        {
            ball = ufo;
            speed.x = 0;
            speed.y = 0;
            carry_timer = carry_timer - dt;
            if (carry_timer<=0) drop_ball();
        }
    }

    //far enough to be off the screen: over, wait for the next one
    float travelled = event_speed*flying;
    float needed = width + height + 400*u;
    if (event_type==2) travelled = event_speed*(flying - 1);
    if (event_type==3) needed = width + 240*u;
    if (travelled>needed)
    {
        if (abducted==1) drop_ball();
        event_type = 0;
        next_event = GetRandomValue(12,22);
    }
}


void update_ball(float dt)
{
    int pushed = 0;

    //solar wind (the old rip current)
    if (CheckCollisionPointRec(ball,solar_wind))
    {
        speed = Vector2Add(speed,Vector2Scale(solar_wind_push,dt));
        pushed = 1;
    }

    //gravity from the planets and the black hole, only bends a moving ball
    if (speed.x!=0 || speed.y!=0)
    {
        for (int i=0; i<2; i++)
        {
            speed = Vector2Add(speed,gravity_pull(planet[i],planet_field,planet_pull,dt));
        }
        speed = Vector2Add(speed,gravity_pull(black_hole,black_hole_field,black_hole_pull,dt));
    }

    //moving
    ball = Vector2Add(ball,Vector2Scale(speed,dt));

    //proportional deceleration (almost no friction in a vacuum strip)
    float friction = 110*u;
    for (int i=0; i<2; i++)
    {
        if (CheckCollisionPointRec(ball,vacuum[i])) friction = 25*u;
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

    //wormholes, one way, the ball comes out the way the exit faces
    if (wormhole_cooldown<=0)
    {
        for (int i=0; i<3; i++)
        {
            if (Vector2Distance(ball,wormhole_in[i])<wormhole_radius)
            {
                Vector2 facing = {cos(wormhole_angle[i]*DEG2RAD),sin(wormhole_angle[i]*DEG2RAD)};
                float ball_speed = Vector2Length(speed);
                if (ball_speed<80*u) ball_speed = 80*u;
                ball = Vector2Add(wormhole_out[i],Vector2Scale(facing,wormhole_radius + radius_ball + 2*u));
                speed = Vector2Scale(facing,ball_speed);
                wormhole_cooldown = 0.5;
                break;
            }
        }
    }

    //lost in space (checked before the collisions, so if something is on the old spot it pushes the ball out)
    if (on_safe_ground(ball)==0)
    {
        lost_position = ball;
        lost_timer = 0.5;
        send_ball_back();
        message_type = 1;
        message_timer = 1;
    }

    //laser gates (from level 1)
    for (int i=0; i<2; i++)
    {
        if (laser_clock[i]>=1.6 && CheckCollisionCircleRec(ball,radius_ball,laser_gate[i]))
        {
            send_ball_back();
            message_type = 2;
            message_timer = 1;
        }
    }

    //planet cores, energy bumpers and asteroids
    for (int i=0; i<2; i++)
    {
        bounce_off_circle(planet[i],planet_core[i],1,no_speed);
    }
    for (int i=0; i<4; i++)
    {
        if (bounce_off_circle(bumper[i],bumper_radius,1.3,no_speed)) bumper_hit[i] = 0.15;
        bounce_off_circle(asteroid[i],asteroid_radius[i],1,asteroid_velocity[i]);
    }

    //spinning satellite (the level 1 fan)
    bounce_off_circle(fan,fan_hub_radius,1,no_speed);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle,fan_spin);
    bounce_off_rotated_rectangle(fan,fan_blade_length,fan_blade_thickness,fan_angle+90,fan_spin);

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

void draw_space()
{
    float t = animation_time;
    DrawRectangleGradientV(0,0,width,height,space_dark,GetColor(0x140A2AFF));

    //nebula clouds
    DrawCircleGradient(make_point(420,300),420*u,Fade(neon_purple,0.12),BLANK);
    DrawCircleGradient(make_point(1500,820),480*u,Fade(panel_blue,0.14),BLANK);
    DrawCircleGradient(make_point(1650,200),300*u,Fade(neon_cyan,0.07),BLANK);
    DrawCircleGradient(make_point(900,950),350*u,Fade(PINK,0.06),BLANK);

    //stars drifting slowly to the left (bigger ones faster), all twinkling
    for (int i=0; i<150; i++)
    {
        float x = fmod(star[i].x - t*star_size[i]*6 + width*100,width);
        float twinkle = 0.5 + 0.5*sin(t*2 + i);
        DrawCircle(x,star[i].y,star_size[i]*0.7,Fade(WHITE,0.3+0.6*twinkle));
    }

    //black hole: glow, spinning bright rings, dark middle
    DrawCircleGradient(black_hole,black_hole_field,Fade(neon_purple,0.22),BLANK);
    for (int j=0; j<5; j++)
    {
        float ring = 34*u + j*13*u;
        float angle = t*(220 - j*35) + j*60;
        Color ring_colour = neon_orange;
        if (j%2==1) ring_colour = neon_purple;
        DrawRing(black_hole,ring-2*u,ring+2*u,angle,angle+250,32,Fade(ring_colour,0.8-j*0.12));
    }
    DrawCircleGradient(black_hole,40*u,BLACK,Fade(BLACK,0));
    DrawCircle(black_hole.x,black_hole.y,26*u,BLACK);
    DrawRing(black_hole,26*u,28*u,0,360,48,Fade(WHITE,0.5));
}


void draw_walkways()
{
    float t = animation_time;

    //glow, shadow and edge lights first, then the walkways on top, so they only show on the outside
    for (int i=0; i<11; i++)
    {
        Rectangle p = platform[i];
        Rectangle glow = {p.x-6*u,p.y-6*u,p.width+12*u,p.height+12*u};
        DrawRectangleRec(glow,Fade(neon_cyan,0.18));
        Rectangle shadow = {p.x+8*u,p.y+10*u,p.width,p.height};
        DrawRectangleRec(shadow,Fade(BLACK,0.5));
        int lights_x = p.width/(60*u);
        int lights_y = p.height/(60*u);
        for (int j=0; j<=lights_x; j++)
        {
            float blink = 0.3 + 0.7*(sin(t*3 - j*0.7 - i)>0.6);
            DrawCircle(p.x+j*60*u,p.y-4*u,2.5*u,Fade(neon_cyan,blink));
            DrawCircle(p.x+j*60*u,p.y+p.height+4*u,2.5*u,Fade(neon_cyan,blink));
        }
        for (int j=0; j<=lights_y; j++)
        {
            float blink = 0.3 + 0.7*(sin(t*3 - j*0.7 - i)>0.6);
            DrawCircle(p.x-4*u,p.y+j*60*u,2.5*u,Fade(neon_cyan,blink));
            DrawCircle(p.x+p.width+4*u,p.y+j*60*u,2.5*u,Fade(neon_cyan,blink));
        }
    }

    //steel walkways with panel lines lined up with the screen, so joins match
    for (int i=0; i<11; i++)
    {
        Rectangle p = platform[i];
        DrawRectangleRec(p,steel_dark);
        int first_column = p.x/(50*u) + 1;
        int first_row = p.y/(50*u) + 1;
        for (int j=first_column; j*50*u<p.x+p.width; j++)
        {
            DrawLine(j*50*u,p.y,j*50*u,p.y+p.height,Fade(steel,0.5));
        }
        for (int j=first_row; j*50*u<p.y+p.height; j++)
        {
            DrawLine(p.x,j*50*u,p.x+p.width,j*50*u,Fade(steel,0.5));
        }
    }

    //vacuum strips: darker with little sparkles
    for (int i=0; i<2; i++)
    {
        Rectangle v = vacuum[i];
        DrawRectangleRec(v,Fade(BLACK,0.45));
        for (int k=0; k<16; k++)
        {
            float x = v.x + fmod(k*67*u + t*20*u,v.width);
            float y = v.y + fmod(k*29*u + 11*u,v.height);
            DrawCircle(x,y,1.5*u,Fade(neon_cyan,0.3+0.3*sin(t*4+k)));
        }
        DrawRectangleLinesEx(v,2*u,Fade(neon_cyan,0.35));
    }

    //solar wind: yellow streaks blowing the way it pushes
    DrawRectangleRec(solar_wind,Fade(neon_orange,0.12));
    int streaks = solar_wind.height/(18*u);
    for (int j=0; j<streaks; j++)
    {
        float x = solar_wind.x + solar_wind.width - fmod(t*140*u + j*31*u,solar_wind.width);
        Vector2 a = {x, solar_wind.y + 9*u + j*18*u};
        Vector2 b = {x + 16*u, a.y};
        if (b.x > solar_wind.x+solar_wind.width) b.x = solar_wind.x + solar_wind.width;
        DrawLineEx(a,b,2*u,Fade(hazard_yellow,0.6));
    }
}


void draw_obstacles()
{
    float t = animation_time;

    //planets: glow, shaded ball, thin ring
    Color planet_light[2] = {{255,190,120,255},{120,240,220,255}};
    Color planet_dark[2] = {{150,60,30,255},{20,90,110,255}};
    for (int i=0; i<2; i++)
    {
        DrawCircleGradient(planet[i],planet_field*0.5,Fade(planet_light[i],0.12),BLANK);
        DrawCircleGradient(planet[i],planet_core[i],planet_light[i],planet_dark[i]);
        DrawCircle(planet[i].x-8*u,planet[i].y-9*u,6*u,Fade(WHITE,0.35));
        DrawRing(planet[i],planet_core[i]+8*u,planet_core[i]+11*u,0,360,48,Fade(planet_light[i],0.5));
    }

    //wormholes: spinning rings at the way in, a dimmer ring and an arrow at the way out
    Color wormhole_colour[3] = {neon_purple,neon_cyan,neon_orange};
    for (int i=0; i<3; i++)
    {
        Color c = wormhole_colour[i];
        DrawCircleGradient(wormhole_in[i],wormhole_radius*1.6,Fade(c,0.35),BLANK);
        for (int j=0; j<3; j++)
        {
            float ring = wormhole_radius*(0.4 + j*0.25);
            float angle = t*(260 + j*80) + j*120;
            DrawRing(wormhole_in[i],ring,ring+3*u,angle,angle+200,24,Fade(c,0.9-j*0.2));
        }
        DrawCircle(wormhole_in[i].x,wormhole_in[i].y,wormhole_radius*0.3,BLACK);

        DrawRing(wormhole_out[i],wormhole_radius*0.7,wormhole_radius*0.8,-t*120,-t*120+270,24,Fade(c,0.6));
        Vector2 facing = {cos(wormhole_angle[i]*DEG2RAD),sin(wormhole_angle[i]*DEG2RAD)};
        Vector2 arrow = Vector2Add(wormhole_out[i],Vector2Scale(facing,wormhole_radius+6*u));
        DrawPoly(arrow,3,9*u,wormhole_angle[i],Fade(c,0.85));
    }

    //energy bumpers, they get bigger for a moment when hit
    for (int i=0; i<4; i++)
    {
        float r = bumper_radius*(1 + bumper_hit[i]*1.5);
        float pulse = 0.5 + 0.5*sin(t*4 + i);
        DrawCircleGradient(bumper[i],r*1.8,Fade(neon_green,0.1+0.25*pulse),BLANK);
        DrawCircle(bumper[i].x,bumper[i].y,r,GetColor(0x0E2A1AFF));
        DrawRing(bumper[i],r-4*u,r,0,360,32,neon_green);
        DrawCircle(bumper[i].x,bumper[i].y,r*0.35,Fade(WHITE,0.6+0.3*pulse));
    }

    //asteroids, turning slowly
    for (int i=0; i<4; i++)
    {
        float size = asteroid_radius[i]*2.4;
        Rectangle source = {(i%3)*192,0,192,192};
        Rectangle dest = {asteroid[i].x,asteroid[i].y,size,size};
        Vector2 origin = {size/2,size/2};
        float spin = t*35;
        if (i%2==1) spin = -t*28;
        DrawTexturePro(asteroid_texture,source,dest,origin,spin,WHITE);
    }

    //lost in space: rings shrinking into the spot
    if (lost_timer>0)
    {
        for (int j=0; j<3; j++)
        {
            float ring = lost_timer*80*u + j*10*u;
            DrawRing(lost_position,ring,ring+2*u,0,360,32,Fade(neon_purple,lost_timer*2));
        }
    }

    //spinning satellite (the level 1 fan, with solar panels)
    Rectangle blade = {fan.x,fan.y,fan_blade_length,fan_blade_thickness};
    Vector2 blade_origin = {fan_blade_length/2,fan_blade_thickness/2};
    DrawRectanglePro(blade,blade_origin,fan_angle-24,Fade(panel_blue,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-24,Fade(panel_blue,0.12));
    DrawRectanglePro(blade,blade_origin,fan_angle-12,Fade(panel_blue,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle+90-12,Fade(panel_blue,0.25));
    DrawRectanglePro(blade,blade_origin,fan_angle,panel_blue);
    DrawRectanglePro(blade,blade_origin,fan_angle+90,panel_blue);
    DrawCircle(fan.x,fan.y,fan_hub_radius,steel_dark);
    DrawCircleLines(fan.x,fan.y,fan_hub_radius,BLACK);
    Vector2 bolt_arm = {9*u,0};
    for (int j=0; j<4; j++)
    {
        Vector2 bolt = Vector2Add(fan,Vector2Rotate(bolt_arm,(fan_angle+45+j*90)*DEG2RAD));
        DrawCircle(bolt.x,bolt.y,2.5*u,steel_light);
    }

    //laser gates (from level 1)
    for (int i=0; i<2; i++)
    {
        Rectangle laser = laser_gate[i];
        float laser_timer = laser_clock[i];
        float wall = 24*u;
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

//short preview of the shot (0.3 seconds), gravity bend included
void draw_aim_preview()
{
    if (aiming==0 || IsMouseButtonDown(MOUSE_BUTTON_LEFT)==0) return;
    Vector2 mouse = {GetMouseX(),GetMouseY()};
    Vector2 drag = Vector2Subtract(ball,mouse);
    if (Vector2Length(drag)<2*radius_ball) return;

    //remember everything update_ball can change
    Vector2 saved_ball = ball;
    Vector2 saved_speed = speed;
    Vector2 saved_last_shot = last_shot_position;
    Vector2 saved_lost_position = lost_position;
    float saved_lost_timer = lost_timer;
    float saved_message_timer = message_timer;
    int saved_message_type = message_type;
    int saved_game_state = game_state;
    float saved_cooldown = wormhole_cooldown;
    float saved_hit[4];
    for (int i=0; i<4; i++) saved_hit[i] = bumper_hit[i];

    //try the shot for 72 tiny steps, a dot every 6, stop at a hazard or a wormhole jump
    speed = Vector2ClampValue(Vector2Scale(drag,2.5),0,max_speed*u);
    message_timer = -1;
    for (int i=1; i<=72; i++)
    {
        Vector2 before = ball;
        update_ball(1.0/240);
        if (message_timer==1 || game_state!=0 || Vector2Distance(before,ball)>20*u) break;
        if (i%6==0) DrawCircle(ball.x,ball.y,3*u,Fade(WHITE,1-i/90.0));
        if (speed.x==0 && speed.y==0) break;
    }

    //put everything back
    ball = saved_ball;
    speed = saved_speed;
    last_shot_position = saved_last_shot;
    lost_position = saved_lost_position;
    lost_timer = saved_lost_timer;
    message_timer = saved_message_timer;
    message_type = saved_message_type;
    game_state = saved_game_state;
    wormhole_cooldown = saved_cooldown;
    for (int i=0; i<4; i++) bumper_hit[i] = saved_hit[i];
}


void draw_events()
{
    float t = animation_time;
    if (event_type==0) return;

    //warning: blinking red arrow at the edge where it comes in
    if (event_timer<1.5)
    {
        Vector2 sign = event_start;
        sign.x = Clamp(sign.x,40*u,width-40*u);
        sign.y = Clamp(sign.y,hud_height+40*u,height-40*u);
        float angle = atan2(event_direction.y,event_direction.x)*RAD2DEG;
        if (fmod(t*6,2)<1.3)
        {
            DrawCircle(sign.x,sign.y,26*u,Fade(RED,0.35));
            DrawPoly(sign,3,18*u,angle,RED);
        }
        return;
    }

    //comet: bright head and a fading tail
    if (event_type==1)
    {
        for (int j=12; j>0; j--)
        {
            Vector2 bit = Vector2Subtract(comet,Vector2Scale(event_direction,j*14*u));
            DrawCircle(bit.x,bit.y,(12-j*0.8)*u,Fade(neon_cyan,0.5-j*0.035));
        }
        DrawCircleGradient(comet,26*u,Fade(WHITE,0.6),BLANK);
        DrawCircle(comet.x,comet.y,12*u,WHITE);
    }

    //meteor shower: small rocks with orange tails
    if (event_type==2)
    {
        for (int i=0; i<5; i++)
        {
            Vector2 tail = Vector2Subtract(meteor[i],Vector2Scale(event_direction,40*u));
            DrawLineEx(tail,meteor[i],6*u,Fade(neon_orange,0.5));
            float size = 29*u;
            Rectangle source = {(i%3)*192,0,192,192};
            Rectangle dest = {meteor[i].x,meteor[i].y,size,size};
            Vector2 origin = {size/2,size/2};
            DrawTexturePro(asteroid_texture,source,dest,origin,t*200,WHITE);
        }
    }

    //ufo: green beam while it takes the ball, chasing lights otherwise
    if (event_type==3)
    {
        int frame = (int)(t*6)%3;
        if (beam_timer>0 || abducted==1)
        {
            frame = 3;
            DrawCircleGradient(ufo,80*u,Fade(neon_green,0.45),Fade(neon_green,0.05));
            DrawRing(ufo,76*u,80*u,0,360,48,Fade(neon_green,0.6));
        }
        DrawCircle(ufo.x+20*u,ufo.y+26*u,70*u,Fade(BLACK,0.3));
        Rectangle source = {frame*320,0,320,320};
        Rectangle dest = {ufo.x,ufo.y,150*u,150*u};
        Vector2 origin = {75*u,75*u};
        DrawTexturePro(ufo_texture,source,dest,origin,0,WHITE);
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
    DrawText("LEVEL 3 - EVENT HORIZON",32*u,20*u,36*u,BLACK);
    DrawText("LEVEL 3 - EVENT HORIZON",30*u,18*u,36*u,hazard_yellow);
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

    //lost, zapped or beamed up message
    if (message_timer>0)
    {
        if (message_type==1)
        {
            DrawText("LOST IN SPACE!",width/2-MeasureText("LOST IN SPACE!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("LOST IN SPACE!",width/2-MeasureText("LOST IN SPACE!",90*u)/2,height/2-45*u,90*u,Fade(neon_purple,message_timer));
        }
        else if (message_type==2)
        {
            DrawText("ZAPPED!",width/2-MeasureText("ZAPPED!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("ZAPPED!",width/2-MeasureText("ZAPPED!",90*u)/2,height/2-45*u,90*u,Fade(laser_red,message_timer));
        }
        else
        {
            DrawText("BEAMED UP!",width/2-MeasureText("BEAMED UP!",90*u)/2+4*u,height/2-41*u,90*u,Fade(BLACK,message_timer));
            DrawText("BEAMED UP!",width/2-MeasureText("BEAMED UP!",90*u)/2,height/2-45*u,90*u,Fade(neon_green,message_timer));
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
    InitWindow(1280,720,"the ultimate golf - level 3");
    int monitor = GetCurrentMonitor();
    width = GetMonitorWidth(monitor);
    height = GetMonitorHeight(monitor);
    ToggleBorderlessWindowed();
    SetTargetFPS(60);

    //pictures (made at 2x size, so they stay sharp)
    asteroid_texture = LoadTexture("assets/space/space_asteroid.png");
    ufo_texture = LoadTexture("assets/space/space_ufo.png");
    SetTextureFilter(asteroid_texture,TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(ufo_texture,TEXTURE_FILTER_BILINEAR);

    //every size is N*u, so it looks the same on any screen
    u = height/1080.0;
    if (width/1920.0 < u) u = width/1920.0;
    make_stars();
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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && ball_stopped==1 && game_state==0 && abducted==0) aiming = 1;
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
            update_events(dt/4);
            if (game_state==0 && abducted==0) update_ball(dt/4);
        }

        //out of strokes
        if (game_state==0 && stroke>=stroke_limit && speed.x==0 && speed.y==0) game_state = 2;


        BeginDrawing();

        draw_space();
        draw_walkways();
        draw_obstacles();
        draw_ball_and_pot();
        draw_aim_preview();
        draw_events();
        draw_hud();

        EndDrawing();
    }
    UnloadTexture(asteroid_texture);
    UnloadTexture(ufo_texture);
    CloseWindow();


    return 0;
}
