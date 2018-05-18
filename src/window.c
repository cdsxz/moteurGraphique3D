#include "window.h"

static void WindowUpdateTexture( window_t * w ) {
	Uint32 * dst;
	int row, col;
	void * pixels;
	int pitch;
	if ( SDL_LockTexture( w->texture, NULL, &pixels, &pitch ) < 0 ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError() );
		SDL_Quit();
	}
	Uint8 * ptr = w->framebuffer;
	for ( row = 0; row < w->height; ++row ) {
		dst = (Uint32*)( (Uint8*)pixels + row * pitch );
		for ( col = 0; col < w->width; ++col ) {
			Uint8 b = *ptr++;
			Uint8 g = *ptr++;
			Uint8 r = *ptr++;
			Uint8 a = *ptr++;
			*dst++ = ( ( b << 24 ) | ( g << 16 ) | ( r << 8 ) | a );
		}
	}
	SDL_UnlockTexture( w->texture );
}

static Uint8 * WindowInitFramebuffer( window_t * w ) {
	size_t sz = w->width * w->height * w->bpp * sizeof( Uint8 );
	Uint8 * buffer = (Uint8*)malloc( sz );
	if ( buffer == NULL) return NULL;
	memset( buffer, 0, sz );
	return buffer;
}

static Uint32 * WindowInitZBuffer( window_t * w ) 
{
	size_t sz = w->width * w->height * sizeof( Uint32 );
	Uint32 * buffer = (Uint32*)malloc( sz );
	if ( buffer == NULL) return NULL;
	memset( buffer, MAX_DEEP, sz );
	return buffer;
}

window_t * WindowInit( int width, int height, int bpp ) {

	if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		printf( "SDL_Init Error: %s", SDL_GetError() );
		return NULL;
	}

	SDL_Window * sdlwindow = SDL_CreateWindow(	"Software OpenGL renderer", 
												SDL_WINDOWPOS_CENTERED, 
												SDL_WINDOWPOS_CENTERED, 
												width, height,
												SDL_WINDOW_ALLOW_HIGHDPI );

	if ( sdlwindow == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't create SDL window: %s", SDL_GetError() );
		SDL_DestroyWindow( sdlwindow );
		SDL_Quit();
		return NULL;
	}

	SDL_Renderer * renderer = SDL_CreateRenderer( sdlwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

	if ( renderer == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError() );
		SDL_DestroyRenderer( renderer );
		SDL_Quit();
		return NULL;
	}

	SDL_Texture * texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, width, height );
	
	if ( texture == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s\n", SDL_GetError() );
		SDL_DestroyTexture( texture );
		SDL_Quit();
		return NULL;
	}

	window_t * mainwindow = (window_t*)malloc( sizeof( window_t ) );

	if ( mainwindow == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't allocate window\n" );
		SDL_Quit();
		return NULL;
	}

	mainwindow->width	= width;
	mainwindow->height	= height;
	mainwindow->bpp		= bpp;
	mainwindow->pitch	= width * bpp;

	Uint8 * framebuffer = WindowInitFramebuffer( mainwindow );

	if ( framebuffer == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't allocate framebuffer: %s\n", SDL_GetError() );
		SDL_Quit();
		return NULL;
	}

	Uint32 * zBuff = WindowInitZBuffer ( mainwindow );
	
	if ( zBuff == NULL ) {
		SDL_LogError( SDL_LOG_CATEGORY_APPLICATION, "Couldn't allocate z-buffer: %s\n", SDL_GetError() );
		SDL_Quit();
		return NULL;
	}

	mainwindow->zBuffer     = zBuff;
	mainwindow->framebuffer = framebuffer;
	mainwindow->sdlwindow	= sdlwindow;
	mainwindow->renderer	= renderer;
	mainwindow->texture		= texture;

	return mainwindow;
}

void WindowDestroy( window_t * w ) {
	SDL_DestroyRenderer( w->renderer );
	SDL_DestroyTexture( w->texture );
	SDL_DestroyWindow( w->sdlwindow );
	free( w->framebuffer );
	free( w->zBuffer );
	SDL_Quit();
}

