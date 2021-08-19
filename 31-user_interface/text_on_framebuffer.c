/* program prezentuje wykorzystanie linuxowego framebuffer'a i biblioteki FreeType
 *
 * kompilacja:    gcc -I /usr/include/freetype2 -lfreetype text_on_framebuffer.c -o fb_print
 * (wymagany pakiet: libfreetype6-dev)
 * 
 * uruchomienie (na terminalu tekstowym działającym w trybie framebruffera):
 *                ./fb_print  /usr/share/fonts/truetype/dejavu/DejaVuSans-BoldOblique.ttf  "Ala ma psa"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/fb.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define DEV_FILE "/dev/fb0"

int    fbFD;
char*  fbBUF;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int    fbFrameSize, fbPixelSize;

inline static void setARGB(int idx, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
	char* point = fbBUF + idx;
	*(point)   = b;
	*(point+1) = g;
	*(point+2) = r;
	*(point+3) = a; // 0 == transparent
}

inline static void drawRectangle(int startX, int startY, int stopX, int stopY) {
	int x, y, lineOffet, pixelOffset;
	
	if (stopX > vinfo.xres || stopX == 0)
		stopX = vinfo.xres;
	if (stopY > vinfo.yres || stopY == 0)
		stopX = vinfo.yres;
	if (startX < 0)
		startX = 0;
	if (startY < 0)
		startY = 0;
	
	lineOffet = (vinfo.yoffset + startY) * finfo.line_length;
	for (y = startY; y < stopY; y++) {
		pixelOffset = lineOffet + (vinfo.xoffset + startX) * fbPixelSize;;
		for (x = startX; x < stopX; x++) {
			setARGB(pixelOffset, 150, 200, 200, 0);
			pixelOffset += fbPixelSize;
		}
		lineOffet += finfo.line_length;
	}
}

int printCharBitmap(FT_GlyphSlot slot, int startX) {
	int fX, fY, fR, xOffset, lineOffet, pixelOffset;
	
	xOffset   = startX + slot->bitmap_left;
	lineOffet = (vinfo.yoffset + 200 - slot->bitmap_top) * finfo.line_length;
	
	for(fY = 0; fY < slot->bitmap.rows; fY++) {
		pixelOffset = lineOffet + (vinfo.xoffset + xOffset) * fbPixelSize;
		fR = fY * slot->bitmap.width;
		
		for (fX = 0; fX < slot->bitmap.width; fX++) {
			if (pixelOffset > fbFrameSize)
				return 0;
			setARGB(pixelOffset, 255, 255, slot->bitmap.buffer[fR + fX], 0);
			pixelOffset += fbPixelSize;
		}
		lineOffet += finfo.line_length;
	}
	
	startX += slot->advance.x >> 6;
	return startX;
}

void renderText(char* text, FT_Face ftFace) {
	int           i, maxChar, xPos = 100;
	
	maxChar = strlen( text );
	for (i=0; i<maxChar; i++) {
		if ( FT_Load_Char(ftFace, text[i], FT_LOAD_RENDER) ) {
			puts("Error in FT_Load_Char");
			continue;
		}
		xPos = printCharBitmap(ftFace->glyph, xPos);
		if (xPos == 0) break;
	}
}

int main(int argc, char** argv) {
	char*         fontName;
	FT_Library    ftLib;
	FT_Face       ftFace;
	
	fontName = argv[1];
	
	// FreeType open font
	if ( FT_Init_FreeType(&ftLib) )
		puts("Error in FT_Init_FreeType");
	
	if ( FT_New_Face(ftLib, fontName, 0, &ftFace) )
		puts("Error in FT_New_Face");
	
	if ( FT_Set_Char_Size( ftFace, 0, 26 * 64, 128, 128 ) )
		puts("Error in FT_Set_Char_Size");
	
	
	// open device and get info
	fbFD = open(DEV_FILE, O_RDWR);
	if (fbFD == -1) {
		perror("Open framebuffer device: " DEV_FILE " error");
		exit(1);
	}
	
	if (ioctl(fbFD, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("ioctl FBIOGET_FSCREENINFO error");
		exit(2);
	}
	
	if (ioctl(fbFD, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("ioctl FBIOGET_VSCREENINFO error");
		exit(3);
	}
	
	if (vinfo.bits_per_pixel != 32) {
		puts("We only support 32 bit per pixel");
		exit(4);
	}
	
	// open memory buffer
	fbPixelSize = vinfo.bits_per_pixel / 8;
	fbFrameSize = vinfo.xres * vinfo.yres * fbPixelSize;
	fbBUF=(char*)mmap(0, fbFrameSize, PROT_WRITE, MAP_SHARED, fbFD, 0);
	
	// write to frame buffer
	memset(fbBUF, 0, fbFrameSize);
	renderText(argv[2], ftFace);

	FT_Done_Face(ftFace);
	FT_Done_FreeType(ftLib);
	
	// close
	munmap(fbBUF, fbFrameSize);
	close(fbFD);
	return 0;
}

