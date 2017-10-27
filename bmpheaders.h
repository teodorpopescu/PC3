#ifndef BMPHEADERS_H
#define BMPHEADERS_H

#include <stdint.h>

#define BMP_SIGNATURE 0x4d42
#define BMP_INFO_HEADER_SIZE 40
#define BMP_BIT_COUNT 24
#define MAX_PIXEL_VALUE 255

#pragma pack(1)

/*   File Header declaration   */
typedef struct {
	uint16_t signature;
	uint32_t file_size;
	uint32_t reserved;
	uint32_t offset;
} bmp_file_header_t;

/*   BITMAPINFOHEADER alternative declaration   */
typedef struct {
	uint32_t header_size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bit_count;
	uint32_t compression;
	uint32_t image_size;
	uint32_t x_pixels_per_meter;
	uint32_t y_pixels_per_meter;
	uint32_t colors_used;
	uint32_t colors_important;
} bmp_info_header_t;

#pragma pack()

#endif
