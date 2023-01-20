#ifndef POINT_H
#define POINT_H

#include <vector>

struct Point {
    float m_x = -1, m_y = -1;
    float m_mass = 0;
    float m_velX = 0, m_velY = 0;
    bool operator==(const Point &other) const{
        return m_x == other.m_x && m_y == other.m_y;
    }
};

struct Force {
    float m_x = 0, m_y = 0;
};


#endif // POINT_H
