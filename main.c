#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define WIDTH	800
#define HEIGHT	600

#define COLOR_RED	0xFF0000
#define COLOR_GREEN	0x00FF00
#define COLOR_BLUE	0x0000FF

#define OUTPUT_FILE_PATH	"output.ppm"

typedef uint32_t Color32;

static Color32 image[HEIGHT][WIDTH];

void fill_image(Color32 color)
{
	for (size_t y = 0; y < HEIGHT; y++)	{
		for (size_t x = 0; x < WIDTH; x++) {
			image[y][x] = color;	
		}
	}
}

void save_image_as_ppm(const char *file_path)
{
	FILE *f = fopen(file_path, "w");
	if (f == NULL) {
		fprintf(stderr, "Error: could not write into file %s: %s\n", file_path, strerror(errno));
		exit(1);
	}
	fprintf(f, "P6\n%d %d 255\n", WIDTH, HEIGHT);
	for (size_t y = 0; y < HEIGHT; y++)	{
		for (size_t x = 0; x < WIDTH; x++) {
			uint32_t pixel = image[y][x];
			uint8_t bytes[3] = {
				(pixel & 0xFF0000) >> 16,
				(pixel & 0x00FF00) >> 8,
				pixel & 0x0000FF,
			};
			fwrite(bytes, sizeof(bytes), 1, f);
			assert(!ferror(f));
		}
	}
	int ret = fclose(f);
	assert(ret == 0);
}

int main(void)
{
	fill_image(COLOR_RED);
	save_image_as_ppm(OUTPUT_FILE_PATH);
	return (0);
}
