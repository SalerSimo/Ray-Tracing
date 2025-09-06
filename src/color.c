#include<stdlib.h>
#include<math.h>
#include"color.h"

Color Color_new(uint32_t color){
	Color c;
	c.color = color;
	return c;
}

Color Color_fromRGB(double r, double g, double b){
	Color c;

	int red   = (int)(r * 255.0 + 0.5);
	int green = (int)(g * 255.0 + 0.5);
	int blue  = (int)(b * 255.0 + 0.5);

	if (red   < 0) red = 0;   else if (red > 255) red = 255;
	if (green < 0) green = 0; else if (green > 255) green = 255;
	if (blue  < 0) blue = 0;  else if (blue > 255) blue = 255;

	c.color = ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
	return c;
}

Color Color_scale(Color c, double factor){
	if(factor <= 0)
		return COLOR_BLACK;
	else if(factor >= 1)
		return c;

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
	if(t <= 0)
		return c1;
	else if(t >= 1)
		return c2;
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
	int r = fmin(((colorA >> 16) & 0xFF) + ((colorB >> 16) & 0xFF), 255);
	int g = fmin(((colorA >> 8) & 0xFF) + ((colorB >> 8) & 0xFF), 255);
	int b = fmin((colorA & 0xFF) + (colorB & 0xFF), 255);
	
	Color result;
	result.color = (r << 16) | (g << 8) | b;
	return result;
}

uint32_t Color_extract(Color c){
	return c.color;
}

Color Color_average(Color *colors, int n) {
	if (n == 0) return COLOR_BLACK;
	if(n == 1) return colors[0];

	uint64_t sum_r = 0, sum_g = 0, sum_b = 0;

	for (size_t i = 0; i < n; ++i) {
		uint32_t color = Color_extract(colors[i]);

		uint8_t r = (color >> 16) & 0xFF;
		uint8_t g = (color >> 8)  & 0xFF;
		uint8_t b = color & 0xFF;

		sum_r += r;
		sum_g += g;
		sum_b += b;
	}

	uint8_t avg_r = sum_r / n;
	uint8_t avg_g = sum_g / n;
	uint8_t avg_b = sum_b / n;

	return Color_new((avg_r << 16) | (avg_g << 8) | avg_b);
}

Color Color_multiply(Color c1, Color c2){
	uint32_t colorA = c1.color;
	uint32_t colorB = c2.color;
	uint8_t rA = (colorA >> 16) & 0xFF;
	uint8_t gA = (colorA >> 8) & 0xFF;
	uint8_t bA = colorA & 0xFF;

	uint8_t rB = (colorB >> 16) & 0xFF;
	uint8_t gB = (colorB >> 8) & 0xFF;
	uint8_t bB = colorB & 0xFF;

	uint8_t r = rA * rB / 255;
	uint8_t g = gA * gB / 255;
	uint8_t b = bA * bB / 255;

	Color result;
	result.color = (r << 16) | (g << 8) | b;
	return result;
}

int Color_size(Color color){
	return sizeof(color.color);
}