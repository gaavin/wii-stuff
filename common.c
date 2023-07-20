#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <endian.h>
#include <stdio.h>

bool read_uint16_be(const void *buffer, const size_t buffer_size, const size_t offset, uint16_t *out) {
    if (buffer_size < offset + sizeof(uint16_t)) {
        return false;
    }
    uint16_t tmp;
    memcpy(&tmp, buffer + offset, sizeof(uint16_t));
    *out = be16toh(tmp);
    return true;
}

bool read_uint32_be(const void *buffer, const size_t buffer_size, const size_t offset, uint32_t *out) {
    if (buffer_size < offset + sizeof(uint32_t)) {
        return false;
    }
    uint32_t tmp;
    memcpy(&tmp, buffer + offset, sizeof(uint32_t));
    *out = be32toh(tmp);
    return true;
}

bool read_float_be(const void *buffer, const size_t buffer_size, const size_t offset, float *out) {
    if (buffer_size < offset + sizeof(float)) {
        return false;
    }
    uint32_t tmp;
    memcpy(&tmp, buffer + offset, sizeof(float));
    *out = be32toh(tmp);
    return true;
}

bool read_raw(const void *buffer, const size_t buffer_size, const size_t offset, const size_t size, void *out) {
    if (buffer_size < offset + size) {
        return false;
    }
    memcpy(out, buffer + offset, size);
    return true;
}