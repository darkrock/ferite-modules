#include "utility.h"

int set_image(FeriteScript *script, Imlib_Image img){
	if( img == NULL ) {
		ferite_error( script, 0, "Image object is NULL");
		return -1;
	}
	
	imlib_context_set_image( img );
	return 0;
}



void set_color(FeriteScript *script,FeriteObject *color_object){
	char *vars[]={"r","g","b","a"};
	int val[4];
	int x;
	FeriteVariable *col;

	
	if( color_object == NULL ) //use the color in context
		return;
	
	for(x=0;x<4;x++) {
		col=ferite_object_get_var(script, color_object,vars[x]);
		if( col == NULL )
			ferite_error(script, 0, "error geting member in class color\n");
		val[x]=VAI(col);
	}
	col=ferite_object_get_var(script,color_object,"r");
	//printf(" setting color (%d,%d,%d,%d)\n", val[0],val[1],val[2],val[3]);
	imlib_context_set_color(val[0],val[1],val[2],val[3]);
}

Imlib_Image create_image(int x,int y){
	Imlib_Image pic;
	
	pic = imlib_create_image(x,y);
	imlib_context_set_image(pic);
	imlib_image_set_has_alpha(1);
	return pic;
}


int save_image(Imlib_Image *img, char* path, char *file_type) {
	Imlib_Load_Error imlib_error;
	char *ptr;
	
	if( file_type == NULL ) {
		file_type =  strrchr(path,'.') + 1;
	        if( *file_type == '\0' )
			return 1;
	}
	
	imlib_context_set_image(img);
	imlib_image_set_format(file_type);
	imlib_save_image_with_error_return(path,&imlib_error);
	if(imlib_error)
		return 1;
	return 0;
}


void free_image(Imlib_Image img){
        if( img )
		imlib_context_set_image(img);
	else
		return;
   imlib_free_image_and_decache();
}


 int draw_text(Imlib_Image *img,char* str,int x,int y,Imlib_Font *font,int angle){
	 int w,h,th,tw;


	 imlib_context_set_font(font);
	 imlib_context_set_image(img);
	 //imlib_context_set_anti_alias(0);
	 //Center ?
	 if(x==-1 || y==-1){
		 imlib_get_text_size(str,&tw,&th);
		 h=imlib_image_get_height();
		 w=imlib_image_get_width();
		 if(x==-1)
			 x=(w-tw)/2;
		 if(y==-1)
			 y=(h-th)/2;
	 }
	   
	 if(angle){
		 imlib_context_set_direction(IMLIB_TEXT_TO_ANGLE);
		 imlib_context_set_angle(M_PI*angle/180);
	 }
	 imlib_text_draw(x,y,str);
	 
	 //restore setting
	 if(angle)
		 imlib_context_set_direction(IMLIB_TEXT_TO_RIGHT);
	 return 0;
 }




Imlib_Image load_image(char* str){
 	//return imlib_load_image(str);
 	return imlib_load_image_without_cache(str);
 }

// /* void _decaf_image_fill_gradient(Imlib_Image img,struct decaf_color* c1,struct decaf_color* c2){ */
// /*   Imlib_Color_Range range; */
// /*   Imlib_Image old_img; */
  
// /*   if(img){ */
// /*     old_img=imlib_context_get_image(); */
// /*     imlib_context_set_image(img);  */
// /*     if(!(imlib_context_get_image())) */
// /*       DECAFerror("load image");     */
// /*   } */


// /*   range = imlib_create_color_range(); */
// /*   imlib_context_set_color_range(range); */
// /*   imlib_context_set_color(c1->r, c1->g, c1->b, c1->a); */
// /*   imlib_add_color_to_color_range(0); */
// /*   imlib_context_set_color(c2->r, c2->g, c2->b, c2->a);                       */
// /*   imlib_add_color_to_color_range(imlib_image_get_width()); */
// /*   imlib_image_fill_color_range_rectangle(0,0,imlib_image_get_width(),imlib_image_get_height(),90); */
  
