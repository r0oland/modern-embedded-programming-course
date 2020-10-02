#ifndef SHAPE_H
#define SHAPE_H

#include <stdint.h>

class Shape {
private: /* Shape's attributes... */
    int16_t x; /* x-coordinate of Shape's position */
    int16_t y; /* y-coordinate of Shape's position */

public: /* Shape's operations (Shape's interface)... */
    Shape(int16_t x, int16_t y);
    void moveBy(int16_t dx, int16_t dy);
    uint16_t distanceFrom(Shape const * other1) const;
} ;

#endif /* SHAPE_H */
