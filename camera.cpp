/*
camera.cpp
method function definitions

wrblack
*/
#include "ray.h"

//default constructor
camera_t::camera_t() {}

//overloaded constructor
camera_t::camera_t(FILE *in)
{
	char buf[256];
	int mask;
	
	assert( fscanf(in, "%s", name) == 1);
	
	fscanf(in, "%s", buf);
	assert(buf[0] == '{');
	
	cookie = CAM_COOKIE;
	
	camera_parse[0].loc = &pixel_dim;
	camera_parse[1].loc = &world_dim;
	camera_parse[2].loc = &view_point;
	
	mask = parser(in, camera_parse, NUM_ATTRS, 0);
	assert(mask == 7);
	
	//allocate a pixmap to hold the ppm image data
	pixmap = (irgb_t *)malloc(sizeof(irgb_t) * pixel_dim[0] * pixel_dim[1]);
}

//might want to move getters/setters together and position them
//after constructors for easier reading

void camera_t::getdir(int x, int y, vec_t *dir)
{
	assert(cookie == CAM_COOKIE);
	
	vec_t *world = (vec_t *)malloc(sizeof(vec_t));
	
	world->x = (double) x / (pixel_dim[X] - 1) * (world_dim[X]);
	world->y = (double) y / (pixel_dim[Y] - 1) * (world_dim[Y]);
	world->z = 0;
	
	vec_diff(view_point, world, dir);
	assert(world->z < 0);
	
	vec_unit(dir, dir);
}

void camera_t::store_pixel(int x, int y, drgb_t *pix)
{
	int maprow;
	irgb_t *maploc;
	
	maprow = pixel_dim[1] - y - 1;
	maploc = pixmap + maprow * pixel_dim[0] + x;
	
	scale_and_clamp(pix);
	
	maploc->r = (unsigned char)pix->r;
	maploc->g = (unsigned char)pix->g;
	maploc->b = (unsigned char)pix->b;
}

int camera_t::getxdim(void)
{
	return( pixel_dim[0] );
}

int camera_t::getydim(void)
{
	return( pixel_dim[1] );
}

void camera_t::getviewpt(vec_t *view)
{
	//
}

void camera_t::camera_print(FILE *out)
{
	assert(cookie == CAM_COOKIE);
	fprintf(out, "camera %11s\n", name);

	fprintf(out, "pixeldim %9d %5d\n", pixel_dim[X], pixel_dim[Y]);

	fprintf(out, "worlddim  %8.1f %5.1f\n", world_dim[X], world_dim[Y]);

	fprintf(out, "viewpoint  %7.1f %5.1f %5.1f\n", 
		view_point.x, view_point.y, view_point.z);	
}

void camera_t::camera_write_image(FILE *out)
{
	assert(cam->cookie == CAM_COOKIE);

	int w = pixel_dim[X];
	int h = pixel_dim[Y];

	//ppm header
	fprintf(out, "P6\n");
	
	//window length x height x highest pixel intensity
	//fprintf(out, "%d %d 255\n", cam->pixel_dim[X], cam->pixel_dim[Y]);
	fprintf(out, "%d %d 255\n", w, h);
	
	//fwrite(cam->pixmap, sizeof(irgb_t), 
	//	cam->pixel_dim[X] * cam->pixel_dim[Y], out);
	fwrite(cam->pixmap, sizeof(irgb_t), w * h, out);
}

//need to ask about this one
void camera_t::getpixsize(double *x, double *y)
{
	
}


/* load if using old .c parser

int camera_t::camera_load_attributes(FILE *in)
{
	char attrib_name[16];
	int count = 0;
	int attrcount = 0;
	
	//eat attribs
	fscanf(in, "%s", attrib_name);
	while (attrib_name[0] != '}') {
	
		if ( strcmp( attrib_name, "pixeldim" ) == 0) {
		
			count = fscanf(in, "%d %d", &pixel_dim[X], &pixel_dim[Y]);
			assert(count == 2);
			attrcount += 1;
			
		} else if ( strcmp( attrib_name, "worlddim" ) == 0) {
		
			count = fscanf(in, "%lf %lf", &world_dim[X], &world_dim[Y]);
			assert(count == 2);
			attrcount += 1;
		} else if (strcmp(attrib_name, "viewpoint") == 0) {
			count = fscanf(in, "%lf %lf %lf", 
				&view_point.x, &view_point.y, &view_point.z);
			assert(count == 3);
			attrcount += 1;
		}
		else
		{
			fprintf(stderr, "Bad camera attribute: %s \n", attrib_name);
			exit(1);
		}	

		fscanf(in, "%s", attrib_name);
	}
	return attrcount;
}
*/