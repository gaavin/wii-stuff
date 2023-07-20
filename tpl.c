#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "common.h"
#include "tpl.h"

#define IMAGE_COUNT_OFFSET 0x4
#define IMAGE_OFFSET_TABLE_OFFSET 0xC
#define PALETTE_ENTRY_SIZE 0x2

Format const I4 = { .bits_per_pixel = 4, .block_width = 8, .block_height = 8, .block_size = 32 };
Format const I8 = { .bits_per_pixel = 8, .block_width = 8, .block_height = 4, .block_size = 32 };
Format const IA4 = { .bits_per_pixel = 8, .block_width = 8, .block_height = 4, .block_size = 32 };
Format const IA8 = { .bits_per_pixel = 16, .block_width = 4, .block_height = 4, .block_size = 32 };
Format const RGB565 = { .bits_per_pixel = 16, .block_width = 4, .block_height = 4, .block_size = 32 };
Format const RGB5A3 = { .bits_per_pixel = 16, .block_width = 4, .block_height = 4, .block_size = 32 };
Format const RGBA32 = { .bits_per_pixel = 32, .block_width = 4, .block_height = 4, .block_size = 64 };
Format const C4 = { .bits_per_pixel = 4, .block_width = 8, .block_height = 8, .block_size = 32 };
Format const C8 = { .bits_per_pixel = 8, .block_width = 8, .block_height = 4, .block_size = 32 };
Format const C14X2 = { .bits_per_pixel = 16, .block_width = 4, .block_height = 4, .block_size = 32 };
Format const CMPR = { .bits_per_pixel = 4, .block_width = 8, .block_height = 8, .block_size = 32 };

const Format *image_formats[15] = { &I4, &I8, &IA4, &IA8, &RGB565, &RGB5A3, &RGBA32, NULL, &C4, &C8, &C14X2, NULL, NULL, NULL, &CMPR};
const Format *palette_formats[3] = { &IA8, &RGB565, &RGB5A3 };

const Format *get_image_format_by_id(const uint32_t id) {
    if (id >= sizeof(image_formats) / sizeof(*image_formats)) {
        return NULL;
    }
    return image_formats[id];
}

const Format *get_palette_format_by_id(const uint32_t id) {
	if (id >= sizeof(palette_formats) / sizeof(*palette_formats)) {
        return NULL;
    }
    return palette_formats[id];
}

uint32_t get_image_data_size(const TPL *tpl) {
    uint16_t width_blocks = (tpl->width + tpl->image.format->block_width - 1) / tpl->image.format->block_width;
    uint16_t height_blocks = (tpl->height + tpl->image.format->block_height - 1) / tpl->image.format->block_height;

    if (width_blocks == 0 || height_blocks == 0 || tpl->image.format->block_size == 0) return 0;

    uint32_t total_blocks = width_blocks * height_blocks;

    return total_blocks * tpl->image.format->block_size;
}

