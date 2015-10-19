//
// Created by hedejin on 9/6/15.
//

#ifndef P2_SUBDIVIDE_H
#define P2_SUBDIVIDE_H

#include <vector>
#include <set>
#include "scene/mesh.hpp"
#include <map>
#endif //P2_SUBDIVIDE_H

namespace _462 {

    struct SDTriangle;
    struct SDVertex;

    class Subdiv {

    public:
        Subdiv(int nlevels, unsigned ntriangles, unsigned nvertices, const int *vertexIndices, const Vector3 *vertices);
        Vector3 weightOneRing(SDVertex *vert, float beta);
        Vector3 weightBoundary(SDVertex *vert, float beta);

        void Refine();
        std::vector<SDVertex *> SDvertices;
        std::vector<SDTriangle *> SDtriangles;

    private:
        int nlevels;


        static float beta(int valence)
        {
            if(valence == 3)
                return 3.f/16.f;
            else
                return 3.f/(8.f *valence);
        }

    };



}
