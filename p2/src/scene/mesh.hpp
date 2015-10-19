/**
 * @file mesh.hpp
 * @brief Mesh class and OBJ loader.
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#ifndef _462_SCENE_MESH_HPP_
#define _462_SCENE_MESH_HPP_

#include "math/vector.hpp"

#include <vector>
#include <cassert>

namespace _462 {

    struct TriIndex
    {
        int vertex;
        int normal;
        int tcoord;

        bool operator<( const TriIndex& rhs ) const {
            if ( vertex == rhs.vertex ) {
                if ( normal == rhs.normal ) {
                    return tcoord < rhs.tcoord;
                } else {
                    return normal < rhs.normal;
                }
            } else {
                return vertex < rhs.vertex;
            }
        }
    };

    struct Face
    {
        TriIndex v[3];
    };


struct MeshVertex
{
    //Vector4 color;
    Vector3 position;
    Vector3 normal;
    Vector2 tex_coord;
};

struct MeshTriangle
{
    // index into the vertex list of the 3 vertices
    unsigned int vertices[3];
};

/**
 * A mesh of triangles.
 */
class Mesh
{
public:

    Mesh();
    virtual ~Mesh();

    typedef std::vector< MeshTriangle > MeshTriangleList;
    typedef std::vector< MeshVertex > MeshVertexList;

    typedef std::vector< Vector3 > PositionList;
    typedef std::vector< Vector3 > NormalList;
    typedef std::vector< Vector2 > UVList;
    typedef std::vector< Face > FaceList;

    // The list of all triangles in this model.
    MeshTriangleList triangles;

    // The list of all vertices in this model.
    MeshVertexList vertices;

    // scene loader stores the filename of the mesh here
    std::string filename;

    bool has_tcoords;
    bool has_normals;
    int has_colors;

    // Loads the model into a list of triangles and vertices.
    bool load();

    // Creates opengl data for rendering and computes normals if needed
    bool create_gl_data();

    bool subdivide();

    // Renders the mesh using opengl.
    void render() const;

    //hedejin
    FaceList face_list;
    PositionList position_list;
    NormalList normal_list;
    UVList uv_list;

private:
    typedef std::vector< float > FloatList;
    typedef std::vector< unsigned int > IndexList;

    // the vertex data used for GL rendering
    FloatList vertex_data;
    // the index data used for GL rendering
    IndexList index_data;

    // prevent copy/assignment
    Mesh( const Mesh& );
    Mesh& operator=( const Mesh& );

};


} /* _462 */

#endif /* _462_SCENE_MESH_HPP_ */
