#pragma once
#include <stdint.h>
#include <stdbool.h>

bool read_uint16_be(const void *buffer, const size_t buffer_size, const size_t offset, uint16_t *out);

bool read_uint32_be(const void *buffer, const size_t buffer_size, const size_t offset, uint32_t *out);

bool read_float_be(const void *buffer, const size_t buffer_size, const size_t offset, float *out);

bool read_raw(const void *buffer, const size_t buffer_size, const size_t offset, const size_t size, void *out);