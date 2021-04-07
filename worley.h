// Public Domain

#include <stddef.h>





typedef struct worley_params {
	
	
} worley_params;



typedef struct worley_noise {
	int num_points;
	int num_boxes;
	size_t width, height;
	unsigned long seed;
	
	float* data;
		
} worley_noise;


typedef struct worley_noise_boxes {
	int points_per_box;
	int n_boxes_w;
	int n_boxes_h;
	int box_sz;
	unsigned long seed;
	
	size_t width, height; // calculated
	float* data;
	
} worley_noise_boxes;



void worley_noise_generate_block_infinite(worley_noise_boxes* wn, int block_x, int block_y);
void worley_noise_generate_random(worley_noise* wn);
void worley_noise_generate_boxed_wrapped(worley_noise* wn);
void worley_noise_generate_infinite(worley_noise* wn, int block_x, int block_y);


int generate_pcg_test(worley_noise* wn);


