#include "shape.h" /* Shape class interface */

/* constructor implementation */
Shape::Shape(int16_t x, int16_t y)
: x(x),
  y(y)
{
}

/* move-by operation implementation */
void Shape::moveBy(int16_t dx, int16_t dy) {
    x += dx;
    y += dy;
}

/* calculate the "taxicab" distance from the other Shape */
uint16_t Shape::distanceFrom(Shape const * other) const {
    int16_t dx = x - other->x;
    int16_t dy = y - other->y;
    if (dx < 0) {
       dx = -dx;
    }
    if (dy < 0) {
       dy = -dy;
    }
    return dx + dy;
}
