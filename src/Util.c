#include "Util.h"

YBRColour RGBToYCBCR(RGBColour c)
{
	int y = 16 + (66*c.r/256) + (129*c.g/256) + (25*c.b/256);
	int cb = 128 - (38*c.r/256) - (74*c.g/256) + (112*c.b/256);
	int cr = 128 + (112*c.r/256) - (94*c.g/256) - (18*c.b/256);
	
	return {y, cb, cr};
}

//math functions to round value to multipe of 8
int round_up(int n, int m) {
    return n >= 0 ? ((n + m - 1) / m) * m : (n / m) * m;
}

//get min value
double min(double a, double b)
{
    if (a < b)
        return a;
    return b;
}

//get max value
double max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

//Zigzag scan translated from:
//https://medium.com/100-days-of-algorithms/day-63-zig-zag-51a41127f31
void zigzag_to_matrix(int* x, int* y, int k, int n)
{
    int i, j;

    if (k >= n * (n + 1) / 2) {
        int a, b;
        zigzag_to_matrix(&a, &b, n * n - 1 - k, n);
        *x = n - 1 - a; *y = n - 1 - b;
        return; 
    }

    i = (int)((sqrt(1 + 8 * k) - 1) / 2);
    j = k - i * (i + 1) / 2;

    if (i & 1){
        *x = j; *y = i - j;
    }else{
        *x = i - j; *y = j;
    }
}

//Zigzag scan translated from:
//https://medium.com/100-days-of-algorithms/day-63-zig-zag-51a41127f31
int matrix_to_zigzag(int i, int j, int n)
{
    if (i + j >= n)
        return n * n - 1 - matrix_to_zigzag(n-1-i, n - 1 - j, n);

    int k = (i + j) * (i + j + 1) / 2;

     if ((i+j) & 1){
        return k + i;
    }else{
        return k + j;
    }
   // return ((i + j) & 1) ? k + j : k + i;

}

//divide values in a by values in b
double** quantise_matrix(double** a, double** b, int size)
{
	double** to_return = create_array(size, size);
    double qt;

	for(int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
        {
            qt = a[y][x] / b[y][x];
            if(qt > 0)
    			to_return[y][x] = floor(qt);
            else
                to_return[y][x] = ceil(qt);
        }
    }

	return to_return;
}

//multiply values in array a by b
double** dequantise_matrix(double** a, double** b, int size)
{
    double qt;

	for(int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
        {
            a[y][x] = a[y][x] * b[y][x];
        }
    }

	return a;
}

/*
    ARRAY CREATION FUNCTIONS 
    create and free 2D arrays
    create and free 3D arrays
*/
void free_array(double** a, int n)
{
	for (int i = 0; i < n; i++)
		free(a[i]);

	free(a);
}
double** create_array(int h, int w)
{
    double **to_return = (double **)malloc(h * sizeof(double *)); 
    for (int i=0; i<h; i++) 
         to_return[i] = (double *)malloc(w * sizeof(double)); 	
    return to_return;
}

void free_3array(double*** a, int c, int h)
{
    for (int j=0; j<c; j++) 
	{
		for (int i = 0; i < h; i++)
	    	free(a[j][i]);	
		free(a[j]);
	}
	free(a);
}

double*** create_3array(int c, int h, int w)
{
    double ***to_return = (double ***)malloc(c * sizeof(double **)); 
    for (int j=0; j<c; j++) 
	{
		to_return[j] = (double **)malloc(h * sizeof(double*));
		for (int i = 0; i < h; i++)
	    	to_return[j][i] = (double *)malloc(w * sizeof(double)); 	
	}
	return to_return;
}


//copy subsquare from source data defined by x, y, h and w
double** copy_square(double** source, int x, int y, int h, int w)
{
    double** to_return = create_array(h, w);
    for (int i = 0; i < w; i++)
        memcpy((void*)&to_return[i][0], (void*)&source[i+y][x], sizeof(double) * w);

    return to_return;
}

//combine stack of arrays into a single image
double** combine_arrays(double*** images, int height, int width)
{
	double** to_return = create_array(height * 8, width * 8);
	
	int row;
	int col;


	for (int k = 0; k < height*width; k++)
	{	

		row = floor(k / width);
		col = k % width;

		for (int j = 0; j < 8; j++)
		{
			for (int i = 0; i < 8; i++)
			{
				to_return[row*8+j][col*8+i] = images[k][j][i];
			}
			
		}
	
	}
	return to_return;
}


