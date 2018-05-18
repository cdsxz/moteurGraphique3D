#include "geometry.h"

matrixf_t Matrixf( int n, int m ) {
	float ** a = (float **)malloc( sizeof( float * ) * n );
	if ( a ) {
		for ( int i = 0; i < n; i++ ) {
            		a[ i ] = (float*)malloc( sizeof( float ) * m );
            		if ( a[ i ] == NULL ) {
                		return NULL;
            		}
        	}
    	}
    	for ( int i = 0; i < n; i++ ) {
        	for ( int j = 0; j < m; j++ ) {
            		a[ i ][ j ] = 0.0f;
        	}
    	}
	return a;
}

void MatrixfDelete( matrixf_t m, int n ) {
	for ( int i = 0; i < n; i++) {
		free( m[ i ] );
	}
    	free( m );
}

matrixf_t MatrixfIdentity( int n ) {
    	matrixf_t m = Matrixf( n, n );
    	for ( int i = 0; i < n; i++) {
        	for ( int j = 0; j < n ; j++) {
            	m[ i ][ j ] = ( i == j ? 1.0f : 0.0f );
        	}
    	}
    	return m;
}

matrixf_t MatrixfMult( matrixf_t a, matrixf_t b, int n, int m ) {
    	matrixf_t mtx = Matrixf( n, m );
    	for ( int i = 0; i < n; i++ ) {
        	for ( int j = 0; j < m; j++ ) {
            	mtx[ i ][ j ] = 0.0f;
            	for ( int k = 0; k < n; k++ ) {
                	mtx[ i ][ j ] += a[ i ][ k ] * b[ k ][ j ];
            	}
        	}
    	}
    	return mtx;
}

matrixf_t	MatrixfMultScal	( matrixf_t a, int b, int n, int m ) {
    	matrixf_t mtx = Matrixf( n, m );
    	for ( int i = 0; i < n; i++ ) {
        	for ( int j = 0; j < m; j++ ) {
            	mtx[ i ][ j ] = 0.0f;
            	for ( int k = 0; k < n; k++ ) {
                	mtx[ i ][ j ] += a[ i ][ k ] * b;
            	}
        	}
    	}
    	return mtx;
}


matrixf_t MatrixfViewport( int x, int y, int w, int h ) {
	// Fonction à implementer
    	matrixf_t m = MatrixfIdentity( 4 );
    	return m;
}

matrixf_t MatrixfLookAt( vec3f_t eye, vec3f_t center, vec3f_t up ) {
	// Fonction à implementer
    	matrixf_t r = MatrixfIdentity( 4 );
    	return r;
}

void set(int t[5], int x, int y, int z, int tx, int ty) 
{
  t[0] = x;
  t[1] = y;
  t[2] = z;
  t[3] = tx;
  t[4] = ty;
}

void cp(int dest[5], int src[5])
{
  dest[0] = src[0];
  dest[1] = src[1];
  dest[2] = src[2];
  dest[3] = src[3];
  dest[4] = src[4];
}

void cp(int* dest, int* src, int n)
{
  for (int i = 0; i < n; ++i)
    dest[i] = src[i]; 
}

void tri_3(int top[5], int mid[5], int bot[5], int a[5], int b[5], int c[5])
{
  if (a[1] > b[1])
    if (a[1] > c[1]) {
      cp(top,a);
      if (b[1] > c[1]) {
        cp(mid,b);
        cp(bot,c);
      } else {
        cp(mid,c); 
        cp(bot,b);
      }
    } else {
      cp(top,c);
      if (a[1] > b[1])
      {
        cp(mid,a);
        cp(bot,b);
      } else {
        cp(mid,b);
        cp(bot,a);
      }
    }
  else if (b[1] > c[1]) {
    cp(top,b);
    if (a[1] > c[1])
    {
      cp(mid,a);
      cp(bot,c);
    } else {
      cp(mid,c);
      cp(bot,a);
    }
  } else {
    cp(top,c);
    if (a[1] > b[1])
    {
      cp(mid,a);
      cp(bot,b);
    } else {
      cp(mid,b);
      cp(bot,a);
    }
  }
}

void tri_3(int &top, int &mid, int &bot, float a, float b, float c)
{
  if (a > b)
    if (a > c) {
      top = 0;
      if (b > c) {
        mid = 1;
        bot = 2;
      } else {
        mid = 2; 
        bot = 1;
      }
    } else {
      top = 2;
      if (a > b)
      {
        mid = 0;
        bot = 1;
      } else {
        mid = 1;
        bot = 0;
      }
    }
  else if (b > c) {
    top = 1;
    if (a > c)
    {
      mid = 0;
      bot = 2;
    } else {
      mid = 2;
      bot = 0;
    }
  } else {
    top = 2;
    if (a > b)
    {
      mid = 0;
      bot = 1;
    } else {
      mid = 1;
      bot = 0;
    }
  }
}
