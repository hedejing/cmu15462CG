/**
 * @file project.cpp
 * @brief OpenGL project
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "p1/project.hpp"

// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

#include <vector>

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

// definitions of functions for the OpenglProject class

// constructor, invoked when object is created
OpenglProject::OpenglProject()
{
    // TODO any basic construction or initialization of members
    // Warning: Although members' constructors are automatically called,
    // ints, floats, pointers, and classes with empty contructors all
    // will have uninitialized data!
	
	
	
}

// destructor, invoked when object is destroyed
OpenglProject::~OpenglProject()
{
    // TODO any final cleanup of members
    // Warning: Do not throw exceptions or call virtual functions from deconstructors!
    // They will cause undefined behavior (probably a crash, but perhaps worse).
}

/**
 * Initialize the project, doing any necessary opengl initialization.
 * @param camera An already-initialized camera.
 * @param scene The scene to render.
 * @return true on success, false on error.
 */
bool OpenglProject::initialize( Camera* camera, Scene* scene )
{
    // copy scene
    this->scene = *scene;
	
    // TODO opengl initialization code and precomputation of mesh/heightmap

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT);

	//lighting
/*	GLfloat light_ambient[] = {1.0,1.0,1.0,1.0};
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);*/

	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f};
	GLfloat diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);

		glEnable(GL_COLOR_MATERIAL);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_NORMALIZE);
		glEnable(GL_RESCALE_NORMAL);
	//compute normal vector for mesh
	mesh_normal = new Vector3[scene->mesh.num_vertices];
	Vector3 *triangle_normal = new Vector3[scene->mesh.num_triangles];
	std::vector<unsigned> *vertex2triangle = new std::vector<unsigned>[scene->mesh.num_vertices];

	for(unsigned i = 0;i < scene->mesh.num_triangles; i++)
	{
		unsigned int *index = scene->mesh.triangles[i].vertices;
		Vector3 v0 = scene->mesh.vertices[index[0]];
		Vector3 v1 = scene->mesh.vertices[index[1]];
		Vector3 v2 = scene->mesh.vertices[index[2]];
		Vector3 normal = normalize(cross((v1-v0),(v2-v0)));
		triangle_normal[i] = normal;

		//construct vertex2triangle
		vertex2triangle[index[0]].push_back(i);
		vertex2triangle[index[1]].push_back(i);
		vertex2triangle[index[2]].push_back(i);
	}

	for(unsigned i = 0; i<scene->mesh.num_vertices; i++)
	{
		std::vector<unsigned> belong_triangles = vertex2triangle[i];
		Vector3 vertex_normal;
		for(unsigned j=0; j< belong_triangles.size(); j++)
		{
			unsigned triangle_index = belong_triangles[j];
			vertex_normal += triangle_normal[triangle_index];
		}
		vertex_normal /= belong_triangles.size();
		mesh_normal[i] = vertex_normal;
	}

	return true;
}

/**
 * Clean up the project. Free any memory, etc.
 */
void OpenglProject::destroy()
{
    // TODO any cleanup code, e.g., freeing memory
}

/**
 * Perform an update step. This happens on a regular interval.
 * @param dt The time difference from the previous frame to the current.
 */
void OpenglProject::update( real_t dt )
{
    // update our heightmap
    scene.heightmap->update( dt );

    // TODO any update code, e.g. commputing heightmap mesh positions and normals

	for(unsigned int i=0;i < vertices_per_axis; i++)
		for(unsigned int j=0; j< vertices_per_axis ;j++)
		{
			Vector2 vec2((real_t)i*2.0/vertices_per_axis-1, (real_t)j*2.0/vertices_per_axis -1);
			height_map[i][j]=scene.heightmap->compute_height(vec2);
			height_map_normal[i][j]=scene.heightmap->compute_normal(vec2);
		}

	//construct height_map normals
}


/**
 * Clear the screen, then render the mesh using the given camera.
 * @param camera The logical camera to use.
 * @see math/camera.hpp
 */
