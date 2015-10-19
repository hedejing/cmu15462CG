varying vec3 norm;
varying vec3 cam_dir;
varying vec3 ref;

// Declare any additional variables here. You may need some uniform variables.

uniform samplerCube cubemap;

void main(void)
{
	// Your shader code here.
	// Note that this shader won't compile since gl_FragColor is never set.
	gl_FragColor = textureCube(cubemap, ref);
}
