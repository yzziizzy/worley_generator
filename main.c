// Public Domain

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "png.h"
#include "worley.h"



void write_worley(char* path, worley_noise* wn);


int main(int argc, char* argv[]) {
	
	srand(1337);
	
	if(argc < 2) {
		fprintf(stderr, "missing output filename\n");
		return 1;
	}
	
	worley_noise wn;
	wn.width = 512;
	wn.height = 512;
	wn.num_points = 1;
	wn.num_boxes = 2;
	wn.seed = 31337;

	worley_noise_boxes wn2;
	wn2.points_per_box = 1;
	wn2.n_boxes_w = 2;
	wn2.n_boxes_h = 2;
	wn2.box_sz = 256;
	wn2.seed = 31337;
	
	

	worley_noise_generate_block_infinite(&wn2, 0, 0);
//	worley_noise_generate_infinite(&wn, 0, 2);
//	worley_noise_generate_boxed_wrapped(&wn);
	//generate_pcg_test(&wn);
	
	wn.data = wn2.data;
	wn.height = wn2.height;
	wn.width = wn2.width;
	
	write_worley(argv[1], &wn);

	return 0;
}


void write_worley(char* path, worley_noise* wn) {
	long len = wn->width * wn->height;
	
	unsigned char* data8 = malloc(len);
	
	float scaler = 0;
	for(int i = 0; i < len; i++) {
		scaler = fmax(scaler, wn->data[i]);
	}
	
	scaler = 1.0 / scaler;
	
	for(int i = 0; i < len; i++) {
		data8[i] = 255 - (wn->data[i] * scaler * 255);
	}
	
	
	write_PNG(path, 1, data8, wn->width, wn->height);
	
	free(data8);
}




