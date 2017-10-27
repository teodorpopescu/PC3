#include <stdio.h>
#include <string.h>

#include "bmplib.h"
#include "stack.h"

#define MAX_FILENAME 1024

#define INPUT_FILENAME "input.txt"

#define GRAYSCALE_NAME_SUFFIX "_black_white"
#define FILTER1_NAME_SUFFIX "_f1"
#define FILTER2_NAME_SUFFIX "_f2"
#define FILTER3_NAME_SUFFIX "_f3"

void split_file_name(char name[], char extension[], const char file_name[])
{
	int i, stride;
	for (i = 0; file_name[i] != '\0' && file_name[i] != '.'; ++i) {
		name[i] = file_name[i];
	}
	name[i] = '\0';
	stride = i;
	for (; file_name[i] != '\0'; ++i) {
		extension[i - stride] = file_name[i];
	}
	extension[i - stride] = '\0';
}

int main(void)
{
	FILE *p_file;

	char file_name[MAX_FILENAME], tmp_file_name[MAX_FILENAME];
	char compression_file_name[MAX_FILENAME];
	char name[MAX_FILENAME], extension[MAX_FILENAME];

	int filter1[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}};
	int filter2[3][3] = {
		{0, 1, 0},
		{1, -4, 1},
		{0, 1, 0}};
	int filter3[3][3] = {
		{1, 0, -1},
		{0, 0, 0},
		{-1, 0, 1}};
	int threshold;

	bmp_file_header_t file_header;
	bmp_info_header_t info_header;
	bitmap_t bitmap, gray_bitmap, tmp_bitmap;
	bitmap.pixels = NULL;
	gray_bitmap.pixels = NULL;
	tmp_bitmap.pixels = NULL;

	int e;

	/* Read the input file */
	p_file = fopen(INPUT_FILENAME, "r");
	if (p_file == NULL) {
		fprintf(stderr, "Can't open the input file\n");
		goto exit_failure;
	}
	if (fgets(file_name, MAX_FILENAME, p_file) == NULL) {
		fprintf(stderr, "Can't read first line of the input file\n");
		fclose(p_file);
		goto exit_failure;
	}
	if (fscanf(p_file, "%d", &threshold) != 1) {
		fprintf(stderr, "Can't read second line of the input file\n");
		fclose(p_file);
		goto exit_failure;
	}
	fgetc(p_file);
	if (fgets(compression_file_name, MAX_FILENAME, p_file) == NULL) {
		fprintf(stderr, "Can't read third line of the input file\n");
		fclose(p_file);
		goto exit_failure;
	}
	file_name[strlen(file_name) - 1] = '\0';
	compression_file_name[strlen(compression_file_name) - 1] = '\0';
	split_file_name(name, extension, file_name);
	fclose(p_file);

	/* Read the bmp file and initialize bitmaps*/
	e = read_bmp(file_name, &file_header, &info_header, &bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while reading file\n");
		goto exit_failure;
	}
	e = initialize_bitmap(&tmp_bitmap, bitmap.width, bitmap.height);
	if (e != 0) {
		fprintf(stderr, "Error initializing a bitmap\n");
		goto exit_failure;
	}
	e = initialize_bitmap(&gray_bitmap, bitmap.width, bitmap.height);
	if (e != 0) {
		fprintf(stderr, "Error initializing a bitmap\n");
		goto exit_failure;
	}

	/* Solve task 1 */
	grayscale_bitmap(&gray_bitmap, &bitmap);
	tmp_file_name[0] = '\0';
	strcat(tmp_file_name, name);
	strcat(tmp_file_name, GRAYSCALE_NAME_SUFFIX);
	strcat(tmp_file_name, extension);
	e = write_bmp(tmp_file_name, &file_header, &info_header, &gray_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task1\n");
		goto exit_failure;
	}

	/* Solve task 2 */
	filter_bitmap(&tmp_bitmap, &gray_bitmap, filter1);
	tmp_file_name[0] = '\0';
	strcat(tmp_file_name, name);
	strcat(tmp_file_name, FILTER1_NAME_SUFFIX);
	strcat(tmp_file_name, extension);
	e = write_bmp(tmp_file_name, &file_header, &info_header, &tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task2\n");
		goto exit_failure;
	}

	filter_bitmap(&tmp_bitmap, &gray_bitmap, filter2);
	tmp_file_name[0] = '\0';
	strcat(tmp_file_name, name);
	strcat(tmp_file_name, FILTER2_NAME_SUFFIX);
	strcat(tmp_file_name, extension);
	e = write_bmp(tmp_file_name, &file_header, &info_header, &tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task2\n");
		goto exit_failure;
	}

	filter_bitmap(&tmp_bitmap, &gray_bitmap, filter3);
	tmp_file_name[0] = '\0';
	strcat(tmp_file_name, name);
	strcat(tmp_file_name, FILTER3_NAME_SUFFIX);
	strcat(tmp_file_name, extension);
	e = write_bmp(tmp_file_name, &file_header, &info_header, &tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task2\n");
		goto exit_failure;
	}

	/* Solve task 3 */
	e = compress_bitmap(&tmp_bitmap, &bitmap, threshold);
	if (e != 0) {
		fprintf(stderr, "Error while compressing at task3\n");
		goto exit_failure;
	}
	e = write_compressed_bmp("compressed.bin", &file_header, &info_header,
		&tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task3\n");
		goto exit_failure;
	}

	/* Solve task 4 */
	clear_bitmap(&tmp_bitmap);
	e = read_compressed_bmp(compression_file_name, &file_header,
		&info_header, &tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while reading file at task4\n");
		goto exit_failure;
	}
	e = write_bmp("decompressed.bmp", &file_header, &info_header,
		&tmp_bitmap);
	if (e != 0) {
		fprintf(stderr, "Error while writing file at task4\n");
		goto exit_failure;
	}

	clear_bitmap(&bitmap);
	clear_bitmap(&tmp_bitmap);
	clear_bitmap(&gray_bitmap);
	return 0;

exit_failure:
	clear_bitmap(&bitmap);
	clear_bitmap(&tmp_bitmap);
	clear_bitmap(&gray_bitmap);
	return 1;
}

