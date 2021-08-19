/*
 * plik ma na celu prezentację podstawowych sposobu czytania plików png z
 * wykorzystaniem biblioteki libpng
 * 
 * kompilacja: gcc -lpng read_png.c
 * (wymagany pakiet: libpng-dev)
 */

#include <png.h>

#define E_FOPEN       -1
#define E_READ_STRUCT -2
#define E_INFO_STRUCT -3
#define E_PNG_TYPE    -4

int read_png(char *file_name) {
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	int col, row, row_cnt, skip_cnt, row_len;
	FILE *fp;
	
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		return E_FOPEN;
	}
	
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return E_READ_STRUCT;
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL || setjmp(png_jmpbuf(png_ptr))) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return E_INFO_STRUCT;
	}
	
	png_init_io(png_ptr, fp);
	
	png_read_png(
		png_ptr, info_ptr,
		PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL
	);
	
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	
	// w zależności od typu pliku png ustalamy co ile bajtów jest interesująca nas informacja
	if (color_type == PNG_COLOR_TYPE_GRAY) {
		puts("GRAY");
		skip_cnt = 1;
	} else if (color_type == PNG_COLOR_TYPE_RGB) {
		puts("RGB");
		skip_cnt = 3;
	} else {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return E_PNG_TYPE;
	}
	row_len = width * skip_cnt;
	
	// czytamy kolejne wiersze (jedno-pixelowe linie) z pliku
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
	for (row=0; row<height; row++) {
		row_cnt = row * row_len;
		for (col=0; col<row_len; col=col+skip_cnt) {
			if (row_pointers[row][col] < 50)
				printf("*");
			else
				printf(" ");
		}
		printf("\n");
	}
	
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	return 0;
}

int main() {
	printf("Return %d\n", read_png("test.png"));
}
