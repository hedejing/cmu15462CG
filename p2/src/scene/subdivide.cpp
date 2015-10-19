#include "scene/mesh.hpp"
#include "subdivide.h"
#define NEXT(i) (((i)+1)%3)
#define PREV(i) (((i)+2)%3)

#include <iostream>
namespace _462 {
    struct SDTriangle;
    struct SDVertex;

    struct SDVertex {

        SDVertex(Vector3 v = Vector3::Zero)
                : vertex(v), startTriangle(NULL), child(NULL), regular(false), boundary(false) { }

        int valence();
        void oneRing(Vector3 *vertices);

        Vector3 vertex;
        SDTriangle *startTriangle;
        SDVertex *child;
        bool regular, boundary;

    };

    struct SDTriangle {
        SDTriangle(){
            for (int i = 0; i < 3; ++i) {
                neighbor[i] = NULL;
                vertices[i] = NULL;
                children[i] = NULL;
            }
            children[3] = NULL;
        }

        int vnum(SDVertex *targetVertex) const
        {
            for(int i=0; i<3 ;i++)
            {
                if(vertices[i] == targetVertex)
                    return i;
            }
            std::cerr << "the target vertex isn't part of this triangle!" << std::endl;
            assert(false);
            return -1;
        }

        SDTriangle *NextTri(SDVertex *vert) const{
            int index = vnum(vert);
            return neighbor[index];
        }
        SDTriangle *PrevTri(SDVertex *vert) const{
            int index = vnum(vert);
            return neighbor[PREV(index)];
        };
        SDVertex *NextVer(SDVertex *vert) const{
            int index = vnum(vert);
            return vertices[NEXT(index)];
        }
        SDVertex *PrevVer(SDVertex *vert) const{
            int index = vnum(vert);
            return vertices[PREV(index)];
        }
        SDVertex *otherVert(SDVertex *v0, SDVertex *v1) const{
            for (int i = 0; i < 3; ++i) {
                if((vertices[i] !=v0)&&(vertices[i]!=v1))
                    return vertices[i];
            }
            std::cerr << "Basic logic error in SDVertex::otherVert!" <<std::endl;
            assert(false);
            return NULL;
        }


        SDTriangle *neighbor[3];
        SDVertex *vertices[3];
        SDTriangle *children[4];

    };



    struct SDEdge{
        SDEdge(SDVertex *v0 = NULL,SDVertex *v1 = NULL )
        {
            vertices[0] = std::min(v0, v1);
            vertices[1] = std::max(v1, v0);
            triangles[0] = NULL;
            triangles[1] = NULL;
            index = -1;
        }

        bool operator==(const SDEdge& e)const
        {
            return ((vertices[0] == e.vertices[0]) && (vertices[1] == e.vertices[1]));
        }

        bool operator<(const SDEdge& e) const
        {
            if(vertices[0] == e.vertices[0])
                return vertices[1] < e.vertices[1];
            else
                return vertices[0] < e.vertices[0];
        }

        SDVertex *vertices[2];
        SDTriangle *triangles[2];
        int index;
    };

    void SDVertex::oneRing(Vector3 *vertices)
    {
        if(!boundary) //interior
        {
            unsigned i = 0;
            vertices[0] = startTriangle->NextVer(this)->vertex;
            SDTriangle *next = startTriangle->NextTri(this);
            while(next != startTriangle)
            {
                i++;
                vertices[i] = next->NextVer(this)->vertex;
                next = next->NextTri(this);
            }
        }
        else//boundary
        {
            SDTriangle *SDtri = startTriangle;
            while(SDtri->NextTri(this) != NULL)
            {
                SDtri = SDtri->NextTri(this);
            }
            vertices[0] = SDtri->NextVer(this)->vertex;
            unsigned i=0;
            while(1)
            {
                i++;
                vertices[i] = SDtri->PrevVer(this)->vertex;
                SDtri = SDtri->PrevTri(this);
                if(SDtri == NULL)
                    break;
            }

        }
    }

    inline int SDVertex::valence()
    {
        SDTriangle *start = startTriangle;
        if(boundary == false)
        {
            SDTriangle *NextTriangle = start->NextTri(this);
            int count = 1;
            while(NextTriangle != start)
            {
                NextTriangle = NextTriangle->NextTri(this);
                count++;
            }
            return count;
        }
        else
        {
            int count = 2;
            SDTriangle *NextTriangle = start->NextTri(this);
            while(NextTriangle != NULL)
            {
                NextTriangle = NextTriangle->NextTri(this);
                count++;
            }
            SDTriangle *PrevTriangle = start->PrevTri(this);
            while(PrevTriangle != NULL)
            {
                PrevTriangle = PrevTriangle->PrevTri(this);
                count++;
            }
            return count;
        }
    }

