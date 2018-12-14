#include "PPM.h"
#include <iomanip>

_ppm::_ppm()
{
    loaded = false;
}

_ppm::~_ppm()
{
    if(loaded)
        clear();
}

/* save_ppm - Saves the image as PPM format to the file stream.
* Arguments: filename to save into, x1, y1 to x2, y2 area to save.
* Returns: 0 on success, -1 on error
*/



int _ppm::save_ppm(const char* filename)
{
    if(!loaded)
        return -1;

    FILE * fi;
    int x, y, rgb;

    int x1 = 0;
    int y1 = 0;
    int x2 = width;
    int y2 = height;

    if((fi = fopen(filename,"w+b"))==0)
        return -2;

    if (x1 < 0 || x1 > width || y1 < 0 || y1 > height ||
        x2 < 0 || x2 > width || y2 < 0 || y2 > height)
        return -1;


/* Dump PPM header: */

    fprintf(fi, "P6\n %d %d\n%d\n", x2 - x1, y2 - y1, depth);


    /* Dump data: */

    for (y = y1; y < y2; y++)
    for (x = x1; x < x2; x++)
    //for (rgb = 0; rgb < 3; rgb++)

    fputc(data[y*width+x].r, fi);
    fputc(data[y*width+x].g, fi);
    fputc(data[y*width+x].b, fi);

    return 0;
}

/* load_ppm - Loads a PPM format image.
* Arguments: filename to load from,x and y locations to start 
* loading into, and opacity with which to add the data to the 
* existing image (default is 255 - full opacity).
* Returns: 0 on success, -1 on error
*/

bool _ppm::load_ppm(const char* filename, int xx, int yy)
{
    if(loaded)
        clear();

    FILE * fi;
    char temp[10240];
    int x, y, rgb, m; 

    if((fi=fopen(filename,"rb"))==0)
        return false;

    /* Load PPM header: */

    fscanf(fi, "%s %d %d %d", &temp, &width, &height, &m); 

    depth = m;

    r_height = round_up(height, 8);
    r_width = round_up(width, 8);

    /* Create storage array for data */

    data = new RGBColour[width*height];
    ydata = new YBRColour[width*height];

    /* Get some data from the ppm */

    fgetc(fi);

    /* Load real data: */

    int r, g, b, index;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            index = y * width + x;
            data[index].r = fgetc(fi);
            data[index].g = fgetc(fi);
            data[index].b = fgetc(fi);
            ydata[index] = RGBToYCBCR(data[index]);
        }
    }

    loaded = true;
    
    return true;

}

double** _ppm::get_image_ptr(int block_size)
{
    //int x_pad = 
    double** img = create_array(r_height, r_width);

    for (int y = 0; y < r_height; y++)
    {
        for (int x = 0; x < r_width; x++)
        {
            if(y >= height)
                img[y][x] = img[height-1][x]; //(double)data[(height-1) * width + x].r;
            else if (x >= width)
                img[y][x] = img[y][width-1];//(double)data[y * width + (width-1)].r;
            else
                img[y][x] = (double)data[y * width + x].r;
        }
    }
    return img;
}

int _ppm::set_pixel(int x, int y, RGBColour c)
{
    if(!loaded)
        return -1;

    if (x < 0 || x > width || y < 0 || y > height)
        return -1;

//if(value < 0 || value > 255)
//	return -1;

    data[y*width+x] = c;

    return 0;
}

int _ppm::get_pixel(int x, int y, RGBColour *c)
{
    if(!loaded)
        return -1;

    if (x < 0 || x > width || y < 0 || y > height)
        return -1;

    c = &data[y * width + x];

    return 0;
}

void _ppm::clear()
{
    delete[] data;
    height = 0;
    width = 0;
    r_height = 0;
    r_width = 0;
    depth = 0;
    loaded = false;
}


void _ppm::print_pixels()
{
    YBRColour xd;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            xd  = ydata[y * width + x];
        }
    }
}

bool _ppm::isLoaded()
{
    return loaded;
}