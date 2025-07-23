#ifndef COLOR_H
#define COLOR_H

#include<stdint.h>

#define COLOR_WHITE Color_new(0xffffff)
#define COLOR_BLACK Color_new(0x000000)
#define COLOR_RED Color_new(0xff0000)
#define COLOR_GREEN Color_new(0x00ff00)
#define COLOR_BLUE Color_new(0x0000ff)

typedef struct{
    uint32_t color;
}Color;

Color Color_new(uint32_t color);

Color Color_scale(Color c, double factor);

Color Color_blend(Color c1, Color c2, double t);

Color Color_add(Color c1, Color c2);

uint32_t Color_extract(Color c);

#endif //COLOR_H