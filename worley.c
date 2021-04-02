// Public Domain

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "worley.h"



typedef struct v2l {
	long x,y; 
} v2l;

typedef struct v2f {
	float x,y; 
} v2f;



//32-bit output, 64-bit state, shift only ("PCG-XSH-RS")
uint32_t pcg_32o_64s(uint64_t* state) {
	uint64_t s = *state;
	s = (s * 314159265358979323ul) + 462643383279584626ul;
	
	uint64_t o = (s ^ (s >> 22ul)) >> (22ul + (s >> 61ul));
	
	*state = s;
	return (uint32_t)o;
}




static float frand(uint64_t* state) {
	return (double)pcg_32o_64s(state) / (double)UINT_MAX;
}


v2l randpt(long w, long h, uint64_t* state) {
	return (v2l){
		.x = frand(state) * w,
		.y = frand(state) * h,
	};
}

float distpt2(float x, float y, v2l pt) {
	float xx = x - pt.x;
	float yy = y - pt.y;
	return xx * xx + yy * yy;
}

float distpt2_wrap(float x, float y, v2l pt, float w, float h) {
	float xx = fabs(x - pt.x);
	float yy = fabs(y - pt.y);
	
	if(xx > w * 0.5) xx = w - xx;
	if(yy > h * 0.5) yy = h - yy;
	
	return xx * xx + yy * yy;
}


float lerp(float a, float b, float t) {
	return ((b - a) * t) + a;
}

float smoothstep(float a, float b, float t) {
	(b - a) * (3.0 - (t * 2.0)) * t * t + a;
}

float smootherstep(float a, float b, float t) {
	(b - a) * ((t * ((t * 6.0) - 15.0) + 10.0) * t * t * t) + a;
}



int worley_noise_generate_boxed_wrapped(worley_noise* wn) {

	long w = wn->width;
	long h = wn->height;
	long nb = wn->num_boxes;
	long nb2 = nb * nb; 
	
	
	
	wn->data = malloc(sizeof(*wn->data) * w * h);
	
	v2l* points = malloc(sizeof(*points) * nb2 * wn->num_points);
	
	// generate points inside boxes
	v2l box_sz = {
		.x = (float)w / (float)nb,
		.y = (float)h / (float)nb,
	};
	
	for(int y = 0; y < nb; y++)
	for(int x = 0; x < nb; x++) {
		int i = (y * nb + x) * wn->num_points;
		uint64_t rand_state = wn->seed;
		
		rand_state = rand_state * y * nb * 0xdeadbeef + x * 0xc00cf00d;
		
		for(int n = 0; n < wn->num_points; n++) {
			v2l p = randpt(box_sz.x, box_sz.y, &rand_state);
			points[i + n].x = p.x + box_sz.x * x;
			points[i + n].y = p.y + box_sz.y * y;
		}
	}
	
	
	for(int y = 0; y < h; y++)
	for(int x = 0; x < w; x++) {
		int i = y * w + x;
		
		
		float closest1 = 9999999999999;
		float closest2 = 999999999999;
		float closest3 = 99999999999;
		
		int xb = floor((float)x / box_sz.x);
		int yb = floor((float)y / box_sz.y);
		
		for(int yp = -1; yp <= 1; yp++)
		for(int xp = -1; xp <= 1; xp++) {
			int j = (((yb + yp + nb) % nb) * nb) + ((xb + xp + nb) % nb);
			j *= wn->num_points;
			
			for(int n = 0; n < wn->num_points; n++) {
				float d = distpt2_wrap(x, y, points[j + n], w, h);
				if(d < closest1) {
					closest3 = closest2;
					closest2 = closest1;
					closest1 = d;
				}
				else if(d < closest2) {
					closest3 = closest2;
					closest2 = d;
				}	
				else if(d < closest3) {
					closest3 = d;
				}	
			}	
			
		}
		
		wn->data[i] = sqrtf(closest1);
	}
	


}




int worley_noise_generate_random(worley_noise* wn) {
	
	long w = wn->width;
	long h = wn->height;
	uint64_t rand_state = wn->seed;
	
	wn->data = malloc(sizeof(*wn->data) * w * h);
	
	v2l* points = malloc(sizeof(*points) * wn->num_points);
	
	// generate points
	for(int i = 0; i < wn->num_points; i++) {
		points[i] = randpt(w, h, &rand_state);
	}
	
	for(int y = 0; y < h; y++)
	for(int x = 0; x < w; x++) {
		int i = y * w + x;
		
		// terrible algorithm
		float closest = 9999999999999;
		float closest2 = 999999999999;
		float closest3 = 99999999999;
		
		for(int i = 0; i < wn->num_points; i++) {
			float d = distpt2(x, y, points[i]);
			if(d < closest) {
				closest3 = closest2;
				closest2 = closest;
				closest = d;
			}
			else if(d < closest2) {
				closest3 = closest2;
				closest2 = d;
			}	
			else if(d < closest3) {
				closest3 = d;
			}	
			
		}
		
		wn->data[i] = sqrtf(closest);
	}
	
	
	free(points);
}




float* blend_bmp(float* a, float* b, v2l sz) {
	long len = sz.x * sz.y;
	float* o = malloc(sizeof(*o) * len);
	
	for(int i = 0; i < len; i++) {
		o[i] = a[i] * b[i];
	}
	
}




int generate_pcg_test(worley_noise* wn) {
	
	long w = wn->width;
	long h = wn->height;
	long np = wn->num_points;
	long np2 = np * np; 
	
	wn->data = malloc(sizeof(*wn->data) * w * h);
	
	uint64_t seed = 2718281828;
	
	for(int i = 0; i < w * h; i++) {
		wn->data[i] = (double)pcg_32o_64s(&seed) / (double)(UINT_MAX);
	}
	
	return 0;
}


