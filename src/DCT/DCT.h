#ifndef DCT_H
#define DCT_H

#include <math.h>
#include <stdlib.h>
#include "../Util.h"


//DCT implementation based on
//https://github.com/penberg/classpath/blob/master/gnu/javax/imageio/jpeg/DCT.java

//quantization matrix
double default_matrix[8][8] = {
    { 16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
   	{14, 13, 16, 24, 40, 57, 69, 56},
   	{14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99},
};

double **q_matrix;

double **low_matrix;
double **med_matrix;
double **high_matrix;
double **no_matrix;

//cosine matrix
double c[8][8];

//transformed cosine matrix
double cT[8][8];

void dct_init()
{
    //create quantization tables
    q_matrix = create_array(8, 8);
    high_matrix = create_array(8, 8);
    med_matrix = create_array(8, 8);
    low_matrix = create_array(8, 8);
    no_matrix = create_array(8, 8);

    for(int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            no_matrix[y][x] = 1;
            q_matrix[y][x] = default_matrix[y][x];
            high_matrix[y][x] = default_matrix[y][x];
            med_matrix[y][x] = default_matrix[y][x] / 4;
            low_matrix[y][x] = default_matrix[y][x] / 8;
        }

    //init DCT matricies
    for (int j = 0; j < 8; j++)
    {
        c[0][j] = 1.0 / sqrt(8.0);
        cT[j][0] = c[0][j];
    }

    for (int i = 1; i < 8; i++)

    {
        for (int j = 0; j < 8; j++)
        {
            double jj = j;
            double ii = i;

            c[i][j] = sqrt(2 / 8.0) * cos(((2.0 * jj + 1) * ii * M_PI) / (2.0 * 8.0));
            cT[j][i] = c[i][j];

        }
    }
}

//normalise DCT values to between -1024 and 1023
double normalise_dct(double n)
{
    return round((((n + 0) / 2047.0) * 255));
}

//get colour from normalised values
double dct_colour(double n)
{
    return round(((n - (-1024)) / (1023 - (-1024)) * 255));
}


//convert image data to pixbuf
GdkPixbuf *data_to_image(double **data, int height, int width)
{
    //guchar img[height * width * 3];

    cairo_surface_t *surf = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);
    cairo_t *cr = cairo_create(surf);
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double c = (double)data[y][x] / 255;
            cairo_set_source_rgb(cr, c, c, c);
            cairo_rectangle(cr, x, y, 1, 1);
            cairo_fill(cr);
        }
    }


    GdkPixbuf *to_return = gdk_pixbuf_get_from_surface(surf, 0, 0, width, height); //gdk_pixbuf_new_from_data(img, GDK_COLORSPACE_RGB, FALSE, 8, width, height, 33, NULL, NULL);
    
    free(surf);
    free(cr);

    return gdk_pixbuf_scale_simple(to_return, width*0.5, height*0.5, GDK_INTERP_NEAREST);

}

//convert DCT to image
GdkPixbuf *get_dct_image(double **data, int height, int width)
{
    double** temp = create_array(height, width);
    
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            temp[y][x] = dct_colour(data[y][x]);
        }
    }

    GdkPixbuf *to_return = data_to_image(temp, height, width);
    free_array(temp, height);
    return gdk_pixbuf_scale_simple(to_return, 256, 256, GDK_INTERP_NEAREST);

}

//
//  Forward DCT
//  
double ** fDCT(double** input)
{
    double** out = create_array(8, 8);

    double temp[8][8];
    double t;
    int i, j, k;

    for(i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            temp[i][j] = 0;
            for(k = 0; k < 8; k++)
                temp[i][j] += (((int) (input[i][k]) - 128) * cT[k][j]);
        }
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            t = 0;

            for (k = 0; k < 8; k++)
                t += c[i][k] * temp[k][j];

            out[i][j] = normalise_dct(round(t) * 8);
        }
    }
    return out;
}

//
//  Inverse DCT
//  
void iDCT(double** input)
{

    //double** out = create_array(8, 8);

    double temp[8][8];
    double t;
    int i, j, k;

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            temp[i][j] = 0;
            for (k = 0; k < 8; k++)
                temp[i][j] += input[i][k] * c[k][j];
        }
    }

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            t = 0;
            for (k = 0; k < 8; k++)
                t += cT[i][k] * temp[k][j];
            t += 128;
            if (t < 0)
                input[i][j] = 0;
            else if (t > 255)
                input[i][j] = 255;
            else
            input[i][j] = round(t);
        }
    }
}

#endif