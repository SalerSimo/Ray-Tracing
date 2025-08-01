#ifndef COLOR_H
#define COLOR_H

#include<stdint.h>

#define COLOR_BLACK    Color_new(0x000000)
#define COLOR_WHITE    Color_new(0xFFFFFF)
#define COLOR_RED      Color_new(0xFF0000)
#define COLOR_GREEN    Color_new(0x008000)
#define COLOR_BLUE     Color_new(0x0000FF)
#define COLOR_YELLOW   Color_new(0xFFFF00)
#define COLOR_CYAN     Color_new(0x00FFFF)
#define COLOR_MAGENTA  Color_new(0xFF00FF)
#define COLOR_GRAY     Color_new(0x808080)
#define COLOR_ORANGE   Color_new(0xFFA500)
#define COLOR_PURPLE   Color_new(0x800080)
#define COLOR_BROWN    Color_new(0xA52A2A)
#define COLOR_PINK     Color_new(0xFFC0CB)
#define COLOR_LIME     Color_new(0x00FF00)

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
 * @brief Create a new Color instance from RGB components.
 * 
 * @param r The red component, in the range [0, 1].
 * @param g The green component, in the range [0, 1].
 * @param b The blue component, in the range [0, 1].
 * 
 * @return A Color struct initialized with the specified color.
 */
Color Color_fromRGB(double r, double g, double b);

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