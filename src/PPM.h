#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <string.h>
#include "Util.h"
#include <gtk/gtk.h>

#define RED   0
#define GREEN 1
#define BLUE  2



class _ppm{
public:
	_ppm();
	~_ppm();

	bool load_ppm(const char* filename, int xx = 0, int yy = 0);
	int save_ppm(const char* filename);


	double** get_image_ptr(int block_size);

	int get_image_height(){return height;};
	int get_image_width(){return width;};
	int get_image_depth(){return depth;};

	int get_pixel(int x, int y, RGBColour *c);
	int set_pixel(int x, int y, RGBColour c);
	
	void print_pixels();
	bool isLoaded();

protected:

	void clear();

	RGBColour* data;
	YBRColour* ydata;
	int width,height,depth,r_height,r_width;

private:

	bool loaded;
};

#endif