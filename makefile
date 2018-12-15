#OBJS specifies which files to compile as part of the project
OBJS = src/main.c src/DCT/DCT.h src/UI/tCompress.h src/UI/tDecompress.h src/Util.h src/Util.c src/PPM.h src/PPM.cpp src/RLE/RLE.h src/RLE/RLE.c
#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -ggdb -w

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = `pkg-config --libs --cflags gtk+-3.0` -lm

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = DCT

run: all
	./DCT

dbg: all
	gdb JPEG

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm DCT