TPL_Container *load_tpl_container(const void *buffer, const size_t buffer_size) {
	TPL_Container *tpl_container = (TPL_Container*)malloc(sizeof(TPL_Container));
	if (!tpl_container) {
        printf("Failed to allocate memory for TPL_Container\n");
        return NULL;
    }

    if(!read_uint32_be(buffer, buffer_size, IMAGE_COUNT_OFFSET, &tpl_container->size)) {
        printf("Failed to read the container size\n");
        free_tpl_container(tpl_container);
        return NULL;
    }
    printf("Read the container size: %u\n", tpl_container->size);

	tpl_container->tpls = (TPL*)calloc(tpl_container->size, sizeof(TPL));
    if (!tpl_container->tpls) {
        printf("Failed to allocate memory for TPLs\n");
        free_tpl_container(tpl_container);
        return NULL;
    }

    TPL *tpl = tpl_container->tpls;
    for (size_t i = 0; i < tpl_container->size; i++, tpl++) {
		uint32_t image_header_offset;
        if(!read_uint32_be(buffer, buffer_size, IMAGE_OFFSET_TABLE_OFFSET+(i*0x8), &image_header_offset)) {
            printf("Failed to read image_header_offset\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read image_header_offset: %u\n", image_header_offset);

        uint32_t palette_header_offset;
        if(!read_uint32_be(buffer, buffer_size, IMAGE_OFFSET_TABLE_OFFSET+(i*0x8)+0x4, &palette_header_offset)) {
            printf("Failed to read palette_header_offset\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read palette_header_offset: %u\n", palette_header_offset);

        uint32_t format_id;
        if(!read_uint32_be(buffer, buffer_size, image_header_offset+0x4, &format_id)) {
            printf("Failed to read the format id\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read the format id: %u\n", format_id);

        tpl->image.format = get_image_format_by_id(format_id);
        if (tpl->image.format == NULL) {
            printf("Invalid format id\n");
		    free_tpl_container(tpl_container);
		    return NULL;
		}

		if(!read_uint16_be(buffer, buffer_size, image_header_offset, &tpl->height)) {
            printf("Failed to read height\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read height: %u\n", tpl->height);

        if(!read_uint16_be(buffer, buffer_size, image_header_offset+0x2, &tpl->width)) {
            printf("Failed to read width\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read width: %u\n", tpl->width);

        if(!read_uint32_be(buffer, buffer_size, image_header_offset+0xC, &tpl->wrap_s)) {
            printf("Failed to read wrap_s\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read wrap_s: %u\n", tpl->wrap_s);

        if(!read_uint32_be(buffer, buffer_size, image_header_offset+0x10, &tpl->wrap_t)) {
            printf("Failed to read wrap_t\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read wrap_t: %u\n", tpl->wrap_t);

        if(!read_uint32_be(buffer, buffer_size, image_header_offset+0x14, &tpl->min_filter)) {
            printf("Failed to read min_filter\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read min_filter: %u\n", tpl->min_filter);

        if(!read_uint32_be(buffer, buffer_size, image_header_offset+0x18, &tpl->mag_filter)) {
            printf("Failed to read mag_filter\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read mag_filter: %u\n", tpl->mag_filter);

        if(!read_float_be(buffer, buffer_size, image_header_offset+0x1C, &tpl->lod_bias)) {
            printf("Failed to read lod_bias\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read lod_bias: %f\n", tpl->lod_bias);

        if (!read_raw(buffer, buffer_size, image_header_offset+0x20, 0x1, &tpl->edge_lod_enable)) {
            printf("Failed to read edge_lod_enable\n");
    		free_tpl_container(tpl_container);
    		return NULL;
		}
        printf("Read edge_lod_enable: %u\n", tpl->edge_lod_enable);

		if (!read_raw(buffer, buffer_size, image_header_offset+0x21, 0x1, &tpl->min_lod)) {
            printf("Failed to read min_lod\n");
    		free_tpl_container(tpl_container);
    		return NULL;
    	}
        printf("Read min_lod: %u\n", tpl->min_lod);

    	if (!read_raw(buffer, buffer_size, image_header_offset+0x22, 0x1, &tpl->max_lod)) {
            printf("Failed to read max_lod\n");
    		free_tpl_container(tpl_container);
    		return NULL;
    	}
        printf("Read max_lod: %u\n", tpl->max_lod);

		tpl->image.size = get_image_data_size(tpl);
		tpl->image.bytes = (void*)malloc(tpl->image.size);
		if (!tpl->image.bytes) {
			printf("Failed to allocate memory for image bytes\n");
			free_tpl_container(tpl_container);
			return NULL;
		}
        printf("Allocated memory for image bytes of size: %u\n", tpl->image.size);

    	uint32_t image_bytes_offset;
    	if(!read_uint32_be(buffer, buffer_size, image_header_offset+0x8, &image_bytes_offset)) {
            printf("Failed to read image bytes offset\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read image bytes offset: %u\n", image_bytes_offset);

    	if (!read_raw(buffer, buffer_size, image_bytes_offset, tpl->image.size, tpl->image.bytes)) {
            printf("Failed to read image bytes\n");
    		free_tpl_container(tpl_container);
    		return NULL;
    	}
        printf("Read image bytes\n");

    	uint16_t palette_entry_count;
    	if (!read_uint16_be(buffer, buffer_size, palette_header_offset, &palette_entry_count)) {
            printf("Failed to read palette entry count\n");
    		free_tpl_container(tpl_container);
    		return NULL;
    	}
        printf("Read palette entry count: %u\n", palette_entry_count);

    	tpl->palette.size = palette_entry_count * PALETTE_ENTRY_SIZE;
    	tpl->palette.bytes = (void*)malloc(tpl->palette.size);
    	if (!tpl->palette.bytes) {
            printf("Failed to allocate memory for palette bytes\n");
			free_tpl_container(tpl_container);
			return NULL;
		}
        printf("Allocated memory for palette bytes of size: %u\n", tpl->palette.size);

    	uint32_t palette_bytes_offset;
    	if(!read_uint32_be(buffer, buffer_size, palette_header_offset+0x8, &palette_bytes_offset)) {
            printf("Failed to read palette bytes offset\n");
            free_tpl_container(tpl_container);
            return NULL;
        }
        printf("Read palette bytes offset: %u\n", palette_bytes_offset);

    	if (!read_raw(buffer, buffer_size, palette_bytes_offset, tpl->palette.size, tpl->palette.bytes)) {
            printf("Failed to read palette bytes\n");
    		free_tpl_container(tpl_container);
    		return NULL;
    	}
        printf("Read palette bytes\n");
	}
	return tpl_container;
}

void free_tpl_container(TPL_Container *tpl_container) {
    if (!tpl_container) {
        return;
    }

    if (tpl_container->tpls) {
        for (size_t i = 0; i < tpl_container->size; i++) {
            TPL *tpl = &tpl_container->tpls[i];
            free(tpl->image.bytes);
            free(tpl->palette.bytes);
        }
        free(tpl_container->tpls);
    }
    printf("Freed TPL Container");
    free(tpl_container);
}