void OpenglProject::render( const Camera* camera )
{
    // TODO render code
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//project transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->get_fov_degrees(),camera->get_aspect_ratio(),camera->get_near_clip(), camera->get_far_clip());

	//viewing transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vector3 center = camera->get_position()+camera->get_direction();
	gluLookAt(camera->get_position().x, camera->get_position().y, camera->get_position().z, center.x,
			  center.y, center.z, camera->get_up().x, camera->get_up().y, camera->get_up().z);

	//materials
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_diffuse_orangle[] = { 249.f/255, 139.f/255, 63.f/255, 1.0 };
	GLfloat mat_ambient_water[] = { 0.0215, 0.215, 0.4745, 1.0 };
	GLfloat mat_diffuse_water[] = { 0.07568, 0.27568, 0.61424, 1.0 };
	GLfloat mat_specular_water[] = { 0.233, 0.52781, 0.633, 1.0 };
	GLfloat mat_shininess_water[] = { 0.6 };
	GLfloat no_shininess[] = { 0.0 };
	GLfloat low_shininess[] = { 5.0 };
	GLfloat high_shininess[] = { 100.0 };

	//transformation for mesh
	glPushMatrix();
		glTranslated(scene.mesh_position.position.x, scene.mesh_position.position.y, scene.mesh_position.position.z);
		glScaled(scene.mesh_position.scale.x,scene.mesh_position.scale.y,scene.mesh_position.scale.z);
		Vector3 rotate_axis;
		real_t rotate_angle;
		scene.mesh_position.orientation.to_axis_angle(&rotate_axis,&rotate_angle);
		glRotated(rotate_angle,rotate_axis.x,rotate_axis.y,rotate_axis.z);
	//material of mesh
	/*
	glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_orangle);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
	*/
	glColor3f(0.65f, 0.15f, 0.15f);

	// set material
	GLfloat specref1[] = { 0.05f, 0.05f, 0.05f, 0.5f };
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref1);
	glMateriali(GL_FRONT, GL_SHININESS, 23);
		draw_mesh();

	glPopMatrix();
	//transformation for height map
	glPushMatrix();
		glTranslated(scene.heightmap_position.position.x, scene.heightmap_position.position.y, scene.heightmap_position.position.z);

		scene.heightmap_position.orientation.to_axis_angle(&rotate_axis,&rotate_angle);
		glRotated(rotate_angle,rotate_axis.x,rotate_axis.y,rotate_axis.z);
		glScaled(scene.heightmap_position.scale.x, scene.heightmap_position.scale.y, scene.heightmap_position.scale.z);

	//material of height map
	glColor4f( 0.15f, 0.35f, 0.85f, 0.2f);

	// set material
	GLfloat specref2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specref2);
	glMateriali(GL_FRONT, GL_SHININESS, 128);
		draw_height_map();
	glPopMatrix();

	//compute the normal

	//render

}

void OpenglProject::draw_mesh()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	Vector3 *vertices = scene.mesh.vertices;
	glVertexPointer(3,GL_DOUBLE,0,vertices );

	Triangle* triangles = scene.mesh.triangles;

	glBegin(GL_TRIANGLES);
	for(int i=0; i<scene.mesh.num_triangles;i++)
	{
		unsigned vertex0 = triangles[i].vertices[0];
		unsigned vertex1 = triangles[i].vertices[1];
		unsigned vertex2 = triangles[i].vertices[2];
		glNormal3d(mesh_normal[vertex0].x, mesh_normal[vertex0].y, mesh_normal[vertex0].z);
		glArrayElement(vertex0);
		glNormal3d(mesh_normal[vertex1].x, mesh_normal[vertex1].y, mesh_normal[vertex1].z);
		glArrayElement(vertex1);
		glNormal3d(mesh_normal[vertex2].x, mesh_normal[vertex2].y, mesh_normal[vertex2].z);
		glArrayElement(vertex2);
	}
	glEnd();
	//glDrawElements(GL_TRIANGLES, scene.mesh.num_triangles,GL_UNSIGNED_INT,triangles );

	glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenglProject::draw_height_map()
{
	GLdouble TS = 2.0f/vertices_per_axis;
	glColor3d(Color3::Blue.r,Color3::Blue.g,Color3::Blue.b);

	for(unsigned i = 0; i<vertices_per_axis-1;i++)
		for(unsigned j = 0; j<vertices_per_axis-1;j++)
		{
			GLdouble vx = i*2.0/vertices_per_axis -1;
			GLdouble vz = j*2.0/vertices_per_axis -1;
			glBegin(GL_TRIANGLES);

			glNormal3d(height_map_normal[i][j].x, height_map_normal[i][j].y, height_map_normal[i][j].z);
			glVertex3d(vx, height_map[i][j], vz);

			glNormal3d(height_map_normal[i][j+1].x, height_map_normal[i][j+1].y, height_map_normal[i][j+1].z);
			glVertex3d(vx, height_map[i][j+1], vz+TS);

			glNormal3d(height_map_normal[i+1][j].x, height_map_normal[i+1][j].y, height_map_normal[i+1][j].z);
			glVertex3d(vx+TS, height_map[i+1][j], vz);

			glEnd();

			glBegin(GL_TRIANGLES);

			glNormal3d(height_map_normal[i+1][j+1].x, height_map_normal[i+1][j+1].y, height_map_normal[i+1][j+1].z);
			glVertex3d(vx+TS, height_map[i+1][j+1], vz+TS);

			glNormal3d(height_map_normal[i+1][j].x, height_map_normal[i+1][j].y, height_map_normal[i+1][j].z);
			glVertex3d(vx+TS, height_map[i+1][j], vz);

			glNormal3d(height_map_normal[i][j+1].x, height_map_normal[i][j+1].y, height_map_normal[i][j+1].z);
			glVertex3d(vx, height_map[i][j+1], vz+TS);


			glEnd();



		}
}

} /* _462 */