    Subdiv::Subdiv(int nlevels, unsigned ntriangles, unsigned nvertices, const int *vertexIndices, const Vector3 *vertices)
    {

        this->nlevels = nlevels;
        //Allocate SDvertices and SDfaces
        SDVertex *SDvertexArr = new SDVertex[nvertices];
        for(unsigned i = 0;i < nvertices ; i++)
        {
            SDvertexArr[i] = SDVertex(vertices[i]);
            SDvertices.push_back(&SDvertexArr[i]);
        }

        SDTriangle *SDtriArr = new SDTriangle[ntriangles];
        for(unsigned i = 0; i< ntriangles; i++)
        {
            SDtriangles.push_back(&SDtriArr[i]);
        }

        //Set face to vertex pointers
        //const int *vp = vertexIndices;
        for(unsigned i = 0; i < ntriangles; i++)
        {
            SDTriangle *tri = SDtriangles[i];
            for(int j = 0; j < 3 ;j++)
            {
                //std::cerr << 3*i+j << std::endl;
                int index = vertexIndices[3*i+j];
                SDVertex *v = SDvertices[index];
                tri->vertices[j] = v;
                v->startTriangle = tri;
            }
            //vp +=3;
        }

        //Set neighbors in faces
        std::set<SDEdge> edges;
        for(unsigned i=0; i<ntriangles; i++)
        {
            SDTriangle *thisTriangle = SDtriangles[i];
            for(unsigned j=0; j<3 ;j++)
            {
                int v0 = j, v1 = NEXT(j);
                SDEdge e(thisTriangle->vertices[v0], thisTriangle->vertices[v1]);

                if(edges.find(e)==edges.end())
                {
                    e.triangles[0] = thisTriangle;
                    e.index = j;
                    edges.insert(e);
                }
                else
                {
                    std::set<SDEdge>::iterator iter = edges.find(e);
                    SDEdge ee = *iter;
                    ee.triangles[0]->neighbor[ee.index] = thisTriangle;
                    thisTriangle->neighbor[j] = ee.triangles[0];
                    edges.erase(e);
                }
            }
        }
/* Interior vertices with valence other than six,
 * or boundary vertices with valence other than four,
 * are called extraordinary vertices;
 * otherwise, they are called regular
 */
        //Initialize other members of SDvertex
        for(unsigned i=0; i<nvertices; i++)
        {
            SDVertex *SDvertex = SDvertices[i];
            SDTriangle *tri = SDvertex->startTriangle;
            do{
                tri = tri->NextTri(SDvertex);
            }
            while(tri && tri!= SDvertex->startTriangle);
            SDvertex->boundary = (tri == NULL);
            /*
            SDTriangle *NextSDtriangle = SDvertex->startTriangle->NextTri(SDvertex);
            unsigned count = 0;
            while(1)
            {
                if(NextSDtriangle == NULL)
                {
                    SDvertex->boundary = true;
                    break;
                }

                if(NextSDtriangle == SDvertex->startTriangle)
                {
                    SDvertex->boundary = false;
                    break;
                }

                NextSDtriangle = NextSDtriangle->NextTri(SDvertex);
            }
*/
            if(SDvertex->boundary == true && SDvertex->valence() == 4)
                SDvertex->regular = true;
            else if(SDvertex->boundary==false && SDvertex->valence() == 6)
                SDvertex->regular = true;
            else
                SDvertex->regular = false;

        }
    }

    Vector3 Subdiv::weightOneRing(SDVertex *vert, float beta)
    {
        //Put vertex onr-ring in
        int valence = vert->valence();
        Vector3 *neighborVertices = new Vector3[valence];
        vert->oneRing(neighborVertices);

        Vector3 newVertex = (1 - valence*beta)*vert->vertex;
        for(int i=0; i<valence; i++)
            newVertex += beta*neighborVertices[i];
        return newVertex;
    }

    Vector3 Subdiv::weightBoundary(SDVertex *vert, float beta){
        int valence = vert->valence();
        Vector3 *neighborVertices = new Vector3[valence];
        vert->oneRing(neighborVertices);

        Vector3 newVertex = (1 - 2*beta)*vert->vertex;
        newVertex +=beta*(neighborVertices[0]+neighborVertices[valence-1]);
        return newVertex;

    }

