#ifndef DRAW_H
#define DRAW_H

#define MM2PT(mm) (mm*(1/25.4)*72)

#define A4_WIDTH  210
#define A4_HEIGHT 297

#define A3_WIDTH  297
#define A3_HEIGHT 420

typedef signed char     int8_t;
typedef short int       int16_t;
typedef int             int32_t;
# if __WORDSIZE == 64
typedef long int        int64_t;
# else
__extension__
typedef long long int   int64_t;
#endif

struct color
{
    double red;
    double green;
    double blue;
    double alpha;
};

//全局变量
double page_width, page_height;
int8_t surface_type;

//函数声明
void draw (char *outfile);

#endif // DRAW_H
