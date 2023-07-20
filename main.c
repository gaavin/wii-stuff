#include <emscripten.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "conversions.h"
#include "tpl.h"

typedef struct Context {
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Window *window;
} Context;

void cleanup(Context* context) {
    SDL_DestroyTexture(context->texture);
    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyWindow(context->window);
    SDL_Quit();
}

void render_image(void *arg) {
    Context *context = (Context*)arg;
    SDL_Renderer *renderer = context->renderer;
    SDL_Texture *texture = context->texture;

    if (!texture) {
        printf("Could not create texture: %s\n", SDL_GetError());
        cleanup(context);
        emscripten_cancel_main_loop();
        return;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main() {
    FILE *file = fopen("savebanner.tpl", "rb"); // example file

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *buffer = malloc(filesize);
    if (!buffer) {
        fclose(file);
    }

    if (fread(buffer, 1, filesize, file) != filesize) {
        free(buffer);
        buffer = NULL;
    }
    fclose(file);

	TPL_Container *tpl_container = load_tpl_container(buffer, filesize);
    free(buffer);
    TPL tpl = tpl_container->tpls[1];

	SDL_Init(SDL_INIT_VIDEO);
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window;
    SDL_Renderer *renderer;
    if (SDL_CreateWindowAndRenderer(tpl.width, tpl.height, 0, &window, &renderer) != 0) {
        printf("Could not create window and renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    if (renderer == NULL) {
        printf("Could not get renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *surf = from_RGB5A3(&tpl);

    if (!surf) {
        printf("Could not create surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    Context context = { renderer, texture, window };

    //while (true) {
        //render_image(&context);
    //}
    emscripten_set_main_loop_arg(render_image, &context, -1, 1);
};