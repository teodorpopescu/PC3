#include <stdlib.h>
#include <stdio.h>

#include "bmplib.h"
#include "stack.h"

int initialize_bitmap(bitmap_t *p_bitmap, int w, int h)
{
	if (w <= 0 || h <= 0) {
		fprintf(stderr, "Invalid height or width for bitmap\n");
		return 1;
	}

	p_bitmap->width = w;
	p_bitmap->height = h;

	/* Allocate the pixel matrix, catching the allocation errors */
	p_bitmap->pixels = malloc(h * sizeof(pixel_t *));
	if (p_bitmap->pixels == NULL) {
		fprintf(stderr, "Not enough memory\n");
		return 1;
	}
	for (int i = 0; i < h; ++i) {
		p_bitmap->pixels[i] = malloc(w * sizeof(pixel_t));
		if (p_bitmap->pixels[i] == NULL) {
			fprintf(stderr, "Not enough memory\n");
			for (int j = 0; j < i; ++j) free(p_bitmap->pixels[j]);
			free(p_bitmap->pixels);
			p_bitmap->pixels = NULL;
			return 1;
		}
	}

	return 0;
}

int clear_bitmap(bitmap_t *p_bitmap)
{
	if (p_bitmap == NULL) return 0;
	if (p_bitmap->pixels == NULL) return 0;
	for (int i = 0; i < p_bitmap->height; ++i) free(p_bitmap->pixels[i]);
	free(p_bitmap->pixels);
	p_bitmap->pixels = NULL;
	return 0;
}

