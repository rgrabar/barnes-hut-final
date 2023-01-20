#ifndef QUADTREE_H
#define QUADTREE_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "point.h"

#define THETA 1
#define G_CONSTANT 1

struct QuadTree {

private:
    float m_newX = 0, m_newY = 0;
    bool m_leaf = true;
    bool m_empty = true;
    Point m_point;
    struct Force m_force;

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
        if(!m_empty && p == m_point){
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
            ul->centerOfMass();
            m_mass += ul->m_mass;

            ur->centerOfMass();
            m_mass += ur->m_mass;

            dl->centerOfMass();
            m_mass += dl->m_mass;

            dr->centerOfMass();
            m_mass += dr->m_mass;

            m_newY += (ul->m_point.m_y * ul->m_mass +
                      ur->m_point.m_y * ur->m_mass +
                      dl->m_point.m_y * dl->m_mass +
                      dr->m_point.m_y * dr->m_mass
                      ) / m_mass;
            m_newX += (ul->m_point.m_x * ul->m_mass +
                      ur->m_point.m_x * ur->m_mass +
                      dl->m_point.m_x * dl->m_mass +
                      dr->m_point.m_x * dr->m_mass
                      ) / m_mass;
            return;
        }
    }

    Force forcen(const Point& p) {
         if (m_empty) {
             m_force.m_x = 0;
             m_force.m_y = 0;
            return {0, 0};
        } else if (m_leaf) {
            const float dist = sqrt((p.m_x-m_newX)*(p.m_x-m_newX) + (p.m_y-m_newY)*(p.m_y-m_newY)) + 0.0025;
            const float magnitude = G_CONSTANT*m_mass/(dist*dist*dist);
            m_force.m_x = m_newX - p.m_x;
            m_force.m_y = m_newY - p.m_y;
            m_force.m_x *= magnitude;
            m_force.m_y *= magnitude;

            return m_force;
        } else { // parent

            const float dist = sqrt((p.m_x-m_newX)*(p.m_x-m_newX) + (p.m_y-m_newY)*(p.m_y-m_newY)) + 0.0025;
            if((m_maxX - m_minX) / dist <= THETA){
                const float magnitude = G_CONSTANT*m_mass/(dist*dist*dist);
                m_force.m_x = m_newX - p.m_x;
                m_force.m_y = m_newY - p.m_y;
                m_force.m_x *= magnitude;
                m_force.m_y *= magnitude;

                return m_force;

            }else {
                ul->forcen(p);
                m_force.m_x = ul->m_force.m_x;
                m_force.m_y = ul->m_force.m_y;

                ur->forcen(p);
                m_force.m_x += ur->m_force.m_x;
                m_force.m_y += ur->m_force.m_y;

                dl->forcen(p);
                m_force.m_x += dl->m_force.m_x;
                m_force.m_y += dl->m_force.m_y;

                dr->forcen(p);
                m_force.m_x += dr->m_force.m_x;
                m_force.m_y += dr->m_force.m_y;

                return m_force;
            }
        }
    }
};

// static vector in quadtree
std::vector<Point> QuadTree::allLinesDraw;

#endif // QUADTREE_H