/*
    save compressed image to given filename
*/
void save_compressed(const char *file_name, double*** image_data, double** quantise, int blocks, int b_width, int height, int width)
{
	//set up things for saving
	FILE *fp = fopen(file_name, "wb");
	char magic_numbers[] = {'D', 'C', 'T'};

	char sB[2];	//short buffer
	char bB[1]; //byte buffer

    short s;	//var for writing shorts to file
	char b;		//var for writing bytes to file

	//write magic number
	fwrite(magic_numbers, sizeof(char), sizeof(magic_numbers), fp);
	//write block count, width and image height and width
	fwrite((const void*)& blocks, sizeof(short), 1, fp);
   	fwrite((const void*)& b_width, sizeof(short), 1, fp);
	fwrite((const void*)& height, sizeof(short), 1, fp);
	fwrite((const void*)& width, sizeof(short), 1, fp);
	
	//write quantise table to file
	for(int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            s = quantise[y][x];
            fwrite((const void*)&s, sizeof(short), 1, fp);
        }
    }

	//for each block
	for( int k = 0; k < blocks; k++)
    {
		//allocate space for 
        double zz[64];
        int x, y;

        for (int i = 0; i < 8*8; i++)
        {
			//get zz value from matrix and build up zz array
            zigzag_to_matrix(&x, &y, i, 8);
            zz[i] = image_data[k][y][x];
        } 

		//get run length encoding for zigzag scan
        RLEData dd = build_rle(zz, 8*8);

		fputc(dd.used, fp);
		//for each 
        for (int i = 0; i < dd.used; i++)
        {
			s = dd.array[i].value; b = (char)dd.array[i].runs;
            fwrite((const void*)&s, sizeof(short), 1, fp);
            fwrite((const void*)&b, sizeof(char), 1, fp);
        }
        free_rle(&dd);
    }

    fclose(fp);

}

//load compressed image from file name
double*** load_compressed(char *file_name, short* h, short* w, short* bl, short* b_w)
{
    //buffers to read into
	char sB[2];	//short buffer
	char bB[1]; //byte buffer

    //image data
    RLEData rle;
    short width, height, b_width, blocks;
    
    //quantisation table
    double** q = create_array(8, 8);

	//open given file in read mode
	FILE *fp = fopen(file_name, "rb");

    //read and discard magic numbers
    fgetc(fp);fgetc(fp);fgetc(fp);

    short s;	//var for reading shorts into
	char b;		//var for reading bytes

	//read block count, width and image size
    fread(sB, sizeof(short), 1, fp); blocks = *((short*) sB);
    fread(sB, sizeof(short), 1, fp); b_width = *((short*) sB);
    fread(sB, sizeof(short), 1, fp); height = *((short*) sB);
    fread(sB, sizeof(short), 1, fp); width = *((short*) sB);
	
	*bl = blocks;
	*b_w = b_width;
	*h = height;
	*w = width;


    //create array to load image into
    double*** img = create_3array(blocks, 8, 8);

	//read quantise table to file
	for(int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {   //read short from file and add to array
            fread(sB, sizeof(short), 1, fp); s = *((short*) sB);
            q[y][x] = s;
        }
    }

	//for each block
	for( int k = 0; k < blocks; k++)
    {
        //init RLE array
        init_rle(&rle, 5);

        //read length of RLE data
        int c = fgetc(fp);

        //holder variables
        short val; char len;

        //for each RLE entry
        for (int i = 0; i < c; i++)
        {   //read the value as a short and the run as a byte
			fread(sB, sizeof(short), 1, fp); val = *((short*) sB);
            fread(bB, sizeof(char), 1, fp); len = *((char*) bB);
            insert_rle(&rle, (RLE){(short)val, len});
        } 

        //x y holders and array to hold rle data
        int x, y;
        double* d = rle_to_array(&rle);
		double** temp = create_array(8, 8);
        for (int i = 0; i < 64; i++)
        {   //convert array position to matrix position and add to image
            zigzag_to_matrix(&x, &y, i, 8);
            temp[y][x] = d[i];
        }
		temp = dequantise_matrix(temp, (double**)q, 8);
		for (int x = 0; x < 8; x++)
			for (int y = 0; y < 8; y++)
				img[k][y][x] = temp[y][x];

		free_rle(&rle);
		free(d);
		free_array(temp, 8);
    }
 
	free_array(q, 8);
    fclose(fp);

	return img;

}