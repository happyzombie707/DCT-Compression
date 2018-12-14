#ifndef RLE_H
#define RLE_H

#include <stdlib.h>


//struct to hold RLE data, value = pixel value, runs = count
typedef struct {
    double value;
    int runs;
}RLE;

//struct to hold an array of RLE data
//contains current size and max size
typedef struct {
    RLE* array;
    size_t used;
    size_t size;
}RLEData;


void init_rle(RLEData *a, size_t init_size);
void insert_rle(RLEData *a, RLE element);
void free_rle(RLEData *a);
RLEData build_rle(double* array, int len);
double* rle_to_array(RLEData *data);


#endif