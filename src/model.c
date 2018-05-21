#include "model.h"

vector_t * g_vertex;
vector_t * g_norm;
vector_t * g_texcoord;
vector_t * g_face;

//Use this to initialyse vectors
void init_vectors() {
	g_vertex = Vector();
	VectorClear(g_vertex);
	g_norm = Vector();
	VectorClear(g_norm);
	g_texcoord = Vector();
	VectorClear(g_texcoord);
	g_face = Vector();
	VectorClear(g_face);
}

vector_t * ModelVertices() {
	return g_vertex;
}

vector_t * ModelNormals() {
	return g_norm;
}

vector_t * ModelTexcoords() {
	return g_texcoord;
}

vector_t * ModelFaces() {
	return g_face;
}

vec3f_t ModelGetVertex( int index ) {
	vec3f_t v = *(vec3f_t*)VectorGetFromIdx( ModelVertices(), index );
	return v;
}

vec3f_t ModelGetNormal( int index ) {
	vec3f_t t = *(vec3f_t*)VectorGetFromIdx( ModelNormals(), index );
	return t;
}

vec3f_t ModelGetTexcoord( int index ) {
	vec3f_t t = *(vec3f_t*)VectorGetFromIdx( ModelTexcoords(), index );
	return t;
}

face_t ModelGetFace( int index ) {
	face_t f = *(face_t*)VectorGetFromIdx( ModelFaces(), index );
	return f;
}

bool ModelLoad( char * objfilename ) {
	// TODO
	// Open .obj
	FILE *fd = fopen(objfilename, "r"); 
	if (fd == NULL){ 
        printf("Could not open the file\n");
        return false;
    }
    // init
	init_vectors(); 
	// process

	int loading = 1;
        while(loading){
            char c1 = '-', c2 = '-';
            fscanf(fd, "%c", &c1); 
            if (c1 != EOF)
            	fscanf(fd, "%c", &c2);
            if (c1 == 'v') {
            	if (c2 == 'n') {
	                float x, y, z;
	                fscanf(fd, "%f %f %f\n", &x, &y, &z);
	                vec3f_t * vec = (vec3f_t*)malloc(sizeof(vec3f_t));
	                Vec3fSet(vec, x, y, z);
	                VectorAdd(g_norm, vec);
	            } else {
		            if (c2 == 't') {
		            	float u, v, trash;
		                fscanf(fd, "%f %f %f\n", &u, &v, &trash);
		                vec2f_t * vec = (vec2f_t*)malloc(sizeof(vec2f_t));
		                Vec2fSet(vec, u, v);
		                VectorAdd(g_texcoord, vec);
		            } else {
		            	float x, y, z;
		                fscanf(fd, "%f %f %f\n", &x, &y, &z);
		                vec3f_t * vec = (vec3f_t*)malloc(sizeof(vec3f_t));
		                Vec3fSet(vec, x, y, z);
		                VectorAdd(g_vertex, vec);
		            }
		        }
            } else if (c1 == 'f') {
                int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
                fscanf(fd, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
                face_t * f = (face_t*)malloc(sizeof(face_t));

                f->v[0] = v1;
                f->v[1] = v2;
                f->v[2] = v3;

                f->vt[0] = vt1;
                f->vt[1] = vt2;
                f->vt[2] = vt3;

                f->vn[0] = vn1;
                f->vn[1] = vn2;
                f->vn[2] = vn3;
                VectorAdd(g_face, f);
        	} else if(c1 == '#') { // skip between data
        		char str[40] = "";
        		fgets(str, 40, fd);
        		fgets(str, 40, fd);
        	} else if (c1 == 'g') {
        		char str[40] = "";
        		fgets(str, 40, fd); //name of group
        	} else if (c1 == 's') {
        		char str[40] = "";
        		fgets(str, 40, fd); //value of smooth
        	} else {
                loading = 0; //
            }
        }

    fclose(fd);
	return true;
}

