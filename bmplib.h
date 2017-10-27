#ifndef BMPLIB_H
#define BMPLIB_H

#include "bmpheaders.h"

/*   Structures declarations   */
#pragma pack(1)

#if BMP_BIT_COUNT == 24
typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} pixel_t;
#else
#error "Invalid BMP_BIT_COUNT"
#endif
typedef struct {
	uint16_t y;
	uint16_t x;
	uint8_t r;
	uint8_t g;
	uint8_t b;
} compressed_point_t;

#pragma pack()

typedef struct {
	int width, height;
	pixel_t **pixels;
} bitmap_t;

/*   Functions declarations   */
/**
 *    Allocate the memory for the pixel array of a bitmap, assigning the
 * width and height members too.
 *    @return 0 if successful or an error code otherwise;
 */
int initialize_bitmap(bitmap_t *p_bitmap,
                      int width,
                      int height);

/**
 *    Deallocate the bitmap.
 *    @return 0 if successful or an error code otherwise;
 */
int clear_bitmap(bitmap_t *p_bitmap);

/**
 *    Apply a grayscale effect to @p_bitmap, storing the result in
 * @p_new_bitmap. @p_new_bitmap should be allocated prior to the call of this
 * function and should have the same width and height as @p_bitmap.
 *    @return 0 if successful or an error code otherwise;
 */
int grayscale_bitmap(bitmap_t *p_new_bitmap,
                      const bitmap_t *p_bitmap);

/**
 *    Apply a filter to @p_bitmap, storing the result in @p_new_bitmap.
 * @p_new_bitmap should be allocated prior to the call of this function and
 * should have the same width and height as @p_bitmap.
 *    @return 0 if successful or an error code otherwise;
 */
int filter_bitmap(bitmap_t *p_new_bitmap,
                  const bitmap_t *p_bitmap,
                  int filter[3][3]);

/**
 *    Reduce the number of colors of @p_bitmap, based on @threshold, and store
 * the result in @p_new_bitmap. @p_new_bitmap should be allocated prior to the
 * call of this function and should have the same width and height as
 * @p_bitmap.
 *    @return 0 if successful or an error code otherwise;
 */
int compress_bitmap(bitmap_t *p_new_bitmap,
                    const bitmap_t *p_bitmap,
                    int threshold);

/**
 *    Read a bmp file located at @file_name. @p_bitmap should not be allocated
 * prior to the call of this function. If the reading is unsuccessful, the state
 * of the arguments is unknown and should be deallocated.
 *    @return 0 if successful or an error code otherwise;
 */
int read_bmp(const char file_name[],
             bmp_file_header_t *p_file_header,
             bmp_info_header_t *p_info_header,
             bitmap_t *p_bitmap);

/**
 *    Write a bmp file to @file_name.
 *    @return 0 if successful or an error code otherwise;
 */
int write_bmp(const char file_name[],
             const bmp_file_header_t *p_file_header,
             const bmp_info_header_t *p_info_header,
             const bitmap_t *p_bitmap);

/**
 *    Read a compressed bmp file located at @file. @p_bitmap should not be
 * allocated prior to the call of this function. If the reading is unsuccessful,
 * the state of the arguments is unknown and should be deallocated.
 *    @return 0 if successful or an error code otherwise;
 */
int read_compressed_bmp(const char file_name[],
             bmp_file_header_t *p_file_header,
             bmp_info_header_t *p_info_header,
             bitmap_t *p_bitmap);

/**
 *    Write a bmp compressed file to @file_name.
 *    @return 0 if successful or an error code otherwise;
 */
int write_compressed_bmp(const char file_name[],
             const bmp_file_header_t *p_file_header,
             const bmp_info_header_t *p_info_header,
             const bitmap_t *p_bitmap);

#endif 
