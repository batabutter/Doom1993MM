/**
 * hello1_sdl.c - Initializes SDL
 */

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FOV 200

typedef struct
{
    struct { float x, y, z; } position, velocity;
    int angle;
    int l;
}player; player P;

typedef struct
{
    int w, s, a, d;          
    int sl, sr;             
    int m;                
}keys; keys K;

typedef struct
{
    float cos[360];
    float sin[360];
}math; math M;

void player_angle_move_offset();

void init();

int move_player();

void draw_3D(SDL_Renderer *renderer);

void draw_wall(SDL_Renderer* renderer,
    int x1, int x2, int b1, int b2, int t1, int t2);

int main(void)
{
    init();
    SDL_Quit();
}

void init()
{
    SDL_Window* window = SDL_CreateWindow("3D Renderer Demo", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    int simulation_running = 1;
    SDL_Event event;

    SDL_Rect background = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

    SDL_Init(SDL_INIT_VIDEO);

    int x = 0;   

    for (x = 0; x < 360; x++)
    {
        M.cos[x] = cos(x / 180.0 * M_PI);
        M.sin[x] = sin(x / 180.0 * M_PI);
    }

    P.position.x = 70; P.position.y = -110; P.position.z = 20;
    P.angle = 0; P.l = 0;

    while (simulation_running)
    {
        simulation_running = move_player();

        draw_3D(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }
}

int move_player()
{

    SDL_Event event;
    int quit = 1;

    while (SDL_PollEvent(&event))
    {
        const Uint8* state = 0;
        switch (event.type)
        {
            case SDL_QUIT:
                quit = 0;
                break;
            case(SDL_KEYDOWN):
                state = SDL_GetKeyboardState(NULL);
                switch (event.key.keysym.sym)
                {
                    case SDLK_SPACE:
                        quit = 0;
                        break;
                    default:
                        K.w = state[SDL_SCANCODE_W];
				        K.a = state[SDL_SCANCODE_A];
                        K.s = state[SDL_SCANCODE_S];
		                K.d = state[SDL_SCANCODE_D];
                        K.m = state[SDL_SCANCODE_M];
                        K.sr = state[SDL_SCANCODE_RIGHTBRACKET];
			            K.sl = state[SDL_SCANCODE_LEFTBRACKET];
                        player_angle_move_offset();
				        break;
                }
        }
    }

    return quit;
}

void player_angle_move_offset()
{
    if (K.a == 1) { P.angle -= 4; if (P.angle < 0) { P.angle += 360;} }
    if (K.d == 1) { P.angle += 4; if (P.angle > 359) { P.angle -= 360;} }
    int dx = M.sin[P.angle] * 10.0;
    int dy = M.cos[P.angle] * 10.0;

    if (K.w == 1) { P.position.x += dx; P.position.y += dy;}
    if (K.s == 1) { P.position.x -= dx; P.position.y -= dy; }

    if (K.sr == 1) { P.position.x += dy; P.position.y += dx; }
    if (K.sl == 1) { P.position.x -= dy; P.position.y -= dx; }

}

void draw_3D(SDL_Renderer* renderer)
{
   float px[4], py[4], pz[4];

    float CS = M.cos[P.angle];
    float SN = M.sin[P.angle];

    float x1 = 40 - P.position.x; int y1 = 10 - P.position.y;
    float x2 = 40 - P.position.x; int y2 = 290 - P.position.y;

    float centerX = WINDOW_WIDTH / 2;
    float centerY = WINDOW_HEIGHT / 2;

    px[0] = x1 * CS - y1 * SN;
    px[1] = x2 * CS - y2 * SN;
    px[2] = px[0];
    px[3] = px[1];

    py[0] = y1 * CS + x1 * SN;
    py[1] = y2 * CS + x2 * SN;
    py[2] = py[0];
    py[3] = py[1];

    pz[0] = 0 - P.position.z + ((P.l * py[0]/32.0));
    pz[1] = 0 - P.position.z + ((P.l * py[1] / 32.0));
    pz[2] = pz[0] + 40;
    pz[3] = pz[1] + 40;

    px[0] = px[0] * FOV/ py[0] + WINDOW_WIDTH / 2;
    px[1] = px[1] * FOV / py[1] + WINDOW_WIDTH / 2;
    px[2] = px[2] * FOV / py[2] + WINDOW_WIDTH / 2;
    px[3] = px[3] * FOV / py[3] + WINDOW_WIDTH / 2;

    py[0] = pz[0] * FOV / py[0] + WINDOW_HEIGHT / 2;
    py[1] = pz[1] * FOV / py[1] + WINDOW_HEIGHT / 2;
    py[2] = pz[2] * FOV / py[2] + WINDOW_HEIGHT / 2;
    py[3] = pz[3] * FOV / py[3] + WINDOW_HEIGHT / 2;

    draw_wall(renderer, px[0], px[1], py[0], py[1], py[2], py[3]);
}

void draw_wall(SDL_Renderer* renderer, 
    int x1, int x2, int b1, int b2, int t1, int t2)
{

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderPresent(renderer);

    int x, y, y1, y2;
    int dyb = b2 - b1;
    int dyt = t2 - t1;
    int dx = x2 - x1; if (dx == 0) { dx = 1; }

    int xs = x1;

    for (x = x1; x < x2; x++)
    {
        y1 = dyb * (x - xs + 0.5) / dx + b1;
        y2 = dyt * (x - xs + 0.5) / dx + t1;

        for (y = y1; y < y2; y++)
        {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

}