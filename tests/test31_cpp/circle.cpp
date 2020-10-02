#include "circle.h" /* Circle class interface */

Circle::Circle(int16_t x0, int16_t y0,
                     uint16_t r0)
: Shape(x0, y0), /* base class ctor */

  /* init attributes added in this class */
  rad(r0)
{
}

void Circle::draw() const {
    //drawEllipse(x, y, rad, rad);
}

uint32_t Circle::area() const {
    return ((uint32_t)(3.1415926 * (1U << 16))
           * (uint32_t)rad * (uint32_t)rad) >> 16;
}
