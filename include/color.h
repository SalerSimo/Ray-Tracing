#ifndef COLOR_H
#define COLOR_H

#include<stdint.h>

#define COLOR_WHITE Color_new(0xffffff)
#define COLOR_BLACK Color_new(0x000000)
#define COLOR_RED Color_new(0xff0000)
#define COLOR_GREEN Color_new(0x00ff00)
#define COLOR_BLUE Color_new(0x0000ff)
#define COlOR_YELLOW Color_new(0xffff00)

typedef struct{
    uint32_t color;
}Color;

/**
 * @brief Creates a new Color instance from a 32-bit unsigned integer value.
 *
 * @param color A 32-bit unsigned integer representing the color value.
 * @return A Color struct initialized with the specified color.
 */
Color Color_new(uint32_t color);

/**
 * Scales the brightness of a Color by a given factor.
 *
 * @param c The original Color to scale.
 * @param factor A double value between 0 and 1 used to scale the color brightness.
 * @return A new Color with RGB components scaled by the factor.
 */
Color Color_scale(Color c, double factor);

/**
 * Blends two Colors based on a parameter `t`, using the logic:
 * 
 * result = (1 - `t`)*`c1` + `t`*`c2`
 *
 * @param c1 The first Color.
 * @param c2 The second Color.
 * @param t  The interpolation factor (clamped between 0 and 1).
 * @return A new Color representing the blend of c1 and c2.
 */
Color Color_blend(Color c1, Color c2, double t);

/**
 * Adds the RGB components of two Colors.
 *
 * @param c1 The first Color.
 * @param c2 The second Color.
 * @return A new Color whose RGB components are the sum of c1 and c2 components.
 */
Color Color_add(Color c1, Color c2);

/**
 * Extracts the raw 32-bit integer color value from a Color struct.
 *
 * @param c The Color instance.
 * @return The 32-bit unsigned integer representing the color.
 */
uint32_t Color_extract(Color c);

Color Color_average(Color* colors, int n);

Color Color_multiply(Color c1, Color c2);

#endif //COLOR_H