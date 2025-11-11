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

//static SDL_Thread *socket_thread_handle = NULL;

int window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return -1;
    }
    if (TTF_Init() < 0) {
        SDL_Log("TTF init failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello", 800, 600, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);


    TTF_Font *font = TTF_OpenFont("resources/arial.ttf", 24);

    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return -1;
    }
    Button btn = create_button(renderer, 300, 250, 200, 100, "Conectar", font);

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

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        draw_button(renderer, &btn);

        SDL_RenderPresent(renderer);
    }

    destroy_button(&btn);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


int main() {

    //_beginthread((void(*)(void*))socket_thread, 0, NULL);
    window();
    return 0;
}