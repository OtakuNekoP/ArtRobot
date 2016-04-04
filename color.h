#ifndef COLOR_H
#define COLOR_H

class color
{
private:
    double red;
    double green;
    double blue;
    double alpha;

public:
    color();
    color(int32_t code);
    color(const char *);
    color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    color(double red, double green, double blue, double alpha);
    void set();
    void set(int32_t code);
    void set(const char *code);
    void set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void set(double red, double green, double blue, double alpha);
    double red_double();
    double green_double();
    double blue_double();
    double alpha_double();
};

#endif // COLOR_H