    void Subdiv::Refine()
    {
        std::vector<SDTriangle *> t = SDtriangles;
        std::vector<SDVertex *> v = SDvertices;
        for(int i=0; i<nlevels; i++)
        {
            //update triangles and vertices for next level of subdivision

            std::vector<SDTriangle *> newTriangles;
            std::vector<SDVertex *> newVertices;
            //allocate next level of children in mesh tree
            SDTriangle *SDTriangleArr = new SDTriangle[t.size()*4];
            for(unsigned j =0 ; j <t.size(); j++)
            {
                for(int k=0; k <4 ; k++)
                {
                    t[j]->children[k] = &SDTriangleArr[4* j + k];
                    newTriangles.push_back(t[j]->children[k]);
                }

            }
            SDVertex *SDVertexArr = new SDVertex[v.size()];
            for(unsigned j =0; j <v.size(); j++)
            {
                v[j]->child = &SDVertexArr[j];
                v[j]->child->boundary = v[j]->boundary;
                v[j]->child->regular = v[j]->regular;
                newVertices.push_back(v[j]->child);
            }
            //update vertex positions and create new edge vertices
                //update for even vertices
            for(unsigned j =0; j <v.size(); j++)
            {
                if(!v[j]->boundary) //interior
                {
                    if(v[j]->regular)
                        v[j]->child->vertex = weightOneRing(v[j], 1.f/16.f);
                    else
                        v[j]->child->vertex = weightOneRing(v[j], beta(v[j]->valence()));

                }
                else //boundary
                {
                    v[j]->child->vertex = weightBoundary(v[j], 1.f/8.f);
                }
            }


                //compute new odd vertices
            std::map<SDEdge, SDVertex *>edgeVerts;
            for(unsigned j =0; j <t.size(); j++)
            {
                SDTriangle *Tri = t[j];
                for(int k =0; k <3; k++)
                {
                    SDEdge e(Tri->vertices[k], Tri->vertices[NEXT(k)]);
                    std::map<SDEdge, SDVertex *>::iterator iter = edgeVerts.find(e);
                    if(iter == edgeVerts.end())
                    {
                        //Create and initialize new odd vertex
                        //e.triangles[0] = Tri;
                        SDVertex *vert = new SDVertex;
                        vert->regular = true;
                        vert->boundary = (Tri->neighbor[k] == NULL);
                        vert->startTriangle = Tri->children[3];
                        newVertices.push_back(vert);
                        //Apply edge rules to compute new vertex position
                        if(vert->boundary)
                            vert->vertex = 0.5*(e.vertices[0]->vertex + e.vertices[1]->vertex);
                        else
                        {
                            vert->vertex =3.f/8.f*(e.vertices[0]->vertex + e.vertices[1]->vertex);
                            vert->vertex+=1.f/8.f*(Tri->otherVert(e.vertices[0], e.vertices[1])->vertex +
                                                    Tri->neighbor[k]->otherVert(e.vertices[0],e.vertices[1])->vertex);
                        }
                        edgeVerts.insert({e, vert});
                    }
                    else
                    {
                        //(iter->second).triangles[1] = Tri;
                    }

                }
            }
            //update new mesh topology
                //update even vertex's start faces
            for(unsigned j=0; j<v.size(); j++)
            {
                SDVertex* vert = v[j];
                int index = vert->startTriangle->vnum(vert);
                vert->child->startTriangle = vert->startTriangle->children[index];
            }
                //update new triangle's neighborhood
            for(unsigned j=0; j<t.size(); j++)
            {
                SDTriangle *tri = t[j];
                for(int k=0; k<3; k++)
                {
                    //update children neighbor for siblings //among children of the same parent
                    tri->children[3]->neighbor[k] = tri->children[NEXT(k)];
                    tri->children[k]->neighbor[NEXT(k)] = tri->children[3];
                    //update children for neighbor children //children of other parent
                    SDTriangle* neighbor = tri->neighbor[k];
                    if(neighbor != NULL)
                    {
                        int index = neighbor->vnum(tri->vertices[k]);
                        tri->children[k]->neighbor[k] = neighbor->children[index];
                    }
                    else
                    {
                        tri->children[k]->neighbor[k] = NULL;
                    }
                    SDTriangle* Prevneighbor = tri->neighbor[PREV(k)];
                    if(Prevneighbor)
                    {
                        int index = Prevneighbor->vnum(tri->vertices[k]);
                        tri->children[k]->neighbor[PREV(k)] = Prevneighbor->children[index];
                    }
                    else
                    {
                        tri->children[k]->neighbor[PREV(k)] = NULL;
                    }
                }
            }
                //update new triangle's vertices
            for(unsigned j=0; j<t.size(); j++)
            {
                SDTriangle *tri = t[j];
                for (int k = 0; k < 3; k++)
                {
                    //update children vertex pointer to new even vertex
                    SDVertex* evenvert = tri->vertices[k];
                    tri->children[k]->vertices[k] = evenvert->child;

                    //update children vertex pointer to new odd vertex
                    SDEdge e(evenvert, tri->vertices[NEXT(k)]);
                    SDVertex* oddvert = edgeVerts[e];

                    tri->children[3]->vertices[k] = oddvert;
                    tri->children[k]->vertices[NEXT(k)] = oddvert;
                    tri->children[NEXT(k)]->vertices[k] = oddvert;
                }
            }
            //prepare for next level of subdivision

/*            for(unsigned j=0; j<t.size(); j++)
                delete t[j];

            for(unsigned j=0; j<v.size(); j++)
                delete v[j];*/

            t = newTriangles;
            v = newVertices;
        }
        //create Mesh
        SDtriangles = t;
        SDvertices = v;
    }

bool Mesh::subdivide()
{
    /*
      You should implement loop subdivision here.

      Triangles are stored in an std::vector<MeshTriangle> in 'triangles'.
      Vertices are stored in an std::vector<MeshVertex> in 'vertices'.

      Check mesh.hpp for the Mesh class definition.
     */

    int *vertexIndices = new int[face_list.size()*3];
    for (int i = 0; i < face_list.size(); ++i)
    {
        for(int j = 0; j < 3; j++)
        {
            vertexIndices[3*i+j] = face_list[i].v[j].vertex;
        }
    }

    Vector3 *verts = new Vector3[position_list.size()];

    for (int i = 0; i < position_list.size(); ++i)
    {
        verts[i] = position_list[i];

    }

    Subdiv subdiv = Subdiv(1, face_list.size(), position_list.size(), vertexIndices, verts);
    subdiv.Refine();
    delete[] verts;

    MeshVertexList newVerList;
    position_list.clear();
    position_list.reserve(subdiv.SDvertices.size());
    std::map<SDVertex*, unsigned int> SDVer2Index;
    for (unsigned int i = 0; i < subdiv.SDvertices.size(); ++i)
    {
        SDVertex* SDvert = subdiv.SDvertices[i];
        SDVer2Index[SDvert] = i;
        MeshVertex meshvert;
        meshvert.position = SDvert->vertex;
        newVerList.push_back(meshvert);
        position_list.push_back(SDvert->vertex);
    }

    MeshTriangleList newTriList;
    face_list.clear();
    face_list.reserve(subdiv.SDtriangles.size());
    for (int i = 0; i < subdiv.SDtriangles.size(); ++i)
    {
        SDTriangle *SDtri = subdiv.SDtriangles[i];
        MeshTriangle newTriangle;
        Face f;
        for (int j = 0; j < 3; ++j)
        {
            SDVertex *vert = SDtri->vertices[j];
            unsigned int index = SDVer2Index[vert];
            newTriangle.vertices[j] = index;
            f.v[j].vertex = index;
        }
        face_list.push_back(f);
        newTriList.push_back(newTriangle);
    }
    /*
    //normals
    Vector3 *normals = new Vector3[normal_list.size()];
    std::cerr << normal_list.size();
    std::cerr << "-----------------" <<std::endl;
    for (int i = 0; i < normal_list.size(); ++i)
    {
        std::cerr << normal_list[i];
        normals[i] = normal_list[i];
    }
    std::cerr << "-----------------" <<std::endl;

    Subdiv subdiv_normal = Subdiv(1, face_list.size(), normal_list.size(), vertexIndices, normals);
    subdiv_normal.Refine();
    for (unsigned int i = 0; i < subdiv_normal.SDvertices.size(); ++i)
    {
        newVerList[i].normal = subdiv_normal.SDvertices[i]->vertex;
    }
    delete[] normals;
     */

    delete[] vertexIndices;
    triangles = newTriList;
    vertices = newVerList;
    has_normals = false;
    create_gl_data();

    return true;
}



} /* _462 */
