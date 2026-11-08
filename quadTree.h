#ifndef QUADTREE_H
#define QUADTREE_H

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "point.h"

#define G_CONSTANT 1

struct QuadTree {

private:
    float m_newX = 0, m_newY = 0;
    bool m_leaf = true;
    bool m_empty = true;
    Point m_point;

public:

    float m_minX = 0, m_maxX = 0, m_minY = 0, m_maxY = 0;
    float m_mass = 0;

    static std::vector <Point> allLinesDraw;

    QuadTree(){}
    QuadTree(float _minX, float _maxX, float _minY, float _maxY): m_minX(_minX), m_maxX(_maxX), m_minY(_minY), m_maxY(_maxY){}

    ~QuadTree() {
        if(ul != nullptr)
            delete(ul);
        if(ur != nullptr)
            delete(ur);
        if(dl != nullptr)
            delete(dl);
        if(dr != nullptr)
            delete(dr);
    }

    QuadTree *ul = nullptr;
    QuadTree *dl = nullptr;
    QuadTree *ur = nullptr;
    QuadTree *dr = nullptr;

    void insert(const Point &p) {
        // leaf and empty
        if(m_leaf && m_empty) {
            m_empty = false;
            m_point = p;

            allLinesDraw.push_back({(m_minX + m_maxX) / 2, m_minY});
            allLinesDraw.push_back({(m_minX + m_maxX) / 2, m_maxY});
            allLinesDraw.push_back({m_minX, (m_minY + m_maxY) / 2});
            allLinesDraw.push_back({m_maxX, (m_minY + m_maxY) / 2});

            return;
        }

        const float midpointX = (m_minX + m_maxX) / 2;
        const float midpointY = (m_minY + m_maxY) / 2;

        bool left = p.m_x <= midpointX;
        bool above = p.m_y <= midpointY;

        // not leaf, check position in tree
        if(!m_leaf) {
            if(above && left)   ul->insert(p);
            if(above && !left)  ur->insert(p);
            if(!above && left)  dl->insert(p);
            if(!above && !left) dr->insert(p);
            return;
        }

        // leaf and not empty -> split
        if(p == m_point){
            m_point.m_mass += p.m_mass;
            return;
        }
        // split
        m_leaf = false;
        ul = new QuadTree{m_minX, midpointX, m_minY, midpointY};
        ur = new QuadTree{midpointX, m_maxX, m_minY, midpointY};
        dl = new QuadTree{m_minX, midpointX, midpointY, m_maxY};
        dr = new QuadTree{midpointX,m_maxX, midpointY, m_maxY};

        if(above && left)   ul->insert(p);
        if(above && !left)  ur->insert(p);
        if(!above && left)  dl->insert(p);
        if(!above && !left) dr->insert(p);

        // pass current point to the kids
        left = m_point.m_x <= midpointX;
        above = m_point.m_y <= midpointY;

        if(above && left)   ul->insert(m_point);
        if(above && !left)  ur->insert(m_point);
        if(!above && left)  dl->insert(m_point);
        if(!above && !left) dr->insert(m_point);
    }

    void centerOfMass() {
        if (m_empty) {
            m_newY = 0;
            m_newX = 0;
            m_mass = 0;
            return;
        } else if (m_leaf) {
            // leaf and not empty end of branch
            m_mass = m_point.m_mass;
            m_newX = m_point.m_x;
            m_newY = m_point.m_y;
            return;
        } else { // parent
            m_mass = 0;
            ul->centerOfMass();
            m_mass += ul->m_mass;

            ur->centerOfMass();
            m_mass += ur->m_mass;

            dl->centerOfMass();
            m_mass += dl->m_mass;

            dr->centerOfMass();
            m_mass += dr->m_mass;

            m_newY = (ul->m_newY * ul->m_mass +
                      ur->m_newY * ur->m_mass +
                      dl->m_newY * dl->m_mass +
                      dr->m_newY * dr->m_mass
                      ) / m_mass;
            m_newX = (ul->m_newX * ul->m_mass +
                      ur->m_newX * ur->m_mass +
                      dl->m_newX * dl->m_mass +
                      dr->m_newX * dr->m_mass
                      ) / m_mass;
            return;
        }
    }

    static constexpr float SOFTENING = 0.0025f;

    Force forcen(const Point& p, float theta) const {
        if (m_empty)
            return {0, 0};
        if (m_leaf && m_point == p)      // this leaf is p itself: no self-force
            return {0, 0};
    
        const float dx = m_newX - p.m_x;
        const float dy = m_newY - p.m_y;
        const float r  = std::sqrt(dx * dx + dy * dy);
    
        const float size = std::max(m_maxX - m_minX, m_maxY - m_minY);
        const bool containsP = p.m_x >= m_minX && p.m_x <= m_maxX &&
                               p.m_y >= m_minY && p.m_y <= m_maxY;
    
        // A single body, or a cell that is far enough away and doesn't contain p:
        // treat it as one point mass at its center of mass.
        if (m_leaf || (!containsP && size < theta * r)) {
            const float d = r + SOFTENING;
            const float k = G_CONSTANT * m_mass / (d * d * d);
            return {dx * k, dy * k};
        }
    
        // Otherwise open the cell and add up the children's forces.
        Force f;
        for (const QuadTree* child : {ul, ur, dl, dr}) {
            const Force cf = child->forcen(p, theta);
            f.m_x += cf.m_x;
            f.m_y += cf.m_y;
        }
        return f;
    }
};

// static vector in quadtree
std::vector<Point> QuadTree::allLinesDraw;

#endif // QUADTREE_H
