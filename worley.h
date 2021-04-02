// Public Domain

#include <stddef.h>





typedef struct worley_params {
	
	
} worley_params;



typedef struct worley_noise {
	int num_points;
	int num_boxes;
	
	size_t width, height;
	float* data;
	
	unsigned long seed;
	
} worley_noise;



int worley_noise_generate_random(worley_noise* wn);
int worley_noise_generate_boxed_wrapped(worley_noise* wn);
int worley_noise_generate_boxed(worley_noise* wn);


int generate_pcg_test(worley_noise* wn);


