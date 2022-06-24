#include "include/std.h"

#define bitmap_font_width 8
#define bitmap_font_height 16
#define bitmap_font_glyphs 256
#define bitmap_font_max (bitmap_font_height * bitmap_font_glyphs)

extern char bitmap_font[bitmap_font_max];

void setpixel(int x, int y, int colour) {
    // int bpp = 32 / 8;
    // int widthtbpp = 640 * bpp;
    // int index = x * bpp + y * widthtbpp;
    int index = x * (32 / 8) + y * (640 * (32 / 8));
    // puti(340 % 4);
    int *pixel = (int *)0x300 + index;
    *pixel = colour;
    puti(*pixel);
    // *pixel = colour;
}

void drawchar(char c, int x, int y, int hex_fg, int hex_bg) {
    int orig_x = x;
    char glypha = bitmap_font[c * bitmap_font_height];
    char *glyphb = &glypha;
   
    int i;
    int j;
   
    int __ret;

    for(i = 0; i < bitmap_font_height; i++) {
        for(j = bitmap_font_width - 1; j >= 0; j--) {
            __ret = (glyphb[i] >> j);
            setpixel(x++, y, __ret & 1 ? hex_fg : hex_bg);
        }
        y++;
        x = orig_x;
    }
}

int main(void) {
    // drawchar('H', 0, 0, 0x00FFFFFF, 0x00000000);
    setpixel(200, 100, 0x00FFFFFF);
    puti(*((int *)0x2F8));
    return 0;
}
