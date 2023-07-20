#pragma once
#include <stdint.h>

typedef struct {
	uint8_t const bits_per_pixel;
	uint8_t const block_width;
	uint8_t const block_height;
	uint8_t const block_size;
} Format;

typedef struct {
	void *bytes;
	const Format *format;
	uint32_t size;
} Data;

typedef struct {
	Data image;
	Data palette;
	uint16_t height; 
	uint16_t width;
	uint32_t wrap_s;
	uint32_t wrap_t;
	uint32_t min_filter;
	uint32_t mag_filter;
	float lod_bias;
	uint8_t edge_lod_enable;
	uint8_t min_lod;
	uint8_t max_lod;
} TPL;

typedef struct {
	TPL *tpls;
	uint32_t size;
} TPL_Container;

TPL_Container *load_tpl_container(const void *buffer, const size_t buffer_size);

void free_tpl_container(TPL_Container *tpl_container);