
#include "RLE.h"
#include <stdio.h>
#include <math.h>

/**
 * <Init dynamic RLE structure>
 *
 * @param  RLEData struct
 * @param  initial size of the array
 */
void init_rle(RLEData *a, size_t init_size)
{
    a->array = (RLE*)malloc(init_size * sizeof(RLE));
    a->used =0;
    a->size = init_size;
}

/**
 * <insert RLE element to RLE structure>
 *
 * @param  RLEData struct
 * @param  element to insert
 */
void insert_rle(RLEData *a, RLE element)
{
    if (a->used == a->size)
    {
        a->size++;
        a->array = (RLE *)realloc(a->array, a->size * sizeof(RLE));
    }
    a->array[a->used++] = element;
}

/**
 * <Free RLE array memory>
 *
 * @param  RLEData struct
 */
void free_rle(RLEData *a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

/**
 * <Build RLE array from an array>
 *
 * @param  pointer to data
 * @param  length of the array
 * @return RLEData structure
 */
RLEData build_rle(double* array, int len)
{
    RLEData rle;
    init_rle(&rle, 5);

    double current = array[0];
    int runs = 0;

    //RLE* runs = malloc(sizeof(RLE) * len);
    for(int i = 0; i < len; i++)
    {
        if(current == array[i])
        {
            runs++;
        }else
        {
            insert_rle(&rle, (RLE){current, runs});
            runs = 1; current = array[i];
        }

    }

    insert_rle(&rle, (RLE){current, runs});

    return rle;
}

/**
 * <Return pointer to array created from RLE data>
 *
 * @param  RLEData struct to convert
 * @return pointer to unwrapped data
 */
double* rle_to_array(RLEData *data)
{
    int size, runs, val, index;
    size = index = 0;

    for (int i = 0; i < data->used; i++)
        size += data->array[i].runs;

    double* to_return = (double*)malloc(size * sizeof(double));
    

    for (int i = 0; i < data->used; i++)
    {
        val = data->array[i].value;
        runs = data->array[i].runs;

        for(int j = 0; j < runs; j++)
        {   
            to_return[index] = val; index++;
        }
    }

    return to_return;

}