void WindowUpdate( window_t * w ) {
	SDL_RenderClear( w->renderer );
	SDL_RenderCopy( w->renderer, w->texture, NULL, NULL );
	SDL_RenderPresent( w->renderer );
	WindowUpdateTexture( w );
}

void WindowDrawPoint( window_t * w, int x, int y, int z, Uint8 r, Uint8 g, Uint8 b ) 
{
	// Fonction à implementer
	//Handle out-of-window displaying
	if (x < 0 || x > w->width || y < 0 || y > w->height)
		return;
	//Depth checks
	//Too close or too far
	if (z > MIN_DEEP || z < MAX_DEEP + (int)w->zBuffer[x + y * w->width]) return; 
	//Update Z-Buffer
	w->zBuffer[x + y * w->width] = z - MAX_DEEP;
	// Update pixel value
	w->framebuffer[(w->width - x)*4 + (w->height - 1 - y) * w->width*4] = (unsigned char) b;
	w->framebuffer[(w->width - x)*4 + (w->height - 1 - y) * w->width*4 + 1] = (unsigned char) g;
	w->framebuffer[(w->width - x)*4 + (w->height - 1 - y) * w->width*4 + 2] = (unsigned char) r;
	w->framebuffer[(w->width - x)*4 + (w->height - 1 - y) * w->width*4 + 3] = (unsigned char) 0xFF;
}

void WindowDrawClearColor( window_t * w, Uint8 r, Uint8 g, Uint8 b ) 
{
	for (int i = 0; i < w->width; i++)
		for (int j = 0; j < w->height; j++) {
			w->zBuffer[i+j*w->width] = MAX_DEEP;
			WindowDrawPoint(w, i, j, MAX_DEEP, r, g, b);
		}
}

void WindowDrawLine( window_t * w, int x0, int y0, int z0, int x1, int y1, int z1, Uint8 r, Uint8 g, Uint8 b ) 
{ 
	float coef_zx = 0, coef_zy = 0;
	float s_x = (x1 - x0) < 0 ? -1 : 1;
	float s_y = (y1 - y0) < 0 ? -1 : 1;
	float s_z = (z1 - z0) < 0 ? -1 : 1;

	// Handle limit condition
	if (x1 == x0 && y1 == y0) {
		WindowDrawPoint(w, x0, y0, (z0<z1?z1:z0), r, g, b);
		return;
	}

	if (x1 == x0) {
		coef_zy = fabs((float)(z1 - z0) / (float)(y1 - y0));
		for (int i = y0; i <= y1; i++)
			WindowDrawPoint(w, x0, i, z0 + s_z * (i-y0) * coef_zy, r, g, b);
		for (int i = y1; i <= y0; i++)
			WindowDrawPoint(w, x0, i, z0 + s_z * (i-y1) * coef_zy, r, g, b);
		return;
	}
	
	if (y1 == y0) {
		coef_zx = fabs((float)(z1 - z0) / (float)(x1 - x0));
		for (int i = x0; i <= x1 ; i++)
			WindowDrawPoint(w, i, y0, z0 + s_z * (i-x0) * coef_zx, r, g, b);
		for (int i = x1; i <= x0 ; i++)
			WindowDrawPoint(w, i, y0, z0 + s_z * (i-x1) * coef_zx, r, g, b);
		return;
	}

	// Calcul coef
	float coef = fabs((float)(y1 - y0) / (float)(x1 - x0));
	coef_zx = fabs((float)(z1 - z0) / (float)(x1 - x0));
	coef_zy = fabs((float)(z1 - z0) / (float)(y1 - y0));

	// Process
	float x = x0, y = y0, z = z0;
	while ((x != x1) && (y != y1)) {
		if (coef == 1) {
			x += s_x; 
			y += s_y;
			z += s_z * coef_zy;
		} else if (coef < 1) {
			x += s_x;
			y += s_y * coef;
			z += s_z * coef_zx;
		} else {
			y += s_y;
			x += s_x / coef;
			z += s_z * coef_zy;
		}
		WindowDrawPoint(w, (int)x, (int)y, (int)z, r, g, b);
	}
}


