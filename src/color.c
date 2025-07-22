#include<stdlib.h>
#include"color.h"

Color Color_new(uint32_t color){
    Color c;
    c.color = color;
    return c;
}

Color Color_scale(Color c, double factor){
    uint32_t color = c.color;
    if (factor < 0) factor = 0;
    if (factor > 1) factor = 1;

    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);

    c.color = (r << 16) | (g << 8) | b;
    return c;
}

Color Color_blend(Color c1, Color c2, double t){
    uint32_t colorA = c1.color;
    uint32_t colorB = c2.color;
    uint8_t rA = (colorA >> 16) & 0xFF;
    uint8_t gA = (colorA >> 8) & 0xFF;
    uint8_t bA = colorA & 0xFF;

    uint8_t rB = (colorB >> 16) & 0xFF;
    uint8_t gB = (colorB >> 8) & 0xFF;
    uint8_t bB = colorB & 0xFF;

    uint8_t r = (1 - t) * rA + t * rB;
    uint8_t g = (1 - t) * gA + t * gB;
    uint8_t b = (1 - t) * bA + t * bB;

    Color blend;
    blend.color = (r << 16) | (g << 8) | b;
    return blend;
}

Color Color_add(Color c1, Color c2){
    uint32_t colorA = c1.color;
    uint32_t colorB = c2.color;
    int r = __min(((colorA >> 16) & 0xFF) + ((colorB >> 16) & 0xFF), 255);
    int g = __min(((colorA >> 8) & 0xFF) + ((colorB >> 8) & 0xFF), 255);
    int b = __min((colorA & 0xFF) + (colorB & 0xFF), 255);
    
    Color result;
    result.color = (r << 16) | (g << 8) | b;
    return result;
}

uint32_t Color_extract(Color c){
    return c.color;
}