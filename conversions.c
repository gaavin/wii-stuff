#include <SDL2/SDL.h>
#include <endian.h>
#include <stdint.h>
#include "conversions.h"
#include "tpl.h"

SDL_Surface* from_RGB5A3(TPL* tpl) {
    SDL_Surface *surface = SDL_CreateRGBSurface(0, tpl->width, tpl->height, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (!surface) {
        return NULL;
    }

    uint16_t* pixel_data = (uint16_t*) tpl->image.bytes;

    // iterate over each block
    for (int by = 0; by < tpl->height; by += tpl->image.format->block_height) {
        for (int bx = 0; bx < tpl->width; bx += tpl->image.format->block_width) {

            // iterate over each pixel in the block
            for (int py = 0; py < tpl->image.format->block_height; ++py) {
                for (int px = 0; px < tpl->image.format->block_width; ++px) {

                    int x = bx + px;
                    int y = by + py;

                    if (x < tpl->width && y < tpl->height) {
                        uint16_t pixel = be16toh(pixel_data[((by / tpl->image.format->block_height) * (tpl->width / tpl->image.format->block_width) + (bx / tpl->image.format->block_width)) * tpl->image.format->block_width * tpl->image.format->block_height + py * tpl->image.format->block_width + px]);
                        uint8_t r, g, b, a;

					if ((pixel & 0x8000) != 0) {
					    r = ((pixel >> 10) & 0x1F) << 3;
					    g = ((pixel >> 5) & 0x1F) << 3;
					    b = (pixel & 0x1F) << 3;
					    a = 0xFF;
					} else {
					    a = ((pixel >> 12) & 0x7) << 5;
					    r = ((pixel >> 8) & 0xF) << 4;
					    g = ((pixel >> 4) & 0xF) << 4;
					    b = (pixel & 0xF) << 4;
					}

                        uint32_t* surface_pixels = (uint32_t*) surface->pixels;
                        surface_pixels[y * tpl->width + x] = (a << 24) | (b << 16) | (g << 8) | r;
                    }
                }
            }
        }
    }

    return surface;
}