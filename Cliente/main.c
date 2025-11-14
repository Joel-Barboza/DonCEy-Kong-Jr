#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "socket_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "widgets/button.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
    #define read(s, b, l) recv(s, b, l, 0)
    #define write(s, b, l) send(s, b, l, 0)
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

#define PORT 8080
#define BUFFER_SIZE 1024
static int connected = 0;

// -----------------------------
// Nivel: plataformas y lianas
// -----------------------------
#define NUM_PLATFORMS 9
#define NUM_VINES 12

SDL_FRect platforms[NUM_PLATFORMS];
SDL_FRect vines[NUM_VINES];

void initialize_level_elements() {

    platforms[0] = (SDL_FRect){ 20.0f,  60.0f, 400.0f, 18.0f };
    platforms[1] = (SDL_FRect){ 400.0f,  80.0f, 250.0f, 18.0f };
    platforms[2] = (SDL_FRect){ 600.0f,  300.0f, 250.0f, 18.0f };
    platforms[3] = (SDL_FRect){ 200.0f,  200.0f, 100.0f, 18.0f };
    platforms[4] = (SDL_FRect){ 200.0f,  310.0f, 200.0f, 18.0f };


    vines[0]  = (SDL_FRect){  680.0f,  30.0f, 6.0f,  380.0f };
    vines[1]  = (SDL_FRect){ 750.0f,  30.0f, 6.0f,  380.0f };
    vines[2]  = (SDL_FRect){ 280.0f,  200.0f, 6.0f,  200.0f };

    vines[3]  = (SDL_FRect){  80.0f,  78.0f, 6.0f,  300.0f };
    vines[4]  = (SDL_FRect){ 150.0f,  78.0f, 6.0f,  300.0f };
    vines[5]  = (SDL_FRect){ 430.0f,  98.0f, 6.0f,  300.0f };
    vines[6]  = (SDL_FRect){ 480.0f,  98.0f, 6.0f,  200.0f };
    vines[7]  = (SDL_FRect){ 590.0f,  98.0f, 6.0f,  260.0f };
    vines[8]  = (SDL_FRect){ 530.0f,  98.0f, 6.0f,  300.0f };




}

// Dibuja una "textura" simple de plataforma estilo arcade (base + borde)
void draw_platform_rect(SDL_Renderer* renderer, SDL_FRect r) {
    // base marrón
    SDL_SetRenderDrawColor(renderer, 150, 70, 30, 255);
    SDL_RenderFillRect(renderer, &r);

    // borde superior (línea brillante)
    SDL_SetRenderDrawColor(renderer, 250, 150, 60, 255);
    SDL_FRect top = { r.x, r.y, r.w, 4 };
    SDL_RenderFillRect(renderer, &top);

    // líneas horizontales decorativas (simular ladrillos)
    SDL_SetRenderDrawColor(renderer, 170, 90, 40, 255);
    float brick_h = 4.0f;
    for (float bx = r.x + 6.0f; bx + 20.0f < r.x + r.w; bx += 26.0f) {
        SDL_FRect brick = { bx, r.y + 6.0f, 20.0f, brick_h };
        SDL_RenderFillRect(renderer, &brick);
    }
}

void draw_platforms(SDL_Renderer* renderer) {
    for (int i = 0; i < NUM_PLATFORMS; ++i) {
        draw_platform_rect(renderer, platforms[i]);
    }
}

void draw_vines(SDL_Renderer* renderer) {
    for (int i = 0; i < NUM_VINES; ++i) {
        SDL_FRect v = vines[i];

        // tronco principal (verde claro)
        SDL_SetRenderDrawColor(renderer, 90, 200, 80, 255);
        SDL_RenderFillRect(renderer, &v);

        // nudos / hojas cada 28 px a partir del inicio
        SDL_SetRenderDrawColor(renderer, 20, 120, 20, 255);
        float knot_step = 28.0f;
        for (float y = v.y + 10.0f; y < v.y + v.h; y += knot_step) {
            SDL_FRect knot = { v.x - 5.0f, y, v.w + 10.0f, 4.0f };
            SDL_RenderFillRect(renderer, &knot);
        }


        if (v.h > 200.0f) {
            SDL_SetRenderDrawColor(renderer, 30, 160, 40, 255);
            SDL_FRect leaf = { v.x - 8.0f, v.y + v.h - 8.0f, v.w + 16.0f, 6.0f };
            SDL_RenderFillRect(renderer, &leaf);
        }
    }
}

// Dibuja "árboles" verdes en la base (opcional para aproximar la imagen)
void draw_ground_trees(SDL_Renderer* renderer) {
    // raíces/madera
    SDL_SetRenderDrawColor(renderer, 120, 60, 20, 255);
    SDL_FRect trunks[] = {
        {70, 480, 60, 40}, {200, 495, 60, 25}, {340, 480, 60, 40}, {480, 495, 60, 25}, {620, 480, 60, 40}
    };
    for (int i = 0; i < sizeof(trunks)/sizeof(trunks[0]); ++i) {
        SDL_RenderFillRect(renderer, &trunks[i]);
    }

    // copas verdes (superposición para darle estilo pixelado)
    SDL_SetRenderDrawColor(renderer, 20, 180, 40, 255);
    SDL_FRect crowns[] = {
        {50, 450, 100, 36}, {180, 470, 100, 30}, {320, 450, 100, 36}, {460, 470, 100, 30}, {600, 450, 100, 36}
    };
    for (int i = 0; i < sizeof(crowns)/sizeof(crowns[0]); ++i) {
        SDL_RenderFillRect(renderer, &crowns[i]);
        // borde de la copa
        SDL_SetRenderDrawColor(renderer, 80, 220, 70, 255);
        SDL_FRect top = { crowns[i].x, crowns[i].y, crowns[i].w, 6 };
        SDL_RenderFillRect(renderer, &top);
        SDL_SetRenderDrawColor(renderer, 20, 180, 40, 255);
    }
}

// ---------------------------
// Ventana y loop principal
// ---------------------------
int window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        SDL_Log("TTF init failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Donkey Kong Jr - Nivel (sin monos)", 800, 600, 0);
    if (!window) {
        SDL_Log("Window create failed: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer create failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    TTF_Font *font = TTF_OpenFont("resources/arial.ttf", 20);
    if (!font) {
        SDL_Log("Failed to load font. Continuing without text.");
    }

    Button btn = create_button(renderer, 300, 550, 200, 36, "Conectar", font ? font : NULL);

    initialize_level_elements();

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = 0;

            if (button_handle_event(&btn, &e)) {
                if (!connected) {
                    SDL_Log("Intentando conectar...");
                    SDL_CreateThread(socket_thread, "SocketThread", NULL);
                } else {
                    SDL_Log("Reintentando conexión...");
                    retry_connection();
                }
            }
        }

        // Fondo negro (como en la captura)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dibujar elementos: plataformas, lianas y árboles de la base
        draw_platforms(renderer);
        draw_vines(renderer);
        draw_ground_trees(renderer);

        // botón (mantiene la interfaz de tu proyecto)
        draw_button(renderer, &btn);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ~60 FPS
    }

    destroy_button(&btn);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main() {
    return window();
}
