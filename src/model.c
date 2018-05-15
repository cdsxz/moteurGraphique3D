#include "model.h"

vector_t * g_vertex;
vector_t * g_norm;
vector_t * g_texcoord;
vector_t * g_face;

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
	// Fonction à implementer
	return true;
}