// /*   imlib_free_color_range(); */
   
// /*   if(img && img!=old_img) */
// /*     imlib_context_set_image(old_img); */
// /* } */



void blend_image(Imlib_Image tar,Imlib_Image src,int x,int y) {
	int xsrc,ysrc,ytar,xtar,w,h;
	int merge = 0;

	imlib_context_set_blend(1);
	imlib_context_set_image(src);
	xsrc = imlib_image_get_width();
	ysrc = imlib_image_get_height();
	
	imlib_context_set_image(tar);
	xtar = imlib_image_get_width() - x;
	ytar = imlib_image_get_height() - y;
	w= (xsrc<xtar) ? xsrc:xtar;
	h= (ysrc<ytar) ? ysrc:ytar;	 
	printf("Width: %d, Height: %d\n", w, h );
	imlib_blend_image_onto_image( src, merge, 0, 0, w, h, x, y, w, h);
}

#define PI (4 * atan(1))

#if 0

#include "image.h"
#include "colormod.h"
#include "blend.h"

Imlib_Image bump_map(Imlib_Image im, int angle, int elevation, int depth, int ired, int igreen, int iblue, int iambient )
{
   Imlib_Image         map = im;
   double              an = 0, el = 30, d = 0x200;
   double              red = 0x200, green = 0x200, blue = 0x200;
   double              ambient = 0;

   int                 free_map = 0;
   DATA32             *src;
   DATA32             *mp, *mpy, *mpp;
   double              z, z_2, x2, y2;
   int                 w, h, i, j, w2, h2, wh2, mx, my;

	if( angle > -1 )
		an = angle;
	if( elevation > -1 ) 
		el = elevation;
	if( depth > -1 ) 
		d = depth;
	if( ired > -1 )
		red = ired;
	if( igreen > -1 ) 
		green = igreen;
	if( iblue > -1 )
		blue = iblue;
	if( iambient > -1 )
	 	ambient = iambient;

   if (!map)
      return im;

   red /= 0x100;
   green /= 0x100;
   blue /= 0x100;
   ambient /= 0x100;
   d /= 0x100;

   imlib_context_set_image(im);
   src = imlib_image_get_data();
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   imlib_context_set_image(map);
   mpp = imlib_image_get_data_for_reading_only();
   w2 = imlib_image_get_width();
   h2 = imlib_image_get_height();
   wh2 = w2 * h2;

   an *= (PI / 180);
   el *= (PI / 180);

   x2 = sin(an) * cos(el);
   y2 = cos(an) * cos(el);
   z = sin(el);

   d /= (255 * (255 + 255 + 255));
   z_2 = z * z;

   my = h2;
   for (j = h; --j >= 0;)
     {
        mp = mpp;
        mpp += w2;
        if (--my <= 0)
          {
             mpp -= wh2;
             my = h2;
          }
        mpy = mpp;
        mx = w2;
        for (i = w; --i >= 0;)
          {
             double              x1, y1, v;
             int                 r, g, b, gr;

             gr = A_VAL(mp) * (R_VAL(mp) + G_VAL(mp) + B_VAL(mp));
             y1 = d * (double)(A_VAL(mpy) * (R_VAL(mpy) +
                                             G_VAL(mpy) + B_VAL(mpy)) - gr);
             mp++;
             mpy++;
             if (--mx <= 0)
               {
                  mp -= w2;
                  mpy -= w2;
                  mx = w2;
               }
             x1 = d * (double)(A_VAL(mp) * (R_VAL(mp) +
                                            G_VAL(mp) + B_VAL(mp)) - gr);
             v = x1 * x2 + y1 * y2 + z;
             v /= sqrt((x1 * x1) + (y1 * y1) + 1.0);
             v += ambient;
             r = v * R_VAL(src) * red;
             g = v * G_VAL(src) * green;
             b = v * B_VAL(src) * blue;
             if (r < 0)
                r = 0;
             if (r > 255)
                r = 255;
             if (g < 0)
                g = 0;
             if (g > 255)
                g = 255;
             if (b < 0)
                b = 0;
             if (b > 255)
                b = 255;
			WRITE_RGB(src, r, g, b);

             src++;
          }
     }
   if (free_map)
     {
        imlib_context_set_image(map);
        imlib_free_image();
     }
   return im;
}

