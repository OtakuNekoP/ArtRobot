#include "default.h"
#include "Color.h"

Color::Color() : color(0x000000FF)
{
}

Color::Color(uint32_t __color)
{
    color = __color;
}

Color::Color(const char *__color)
{
    if (__color[0] == '#')
        __color++;
    else if (__color[0] == '0' &&
             (__color[1] == 'x' ||
              __color[1] == 'X'))
        __color += 2;
    uint32_t intColor = 0x000000FF; // 黑不透明
    for (int i = 7; i >= 0 && *__color; i--)
    {
        intColor |= ((*__color >= '0' && *__color <= '9')
                         ? *__color - '0'
                         : (*__color >= 'A' && *__color <= 'F')
                               ? *__color - 'A' + 0xA
                               : (*__color >= 'a' && *__color <= 'f')
                                     ? *__color - 'a' + 0xA
                                     : 0)
                    << (4 * i);
        __color++;
    }
    color = intColor;
}

Color &Color::operator=(uint32_t __color)
{
    Color((uint32_t)__color);
    return *this;
}

Color &Color::operator=(const char *__color)
{
    Color((char *)__color);
    return *this;
}

Color &Color::operator=(const Color &__color)
{
    color = __color.color;
    return *this;
}

double Color::red()
{
    return (double)(color >> 24 & 0xff) / (double)0xff;
}

double Color::green()
{
    return (double)(color >> 16 & 0xff) / (double)0xff;
}

double Color::blue()
{
    return (double)(color >> 8 & 0xff) / (double)0xff;
}

double Color::alpha()
{
    return (double)(color & 0xff) / (double)0xff;
}