int read_bmp(const char file_name[],
             bmp_file_header_t *p_file_header,
             bmp_info_header_t *p_info_header,
             bitmap_t *p_bitmap)
{
	FILE *p_file;
	int w, h, padding, e;

	p_file = fopen(file_name, "rb");
	if (p_file == NULL) {
		fprintf(stderr, "Can't open file %s\n", file_name);
		return 1;
	}

	/* Read File Header */
	e = fread(p_file_header, sizeof(bmp_file_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while reading the File Header\n");
		fclose(p_file);
		return 1;
	}
	if (p_file_header->signature != BMP_SIGNATURE) {
		fprintf(stderr, "Invalid BMP signature: %X\n",
			p_file_header->signature);
		fclose(p_file);
		return 1;
	}

	/* Read Info Header */
	e = fread(p_info_header, sizeof(bmp_info_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while reading the Info Header\n");
		fclose(p_file);
		return 1;
	}

	/* Read the pixel array */
	w = p_info_header->width;
	h = p_info_header->height;
	padding = (w * sizeof(pixel_t)) - (w * sizeof(pixel_t) / 4) * 4;
	e = initialize_bitmap(p_bitmap, w, h);
	if (e != 0) {
		fprintf(stderr, "Error while initializing the bitmap");
		fclose(p_file);
		return 1;
	}
	e = fseek(p_file, p_file_header->offset, SEEK_SET);
	if (e != 0) {
		fprintf(stderr, "Error while moving cursor to %d\n",
			p_file_header->offset);
		fclose(p_file);
		return 1;
	}
	for (int i = h - 1; i >= 0; --i) {
		e = fread(p_bitmap->pixels[i], sizeof(pixel_t), w, p_file);
		if (e != w) {
			fprintf(stderr, "Error while reading line %d\n", i);
			fclose(p_file);
			return 1;
		}
		e = fseek(p_file, padding, SEEK_CUR);
		if (e != 0) {
			fprintf(stderr, "Error while moving cursor with %d\n",
				padding);
			fclose(p_file);
			return 1;
		}
	}

	fclose(p_file);
	return 0;
}

int write_bmp(const char file_name[],
              const bmp_file_header_t *p_file_header,
              const bmp_info_header_t *p_info_header,
              const bitmap_t *p_bitmap)
{
	FILE *p_file;
	int w, h, padding, e;

	p_file = fopen(file_name, "wb");
	if (p_file == NULL) {
		fprintf(stderr, "Can't open file %s\n", file_name);
		return 1;
	}

	/* Write File Header */
	e = fwrite(p_file_header, sizeof(bmp_file_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while writing the File Header\n");
		fclose(p_file);
		return 1;
	}

	/* Write Info Header */
	e = fwrite(p_info_header, sizeof(bmp_info_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while writing the Info Header\n");
		fclose(p_file);
		return 1;
	}

	/* Write to offset */
	while (ftell(p_file) < (long)p_file_header->offset) {
		e = fputc(0, p_file);
		if (e != 0) {
			fprintf(stderr, "Error while writing to offset\n");
			fclose(p_file);
			return 1;
		}
	}

	/* Write the pixel array */
	w = p_info_header->width;
	h = p_info_header->height;
	padding = (w * sizeof(pixel_t)) - (w * sizeof(pixel_t) / 4) * 4;

	for (int i = h - 1; i >= 0; --i) {
		e = fwrite(p_bitmap->pixels[i], sizeof(pixel_t), w, p_file);
		if (e != w) {
			fprintf(stderr, "Error while writing line %d\n", i);
			fclose(p_file);
			return 1;
		}
		for (int j = 0; j < padding; ++j) {
			e = fputc(0, p_file);
			if (e != 0) {
				fprintf(stderr, "Error while writing padding");
				fclose(p_file);
				return 1;
			}
		}
	}

	fclose(p_file);
	return 0;
}

int grayscale_bitmap(bitmap_t *p_new_bitmap, const bitmap_t *p_bitmap)
{
	if (p_new_bitmap == NULL || p_bitmap == NULL
	    || p_new_bitmap->width != p_bitmap->width
	    || p_new_bitmap->height != p_bitmap->height) {
		fprintf(stderr, "Invalid arguments");
		return 1;
	}

	/* Apply the effect */
	for (int i = 0; i < p_bitmap->height; ++i) {
		for (int j = 0; j < p_bitmap->width; ++j) {
			int tmp = (p_bitmap->pixels[i][j].r
				+ p_bitmap->pixels[i][j].g
				+ p_bitmap->pixels[i][j].b) / 3;
			p_new_bitmap->pixels[i][j].r = tmp;
			p_new_bitmap->pixels[i][j].g = tmp;
			p_new_bitmap->pixels[i][j].b = tmp;
		}
	}

	return 0;
}

int filter_bitmap(bitmap_t *p_new_bitmap,
                   const bitmap_t *p_bitmap,
                   int filter[3][3])
{
	if (p_new_bitmap == NULL || p_bitmap == NULL
	    || p_new_bitmap->width != p_bitmap->width
	    || p_new_bitmap->height != p_bitmap->height) {
		fprintf(stderr, "Invalid arguments");
		return 1;
	}

	int w = p_bitmap->width;
	int h = p_bitmap->height;

	/* Apply the filter */
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int r = 0, g = 0, b = 0;
			for (int p = i - 1; p <= i + 1; ++p) {
				for (int q = j - 1; q <= j + 1; ++q) {
					if (p >= 0 && p < h
					    && q >= 0 && q < w) {
						r += p_bitmap->pixels[p][q].r
                                                     * filter[p-i+1][q-j+1];
						g += p_bitmap->pixels[p][q].g
						     * filter[p-i+1][q-j+1];
						b += p_bitmap->pixels[p][q].b
						     * filter[p-i+1][q-j+1];
					}
				}
			}
			if (r < 0) {
				p_new_bitmap->pixels[i][j].r = 0;
			} else if (r > MAX_PIXEL_VALUE) {
				p_new_bitmap->pixels[i][j].r = MAX_PIXEL_VALUE;
			} else {
				p_new_bitmap->pixels[i][j].r = r;
			}
			if (g < 0) {
				p_new_bitmap->pixels[i][j].g = 0;
			} else if (g > MAX_PIXEL_VALUE) {
				p_new_bitmap->pixels[i][j].g = MAX_PIXEL_VALUE;
			} else {
				p_new_bitmap->pixels[i][j].g = g;
			}
			if (b < 0) {
				p_new_bitmap->pixels[i][j].b = 0;
			} else if (b > MAX_PIXEL_VALUE) {
				p_new_bitmap->pixels[i][j].b = MAX_PIXEL_VALUE;
			} else {
				p_new_bitmap->pixels[i][j].b = b;
			}	
		}
	}

	return 0;
}

int is_similar(pixel_t px1, pixel_t px2, int threshold)
{
	int sum = 0;
	if (px1.r < px2.r) sum += px2.r - px1.r;
	else sum += px1.r - px2.r;
	if (px1.g < px2.g) sum += px2.g - px1.g;
	else sum += px1.g - px2.g;
	if (px1.b < px2.b) sum += px2.b - px1.b;
	else sum += px1.b - px2.b;
	if (sum <= threshold) return 1;
	else return 0;
}

int fill_bitmap(bitmap_t *p_new_bitmap,
                const bitmap_t *p_bitmap,
                uint8_t **flags,
                int x,
                int y,
                int threshold)
{
	stack_t stack;
	pixel_t pixel;
	int e;

	int w = p_bitmap->width;
	int h = p_bitmap->height;

	e = initialize_stack(&stack);
	if (e != 0) {
		fprintf(stderr, "Error initializing the stack\n");
		return 1;
	}

	/* Apply iterative dfs */
	flags[y][x] = 1;
	pixel = p_bitmap->pixels[y][x];
	p_new_bitmap->pixels[y][x] = pixel;
	stack_push(&stack, x, y);
	while (!stack_is_empty(&stack)) {
		int i = stack_query_y(&stack);
		int j = stack_query_x(&stack);
		stack_pop(&stack);
		if (j > 0 && flags[i][j - 1] == 0 &&
		    is_similar(p_bitmap->pixels[i][j - 1], pixel, threshold)) {
			flags[i][j - 1] = 1;
			p_new_bitmap->pixels[i][j - 1] = pixel;
			stack_push(&stack, j - 1, i);
		}
		if (j + 1 < w && flags[i][j + 1] == 0 &&
		    is_similar(p_bitmap->pixels[i][j + 1], pixel, threshold)) {
			flags[i][j + 1] = 1;
			p_new_bitmap->pixels[i][j + 1] = pixel;
			stack_push(&stack, j + 1, i);
		}
		if (i > 0 && flags[i - 1][j] == 0 &&
		    is_similar(p_bitmap->pixels[i - 1][j], pixel, threshold)) {
			flags[i - 1][j] = 1;
			p_new_bitmap->pixels[i - 1][j] = pixel;
			stack_push(&stack, j, i - 1);
		}
		if (i + 1 < h && flags[i + 1][j] == 0 &&
		    is_similar(p_bitmap->pixels[i + 1][j], pixel, threshold)) {
			flags[i + 1][j] = 1;
			p_new_bitmap->pixels[i + 1][j] = pixel;
			stack_push(&stack, j, i + 1);
		}
	}
	clear_stack(&stack);

	return 0;
}

int compress_bitmap(bitmap_t *p_new_bitmap,
                    const bitmap_t *p_bitmap,
                    int threshold)
{
	if (p_new_bitmap == NULL || p_bitmap == NULL
	    || p_new_bitmap->width != p_bitmap->width
	    || p_new_bitmap->height != p_bitmap->height) {
		fprintf(stderr, "Invalid arguments");
		return 1;
	}

	uint8_t **flags;
	int w, h;

	/* Initialize the temporary data */
	w = p_bitmap->width;
	h = p_bitmap->height;

	flags = malloc(h * sizeof(uint8_t *));
	if (flags == NULL) {
		fprintf(stderr, "Error allocating the flags\n");
		return 1;
	}
	for (int i = 0; i < h; ++i) {
		flags[i] = calloc(w, sizeof(uint8_t));
		if (flags[i] == NULL) {
			for (int j = 0; j < i; ++j) free(flags[j]);
			free(flags);
			fprintf(stderr, "Error allocating the flags\n");
			return 1;
		}
	}

	/* Applying the fill algorithm */
	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if (flags[i][j] == 0) {
				fill_bitmap(p_new_bitmap, p_bitmap, flags,
				            j, i, threshold);
			}
		}
	}

	/* Clean the temporary data */
	for (int i = 0; i < h; ++i) free(flags[i]);
	free(flags);

	return 0;
}