Imlib_Image
bump_map_point(Imlib_Image im, int ix, int iy, int iz, int depth, int ired, int igreen, int iblue, int iambient )
{
   Imlib_Image         map = im;
   double              x = 0, y = 0, z = 30, d = 0x200;
   double              red = 0x200, green = 0x200, blue = 0x200;
   double              ambient = 0;

   int                 free_map = 0;
   DATA32             *src;
   DATA32             *mp, *mpy, *mpp;
   double              z_2, x2, y2;
   int                 w, h, i, j, w2, h2, wh2, mx, my;

	if( ix > -1 ) 
		x = ix;
	if( iy > -1 )
		y = iy;
	if( iz > -1 )
		z = iz;
	if( depth > -1 ) 
		d = depth;
	if( ired > -1 )
		red = ired;
	if( igreen > -1 ) 
		green = igreen;
	if( iblue > -1 )
		blue = iblue;
	if( iambient > -1 )
	 	ambient = iambient;
	
   if (!map)
      return im;

   red /= 0x100;
   green /= 0x100;
   blue /= 0x100;
   ambient /= 0x100;
   d /= 0x100;

   imlib_context_set_image(im);
   src = imlib_image_get_data();
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   imlib_context_set_image(map);
   mpp = imlib_image_get_data_for_reading_only();
   w2 = imlib_image_get_width();
   h2 = imlib_image_get_height();
   wh2 = w2 * h2;

   d /= (255 * (255 + 255 + 255));
   z_2 = z * z;

   my = h2;
   y2 = -y;
   for (j = h; --j >= 0;)
     {
        mp = mpp;
        mpp += w2;
        if (--my <= 0)
          {
             mpp -= wh2;
             my = h2;
          }
        mpy = mpp;
        mx = w2;
        x2 = -x;
        for (i = w; --i >= 0;)
          {
             double              x1, y1, v;
             int                 r, g, b, gr;

             gr = A_VAL(mp) * (R_VAL(mp) + G_VAL(mp) + B_VAL(mp));
             y1 = d * (double)(A_VAL(mpy) * (R_VAL(mpy) +
                                             G_VAL(mpy) + B_VAL(mpy)) - gr);
             mp++;
             mpy++;
             if (--mx <= 0)
               {
                  mp -= w2;
                  mpy -= w2;
                  mx = w2;
               }
             x1 = d * (double)(A_VAL(mp) * (R_VAL(mp) +
                                            G_VAL(mp) + B_VAL(mp)) - gr);
             v = x1 * x2 + y1 * y2 + z;
             v /= sqrt((x1 * x1) + (y1 * y1) + 1.0);
             v /= sqrt((x2 * x2) + (y2 * y2) + z_2);
             v += ambient;
             r = v * R_VAL(src) * red;
             g = v * G_VAL(src) * green;
             b = v * B_VAL(src) * blue;
             if (r < 0)
                r = 0;
             if (r > 255)
                r = 255;
             if (g < 0)
                g = 0;
             if (g > 255)
                g = 255;
             if (b < 0)
                b = 0;
             if (b > 255)
                b = 255;
			 WRITE_RGB(src, r, g, b);

             x2++;
             src++;
          }
        y2++;
     }
   if (free_map)
     {
        imlib_context_set_image(map);
        imlib_free_image();
     }
   return im;
}

#endif