#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define WIDTH	800
#define HEIGHT	600
#define SEEDS_COUNT	10

#define COLOR_RED	0xFF0000
#define COLOR_GREEN	0x00FF00
#define COLOR_BLUE	0x0000FF
#define COLOR_BG	0x181818

#define SEED_RADIUS 5
#define SEED_COLOR 0xFFFFFF

#define OUTPUT_FILE_PATH	"output.ppm"

typedef uint32_t Color32;

typedef struct {
	int x, y;
} Point;

static Color32 image[HEIGHT][WIDTH];
static Point seeds[SEEDS_COUNT];

void fill_image(Color32 color)
{
	for (size_t y = 0; y < HEIGHT; y++)	{
		for (size_t x = 0; x < WIDTH; x++) {
			image[y][x] = color;	
		}
	}
}

void fill_circle(int cx, int cy, int radius, Color32 color)
{
	Point p0, p1;
	p0.x = cx - radius;
	p0.y = cy - radius;
	p1.x = cx + radius;
	p1.y = cy + radius;
	for (int x = p0.x; x < p1.x; x++) {
		if (0 <= x && x < WIDTH) {
			for (int y = p0.y; y < p1.y; y++) {
				if (0 <= y && y < HEIGHT) {
					int dx = cx - x;
					int dy = cy - y;
					if (dx*dx + dy*dy <= radius*radius) {
						image[y][x] = color;
					}
				}
			}

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

void generate_random_seeds(void)
{
	for (size_t i = 0; i < SEEDS_COUNT; i++) {
		seeds[i].x = rand() % WIDTH;
		seeds[i].y = rand() % HEIGHT;
	}
}

int main(void)
{
	fill_image(COLOR_BG);
	generate_random_seeds();
	for (size_t i = 0; i < SEEDS_COUNT; i++) {
		fill_circle(seeds[i].x, seeds[i].y, SEED_RADIUS, SEED_COLOR);
	}
	save_image_as_ppm(OUTPUT_FILE_PATH);
	return (0);
}