int read_compressed_bmp(const char file_name[],
                        bmp_file_header_t *p_file_header,
                        bmp_info_header_t *p_info_header,
                        bitmap_t *p_bitmap)
{
	FILE *p_file;
	int w, h, e;
	compressed_point_t p1, p2;

	p_file = fopen(file_name, "rb");
	if (p_file == NULL) {
		fprintf(stderr, "Can't open file %s\n", file_name);
		return 1;
	}

	/* Read File Header */
	e = fread(p_file_header, sizeof(bmp_file_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while reading the File Header\n");
		fclose(p_file);
		return 1;
	}
	if (p_file_header->signature != BMP_SIGNATURE) {
		fprintf(stderr, "Invalid BMP signature: %X\n",
			p_file_header->signature);
		fclose(p_file);
		return 1;
	}

	/* Read Info Header */
	e = fread(p_info_header, sizeof(bmp_info_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while reading the Info Header\n");
		fclose(p_file);
		return 1;
	}

	/* Read the compressed data, pixel by pixel */
	w = p_info_header->width;
	h = p_info_header->height;
	e = initialize_bitmap(p_bitmap, w, h);
	if (e != 0) {
		fprintf(stderr, "Error while initializing the bitmap");
		fclose(p_file);
		return 1;
	}
	e = fseek(p_file, p_file_header->offset, SEEK_SET);
	if (e != 0) {
		fprintf(stderr, "Error while moving cursor to %d\n",
			p_file_header->offset);
		fclose(p_file);
		return 1;
	}
	if (fread(&p1, sizeof(compressed_point_t), 1, p_file) != 1) {
		fprintf(stderr, "Error while reading the compressed data\n");
		fclose(p_file);
		return 1;
	}
	while (fread(&p2, sizeof(compressed_point_t), 1, p_file) == 1) {
		int i = p1.y - 1;
		if (p1.y != p2.y) {
			for (int j = p1.x - 1; j < w; ++j) {
				p_bitmap->pixels[i][j].r = p1.r;
				p_bitmap->pixels[i][j].g = p1.g;
				p_bitmap->pixels[i][j].b = p1.b;
			}
			p1 = p2;
			continue;
		}
		for (int j = p1.x - 1; j < p2.x - 1 && j < w; ++j) {
			p_bitmap->pixels[i][j].r = p1.r;
			p_bitmap->pixels[i][j].g = p1.g;
			p_bitmap->pixels[i][j].b = p1.b;
		}
		p1 = p2;
	}
	for (int j = p1.x - 1; j < w; ++j) {
		p_bitmap->pixels[p1.y - 1][j].r = p1.r;
		p_bitmap->pixels[p1.y - 1][j].g = p1.g;
		p_bitmap->pixels[p1.y - 1][j].b = p1.b;
	}

	fclose(p_file);
	return 0;
}



int write_compressed_bmp(const char file_name[],
                         const bmp_file_header_t *p_file_header,
                         const bmp_info_header_t *p_info_header,
                         const bitmap_t *p_bitmap)
{
	FILE *p_file;
	int w, h, e;

	p_file = fopen(file_name, "wb");
	if (p_file == NULL) {
		fprintf(stderr, "Can't open file %s\n", file_name);
		fclose(p_file);
		return 1;
	}

	/* Write File Header */
	e = fwrite(p_file_header, sizeof(bmp_file_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while writing the File Header\n");
		fclose(p_file);
		return 1;
	}

	/* Write Info Header */
	e = fwrite(p_info_header, sizeof(bmp_info_header_t), 1, p_file);
	if (e != 1) {
		fprintf(stderr, "Error while writing the Info Header\n");
		fclose(p_file);
		return 1;
	}

	/* Write to offset */
	while (ftell(p_file) < (long)p_file_header->offset) {
		e = fputc(0, p_file);
		if (e != 0) {
			fprintf(stderr, "Error while writing to offset\n");
			fclose(p_file);
			return 1;
		}
	}

	/* Write the compressed data */
	w = p_info_header->width;
	h = p_info_header->height;

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			if (i == 0 || i == h - 1 || j == 0 || j == w - 1 ||
			    (i > 0 && !is_similar(p_bitmap->pixels[i][j],
			    p_bitmap->pixels[i - 1][j], 0)) ||
			    (j > 0 && !is_similar(p_bitmap->pixels[i][j],
			    p_bitmap->pixels[i][j - 1], 0)) ||
			    (i + 1 < h && !is_similar(p_bitmap->pixels[i][j],
			    p_bitmap->pixels[i + 1][j], 0)) ||
			    (j + 1 < w && !is_similar(p_bitmap->pixels[i][j],
			    p_bitmap->pixels[i][j + 1], 0)))
			{
				compressed_point_t pt;
				pt.y = i + 1;
				pt.x = j + 1;
				pt.r = p_bitmap->pixels[i][j].r;
				pt.g = p_bitmap->pixels[i][j].g;
				pt.b = p_bitmap->pixels[i][j].b;
				if (fwrite(&pt, sizeof(pt), 1, p_file) != 1) {
					fprintf(stderr, "Error writing\n");
					fclose(p_file);
					return 1;
				}
			}
		}
	}

	fclose(p_file);
	return 0;
}

