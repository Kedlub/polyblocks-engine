#include "aabb.hpp"
#include <glm/gtc/matrix_transform.hpp>

AABB::AABB(float x, float y, float z, float w, float h, float d) {
    center.x = x;
    center.y = y;
    center.z = z;
    halfSize.x = w / 2;
    halfSize.y = h / 2;
    halfSize.z = d / 2;
}

// Check if this box overlaps with another box
bool AABB::overlaps(const AABB& other) const {
    // If any of these conditions are true,
    // then there is no overlap
    if (abs(center.x - other.center.x) > (halfSize.x + other.halfSize.x)) return false;
    if (abs(center.y - other.center.y) > (halfSize.y + other.halfSize.y)) return false;
    if (abs(center.z - other.center.z) > (halfSize.z + other.halfSize.z)) return false;

    // Otherwise, there is overlap
    return true;
}

// Update position
void AABB::updatePosition(float x, float y, float z) {
	center.x = x;
	center.y = y;
	center.z = z;
}