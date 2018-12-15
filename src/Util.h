#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "RLE/RLE.h"
#include <gdk/gdk.h>



typedef struct RGBColour{
	int r, g, b;

	bool operator==(const RGBColour& c)
		{return this->r == c.r && this->g == c.g && this->b == c.b;}

	bool operator!=(const RGBColour& c)
		{	return !(*this == c);  }

} RGBColour;

typedef struct YBRColour {

	int y, cb, cr;

}YBRColour;

void zigzag_to_matrix(int* x, int* y, int k, int n);
int matrix_to_zigzag(int i, int j, int n);
int round_up(int n, int m);
double max(double a, double b);
double min(double a, double b);
YBRColour RGBToYCBCR(RGBColour c);
double** create_array(int h, int w);
double*** create_3array(int c, int h, int w);
double** copy_square(double** source, int x, int y, int h, int w);

double** combine_arrays(double*** images, int height, int width);
void free_array(double** a, int n);
void free_3array(double*** a, int c, int h);

double*** load_compressed(char *file_name, short* h, short* w, short* bl, short* b_w);
void save_compressed(const char *file_name, double*** image_data, double** quantise, int blocks, int b_width, int height, int width);

double** dequantise_matrix(double** a, double** b, int size);
double** quantise_matrix(double** a, double** b, int size);


#endif
