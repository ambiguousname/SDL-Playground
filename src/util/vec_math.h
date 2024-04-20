#pragma once

struct Vector2 {
	float x;
	float y;
	Vector2(float x, float y) { this->x = x; this->y = y; }
	bool operator==(const Vector2& rhs) const;
	bool operator!=(const Vector2& rhs) const;
};

struct Vector3 {
	float x;
	float y;
	float z;
	
	Vector3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
};