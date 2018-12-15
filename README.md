# DCT-Compression

Compress raw .ppm images with JPEG like discrete cosine transformation. Only handles greyscale at the moment.

Requires GTK3+
build with `make`

* main.c – initialise program, create initial UI elements.
* PPM.cpp & PPM.h – used to load ppm images
* Util.c & Util.h – contains useful functions including the zigzag scan, and array manipulation functions.

* UI/tCompress.h – compression tab, can be used to view the various transforms, compress and save the image.
* UI/tDecompress.h – used to decompress and display images.

* RLE/RLE.c & RLE/RLE.h – contains functions to build a dynamic array of RLE elements and the data structures.
* DCT/DCT.h – contains fDCT and iDCT implementations, also creation of quantisation tables.

