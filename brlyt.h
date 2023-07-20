#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "brfnt.h"
#include "tpl.h"

typedef struct {
	Material *material;
	uint8_t flip_type;
} Window_Frame;

typedef struct {
	uint8_t comparison;
	uint8_t operation;
	uint8_t comp_a;
	uint8_t comp_b;
} Alpha_Compare;

typedef struct {
	float translation[2];
	float rotation;
	float scale[2];
} SRT;

typedef struct {
	TPL *texture;
	UV_Coordinates *uv;
	uint8_t wrap_s;
	uint8_t wrap_t;
} Indirect_Stage;

typedef struct {
    uint8_t raster_swap_table[4];
    uint8_t texture_swap_table[4];
} TEV_Swap_Table;

typedef struct {
    Material *alpha;
    Material *color;
} Channel_Control;

typedef struct {
    float top_left[2];
    float top_right[2];
    float bottom_left[2];
    float bottom_right[2];
} UV_Coordinates;

typedef struct {
	char name[14];
	uint16_t fore_color[4];
	uint16_t back_color[4];
	uint16_t color_register_3[4];
	uint16_t tev_colors[4][4];
	uint16_t color[4];
	uint8_t tev_stage_count;
    Channel_Control *channel_control;
	Alpha_Compare *alpha_compare;
	Indirect_Stage *indirect_stages;
	SRT *indirect_srts;
	TEV_Swap_Table *tev_swap_table;
	SRT *srts;
} Material;

typedef struct {
    bool visible;
    bool influenced_alpha;
    bool widescreen;
    uint8_t alpha;
    char name[10];
    float translation[3];
    float rotation[3];
    float scale[2];
    float width;
    float height;
    Pane *children;
    union {
    	struct {
            Material *material;
            BRFNT *font;
            uint8_t line_alignment;
            uint8_t top_color[4];
            uint8_t bottom_color[4];
            float font_size[2];
            float character_size;
            float line_size;
            char *text;
        } text;
        struct {
            Material *material;
            uint8_t vertex_color[4][4];
            UV_Coordinates *uvs;
        } image;
        struct {
            Material *material;
            uint8_t vertex_color[4][4];
            UV_Coordinates *uvs;
            float coordinates[4];
            Window_Frame *window_frames;
        } window;
    };
    enum {
    	NULL_PANE,
    	TEXT_PANE,
        IMAGE_PANE,
        WINDOW_PANE
    } type;
} Pane;

typedef struct {
	bool centered;
	float width;
	float height;
	Pane *root_pane;
} BRLYT;

BRLYT *load_BRLYT(char *buffer);