#ifndef _zsp_hpp
#define _zsp_hpp 1

#include "shpchunk.hpp"


// this file should be included from chunk.hpp


struct ChunkShape;

struct ZSP_zone
{

	ZSP_zone ();
	~ZSP_zone();
	
	ZSP_zone (const ZSP_zone &);
	ZSP_zone & operator=(const ZSP_zone &);
	
	int num_z_polys;
	int * z_poly_list;
	int num_z_verts;
	int * z_vert_list;
	
	friend unsigned char operator==(const ZSP_zone &, const ZSP_zone &);
	friend unsigned char operator!=(const ZSP_zone &, const ZSP_zone &);
};


class ZSP_Data
{

public:


	ZSP_Data (const char * zdata, size_t zsize);

	~ZSP_Data ();

	double cube_size;
	double cube_radius;
	
	int num_x_cubes, num_y_cubes, num_z_cubes;

	List<ZSP_zone> zone_array;
	
	
};


/////////////////////////////////////////

class Shape_ZSP_Data_Chunk : public Chunk
{
public:


	const ZSP_Data zspdata;

	size_t size_chunk ();
	void fill_data_block (char *);

	Shape_ZSP_Data_Chunk (Shape_Sub_Shape_Chunk * parent, const char * zdata, size_t zsize)
	: Chunk (parent, "SHPZSPDT"), zspdata	(zdata, zsize)
	{}

	Shape_ZSP_Data_Chunk (Shape_Chunk * parent, const char * zdata, size_t zsize)
	: Chunk (parent, "SHPZSPDT"), zspdata	(zdata, zsize)
	{}
	
};



#endif
