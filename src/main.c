#include "window.h"
#include "events.h"
#include "vector.h"
#include "geometry.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <time.h>
#include <iostream>
#include <stdlib.h>

#define ZOOM 300

	const int width		= 1024;
	const int height	= 768;

void drawObj(window_t* mainwindow, vec3f_t light)
{
	int l = VectorGetLength(ModelFaces());

	for (int i = 0; i < l; i++)
	{
		printf("%d\n", i);
		face_t f = ModelGetFace(i);
		vec3f_t p0 = ModelGetVertex(f.v[0]-1);
		vec3f_t p1 = ModelGetVertex(f.v[1]-1);
		vec3f_t p2 = ModelGetVertex(f.v[2]-1);
		//Calcul de la normal du triangle
		vec3f_t n0 = ModelGetNormal(f.vn[0]-1);
		vec3f_t n1 = ModelGetNormal(f.vn[1]-1);
		vec3f_t n2 = ModelGetNormal(f.vn[2]-1);
		//récupération des coords de textures
		vec3f_t t0 = ModelGetTexcoord(f.vn[0]-1);
		vec3f_t t1 = ModelGetTexcoord(f.vn[1]-1);
		vec3f_t t2 = ModelGetTexcoord(f.vn[2]-1);

		vec3f_t n = Vec3f((n0.x+n1.x+n2.x)/3,
						  (n0.y+n1.y+n2.y)/3,
						  (n0.z+n1.z+n2.z)/3);

		// Produit scalaire
		float intensity = (light.x * n.x + light.y * n.y + light.z * n.z);

		if (intensity >= -0.2) { //-0.2 OK pour light normale
			float intens = ((intensity > 0)?intensity:-intensity) * 255;
			WindowDrawTriangle(mainwindow,  width / 2 + p0.x * ZOOM, height / 2 + p0.y * ZOOM, p0.z*300, 
										width / 2 + p1.x * ZOOM, height / 2 + p1.y * ZOOM, p1.z*300, 
										width / 2 + p2.x * ZOOM, height / 2 + p2.y * ZOOM, p2.z*300, 
										intens, intens, intens);
		}
	}
}

void drawObj_aleat( window_t* mainwindow )
{
	int l = VectorGetLength(ModelFaces());

	for (int i = 0; i < l; i++)
	{
		printf("%d\n", i);
		face_t f = ModelGetFace(i);
		vec3f_t p0 = ModelGetVertex(f.v[0]-1);
		vec3f_t p1 = ModelGetVertex(f.v[1]-1);
		vec3f_t p2 = ModelGetVertex(f.v[2]-1);
		WindowDrawTriangle(mainwindow,  width / 2 + p0.x * ZOOM, height / 2 + p0.y * ZOOM, p0.z*0, 
									width / 2 + p1.x * ZOOM, height / 2 + p1.y * ZOOM, p1.z*0, 
									width / 2 + p2.x * ZOOM, height / 2 + p2.y * ZOOM, p2.z*0, 
									rand()%255, rand()%255, rand()%255);
	}
}

void drawObj_wireframe(window_t* mainwindow, Uint8 r, Uint8 g, Uint8 b )
{
	int l = VectorGetLength(ModelFaces());

	for (int i = 0; i < l; i++)
	{
		printf("%d\n", i);
		face_t f = ModelGetFace(i);
		vec3f_t p0 = ModelGetVertex(f.v[0]-1);
		vec3f_t p1 = ModelGetVertex(f.v[1]-1);
		vec3f_t p2 = ModelGetVertex(f.v[2]-1);

		WindowDrawLine(mainwindow, width / 2 + p0.x * ZOOM, height / 2 + p0.y * ZOOM, p0.z * ZOOM, width / 2 + p1.x * ZOOM, height / 2 + p1.y * ZOOM, p1.z * ZOOM, r, g, b);
		WindowDrawLine(mainwindow, width / 2 + p2.x * ZOOM, height / 2 + p2.y * ZOOM, p2.z * ZOOM, width / 2 + p1.x * ZOOM, height / 2 + p1.y * ZOOM, p1.z * ZOOM, r, g, b);
		WindowDrawLine(mainwindow, width / 2 + p0.x * ZOOM, height / 2 + p0.y * ZOOM, p0.z * ZOOM, width / 2 + p2.x * ZOOM, height / 2 + p2.y * ZOOM, p2.z * ZOOM, r, g, b);
	}
}

int main( int argc, char ** argv ) 
{

	if (argc < 1)
		return -1;
	if (!ModelLoad(argv[1]))
		return -1;
	

	// Open a new window
	window_t * mainwindow = WindowInit( width, height, 4 );

	int done = false;
	// Random
	srand(time(NULL));
	//Light
	vec3f_t light = Vec3f(0, 0, 1);

	// While user doesn't close
	while ( !done ) {	
		
		// Update with events
		done = EventsUpdate( mainwindow );
		
		// Filling background with a color
		WindowDrawClearColor( mainwindow, 255, 64, 64 );
		
		// Draw a point
		//WindowDrawPoint( mainwindow, width / 2, height / 2, 0, 0, 0 );

		// Draw an object in wireframe mod
		//drawObj_wireframe( mainwindow, 0, 0, 0 );

		// Draw an object with random colors
		//drawObj_aleat( mainwindow );

		// Draw an object
		drawObj( mainwindow, light);

		// Update window
		WindowUpdate( mainwindow );

	}
	
	// Close
	WindowDestroy( mainwindow );

	return 1;
}

