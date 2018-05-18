#include "window.h"
#include "events.h"
#include "vector.h"
#include "geometry.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <stdlib.h>

#define ZOOM 300

	const int width		= 1024;
	const int height	= 768;

int main( int argc, char ** argv ) {



	if (argc < 1)
		return -1;
	if (!ModelLoad(argv[1]))
		return -1;
	int l = VectorGetLength(ModelFaces());

	// Ouverture d'une nouvelle fenêtre
	window_t * mainwindow = WindowInit( width, height, 4 );

	int done = false;
	
	//Lumière
	vec3f_t light = Vec3f(0, 0, 1);

	// Tant que l'utilisateur de ferme pas la fenêtre
	while ( !done ) {	
		
		// Mise à jour et traitement des evênements de la fenêtre
		done = EventsUpdate( mainwindow );
		
		// Effacement de l'écran avec une couleur
		WindowDrawClearColor( mainwindow, 255, 64, 64 );
		
		// Dessin d'un point blanc au milieu de le fenêtre
		//WindowDrawPoint( mainwindow, width / 2, height / 2, 255, 255, 255 );

		//AFFICHAGE TETE
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

			//Produit scalaire
			float intensity = (light.x * n.x + light.y * n.y + light.z * n.z);

			if (intensity >= -0.2) { //-0.2 OK pour light normale
				float intens = ((intensity > 0)?intensity:-intensity) * 255;
				WindowDrawTriangle(mainwindow,  width / 2 + p0.x * ZOOM, height / 2 + p0.y * ZOOM, p0.z*300, 
											width / 2 + p1.x * ZOOM, height / 2 + p1.y * ZOOM, p1.z*300, 
											width / 2 + p2.x * ZOOM, height / 2 + p2.y * ZOOM, p2.z*300, 
											intens, intens, intens,
											t0.x, t0.y,
											t1.x, t1.y,
											t2.x, t2.y);
			}
		}
		// Mise à jour de la fenêtre
		WindowUpdate( mainwindow );

	}
	
	// Fermeture de la fenêtre
	WindowDestroy( mainwindow );

	return 1;
}