void WindowDrawTriangle( window_t * w, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3, Uint8 r, Uint8 g, Uint8 b, int tx1, int ty1, int tx2, int ty2, int tx3, int ty3) 
{ 
	//line sweeping
	vec3f_t* p = (vec3f_t*) malloc(3 * sizeof(vec3f_t));
	vec3f_t* n = (vec3f_t*) malloc(3 * sizeof(vec3f_t));
	vec2f_t* t = (vec2f_t*) malloc(3 * sizeof(vec2f_t));

	int p1[5], p2[5], p3[5];
	set(p1, x1, y1, z1, tx1, ty1);
	set(p2, x2, y2, z2, tx2, ty2);
	set(p3, x3, y3, z3, tx3, ty3);

	p[0] = Vec3f(x1, y1, z1);
	p[1] = Vec3f(x2, y2, z2);
	p[2] = Vec3f(x3, y3, z3);

	int top[5], mid[5], bot[5];
	set(top, 0, 0, 0, 0, 0);
	set(mid, 0, 0, 0, 0, 0);
	set(bot, 0, 0, 0, 0, 0);

	int i_top = 0, i_mid = 1, i_bot = 2;
	tri_3(i_top, i_mid, i_bot, p[0].y, p[1].y, p[2].y);
	//tri_3(top, mid, bot, p1, p2, p3);
	top[0] = p[i_top].x;
	top[1] = p[i_top].y;
	top[2] = p[i_top].z;

	mid[0] = p[i_mid].x;
	mid[1] = p[i_mid].y;
	mid[2] = p[i_mid].z;

	bot[0] = p[i_bot].x;
	bot[1] = p[i_bot].y;
	bot[2] = p[i_bot].z;

	float dx_bot_top = (float)(top[0] - bot[0]) / (float)(top[1] - bot[1]); // dx/dy * DY = DX
	float dx_bot_mid = (float)(mid[0] - bot[0]) / (float)(mid[1] - bot[1]);
	float dx_mid_top = (float)(top[0] - mid[0]) / (float)(top[1] - mid[1]);
	
	float dz_bot_top = (float)(top[2] - bot[2]) / (float)(top[1] - bot[1]); // dx/dy * DY = DX
	float dz_bot_mid = (float)(mid[2] - bot[2]) / (float)(mid[1] - bot[1]);
	float dz_mid_top = (float)(top[2] - mid[2]) / (float)(top[1] - mid[1]);

	float r_lim[2], l_lim[2];
	if (bot[1] != mid[1]) {
		r_lim[0] = bot[0]; 
		r_lim[1] = bot[2]; 
		l_lim[0] = bot[0]; 
		l_lim[1] = bot[2]; 
	} else {
		if (bot[0] < mid[0])
		{
			r_lim[0] = mid[0];
			r_lim[1] = mid[2];
			l_lim[0] = bot[0];
			l_lim[1] = bot[2];
		} else {
			r_lim[0] = bot[0];
			r_lim[1] = bot[2];
			l_lim[0] = mid[0];
			l_lim[1] = mid[2];
		}
	}
	// Drawing
	for (int i = bot[1]; i < top[1]; i++)
	{
		//WindowDrawLine(w, l_lim[0], i, l_lim[1], r_lim[0], i, r_lim[1], r, g, b);
		for (int j = l_lim[0]; j <= r_lim[0]; j++)
			WindowDrawPoint(w, j, i, (l_lim[1] + r_lim[1])/2, r, g, b);
		for (int j = r_lim[0]; j <= l_lim[0]; j++)
			WindowDrawPoint(w, j, i, (l_lim[1] + r_lim[1])/2, r, g, b);
		r_lim[0] += i < mid[1] ? dx_bot_mid : dx_mid_top;
		l_lim[0] += dx_bot_top;

		r_lim[1] += i < mid[1] ? dz_bot_mid : dz_mid_top;
		l_lim[1] += dz_bot_top;
	}
	
	delete p;
	delete n;
	delete t;
}


