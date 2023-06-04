#ifndef AABB_H
#define AABB_H

class AABB {
public:
    // Constructor
    AABB(float x, float y, float z, float w, float h, float d);

    // Center point of the box
    glm::vec3 center;

    // Half of the box's size along each axis
    glm::vec3 halfSize;

    // Check if this box overlaps with another box
    bool overlaps(const AABB& other) const;

    // Update position
    void updatePosition(float x, float y, float z);
};  

#endif