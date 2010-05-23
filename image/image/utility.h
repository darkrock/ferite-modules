/* #include <X11/Xlib.h> */
#define X_DISPLAY_MISSING
#include <Imlib2.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ferite.h>

#define SelfImg ((Imlib_Image)(self->odata))
#define SelfPoly ((ImlibPolygon)(self->odata))
#define SelfFont ((Imlib_Font)(self->odata))
#define RAD(deg) deg*M_PI/180

	

void blend_image(Imlib_Image tar,Imlib_Image src,int x,int y);
Imlib_Image load_image(char* str);
int draw_text(Imlib_Image *img,char* str,int x,int y,Imlib_Font *font,int angle);
void free_image(Imlib_Image img);
int save_image(Imlib_Image *img, char* path, char *file_type);
Imlib_Image create_image(int x,int y);
void set_color(FeriteScript *script,FeriteObject *color_object);
int set_image(FeriteScript *script, Imlib_Image img);

#if 0
Imlib_Image bump_map(Imlib_Image im, int angle, int elevation, int depth, int ired, int igreen, int iblue, int iambient );
Imlib_Image bump_map_point(Imlib_Image im, int ix, int iy, int iz, int depth, int ired, int igreen, int iblue, int iambient );
#endif
