#include "vec_math.h"

bool Vector2::operator==(const Vector2& rhs) const {
	return rhs.x == x && rhs.y == y;
}

bool Vector2::operator!=(const Vector2& rhs) const {
	return !this->operator==(rhs);
}