#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <time.h>

#define WIDTH	800
#define HEIGHT	600
#define SEEDS_COUNT	20


#define SEED_RADIUS 5
#define SEED_COLOR	0x24237A

#define PALETTE_SIZE 10
#define ROSEWATER	0xF4DBD6
#define PINK		0xF5BDE6
#define MAUVE		0xC6A0F6
#define RED			0xED8796
#define MAROON		0xEE99A0
#define PEACH		0xF5A97F
#define YELLOW		0xEED49F
#define GREEN		0xA6DA95
#define BLUE		0x8AADF4
#define LAVENDER	0xB7BDF8

#define OUTPUT_FILE_PATH	"voronoi.ppm"

typedef uint32_t Color32;

typedef struct {
	int x, y;
} Point;

static Color32 image[HEIGHT][WIDTH];
static Color32 palette[PALETTE_SIZE] = {
	ROSEWATER,
	PINK,
	MAUVE,
	RED,
	MAROON,
	PEACH,
	YELLOW,
	GREEN,
	BLUE,
	LAVENDER,
};

static Point seeds[SEEDS_COUNT];

void fill_image(Color32 color)
{
	for (size_t y = 0; y < HEIGHT; y++)	{
		for (size_t x = 0; x < WIDTH; x++) {
			image[y][x] = color;	
		}
	}
}

int sqr_dist(int x1, int y1, int x2, int y2)
{
	int dx = x1 - x2;
	int dy = y1 - y2;
	return dx*dx + dy*dy;
}

void fill_circle(int cx, int cy, int radius, Color32 color)
{
	Point p0, p1;
	p0.x = cx - radius;
	p0.y = cy - radius;
	p1.x = cx + radius;
	p1.y = cy + radius;
	for (int x = p0.x; x < p1.x; x++) {
		if (x < 0 || x >= WIDTH)
			continue;
		for (int y = p0.y; y < p1.y; y++) {
			if (y < 0 || y >= HEIGHT)
				continue ;
			if (sqr_dist(cx, cy, x, y) <= radius*radius) {
				image[y][x] = color;
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
	srand(time(NULL));
	for (size_t i = 0; i < SEEDS_COUNT; i++) {
		seeds[i].x = rand() % WIDTH;
		seeds[i].y = rand() % HEIGHT;
	}
}

void render_seeds(void)
{
	for (size_t i = 0; i < SEEDS_COUNT; i++) {
		fill_circle(seeds[i].x, seeds[i].y, SEED_RADIUS, SEED_COLOR);
	}
}

void render_voronoi(void)
{
	for (size_t y = 0; y < HEIGHT; y++) {
		for (size_t x = 0; x < WIDTH; x++) {
			int j = 0;
			for (size_t i = 1; i < SEEDS_COUNT; i++) {
				if (sqr_dist(seeds[i].x, seeds[i].y, x, y) < sqr_dist(seeds[j].x, seeds[j].y, x, y)) {
					j = i;
				}
			}
			image[y][x] = palette[j % PALETTE_SIZE];
		}
	}
}

int main(void)
{
	generate_random_seeds();
	render_voronoi();
	render_seeds();
	save_image_as_ppm(OUTPUT_FILE_PATH);
	return (0);
}
