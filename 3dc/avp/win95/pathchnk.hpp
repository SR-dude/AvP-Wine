#ifndef pathchnk_hpp
#define pathchnk_hpp 1

#include "chunk.hpp"
#include "chnktype.hpp"
#include "obchunk.hpp"



struct ChunkPathPoint
{
	int module_index;
	ChunkVectorInt location; //relative to world
	int flags,spare2;
};



#define PathFlag_BackAndForth 0x00000002
 
class AVP_Path_Chunk : public Chunk
{
	public :
	AVP_Path_Chunk(Chunk_With_Children*,const char *,size_t);
	AVP_Path_Chunk(Chunk_With_Children*);
	~AVP_Path_Chunk();

	virtual size_t size_chunk();
	virtual void fill_data_block(char* data_start);

	int PathLength;
	ChunkPathPoint* Path;
	
	char* PathName;
	int PathID;
	int flags,spare2;	


};

#endif
