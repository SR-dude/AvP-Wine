#include "chunk.hpp"
#include "chnktype.hpp"
#include "mishchnk.hpp"
#include "shpchunk.hpp"
#include "obchunk.hpp"
#include "huffman.hpp"
#include <mbstring.h>

extern char * users_name;


//macro for helping to force inclusion of chunks when using libraries
FORCE_CHUNK_INCLUDE_IMPLEMENT(mishchnk)

BOOL Lockable_Chunk_With_Children::lock_chunk(File_Chunk & fchunk)
{
	if (!fchunk.filename) return FALSE;

	if (local_lock) return FALSE; // you can't lock a chunk twice

	if(external_lock) return FALSE;
	local_lock = TRUE;
	set_lock_user (users_name);
	return TRUE;

}	

BOOL Lockable_Chunk_With_Children::unlock_chunk (File_Chunk & fchunk, BOOL updateyn)
{
	if (updateyn)	{
		updated = TRUE;
	}
	local_lock = FALSE;
	(void)fchunk;

	return TRUE;

}


BOOL Lockable_Chunk_With_Children::update_chunk_in_file(HANDLE &rif_file)
{

	unsigned long bytes_read;
	int length = 0;

	const char * hd_id = get_head_id();

	if (!hd_id) return FALSE;

	SetFilePointer (rif_file,0,0,FILE_BEGIN);


	List<int> shpfptrs;
	list_chunks_in_file(& shpfptrs, rif_file, identifier);

	// look through chunks for the save of our current chunk


	LIF<int> sfpl(&shpfptrs);

	if (shpfptrs.size()) {
		for (; !sfpl.done(); sfpl.next()) {

			SetFilePointer (rif_file, sfpl()+8,0,FILE_BEGIN);

			ReadFile (rif_file, (long *) &(length), 4, &bytes_read, 0);

			SetFilePointer (rif_file, sfpl(),0,FILE_BEGIN);
			if (file_equals(rif_file)) break;
		}
	}

	// then load the file after that chunk into a buffer,
	// output the chunk and write the buffer
	// unless the chunk is the last one in the file


	if (!sfpl.done())
	{

		int file_length = GetFileSize(rif_file,0);

		if (file_length > (sfpl() + length)) {
							
			SetFilePointer (rif_file, sfpl() + length,0,FILE_BEGIN);

			char * tempbuffer;
			tempbuffer = new char [file_length - (sfpl() + length)];

			ReadFile (rif_file, (long *) tempbuffer, (file_length - (sfpl() + length)), &bytes_read, 0);

			SetFilePointer (rif_file, sfpl() ,0,FILE_BEGIN);
			
			if (!deleted)
			{
				size_chunk();
				output_chunk(rif_file);
			}
			
			WriteFile (rif_file, (long *) tempbuffer, (file_length - (sfpl() + length)), &bytes_read, 0);

			delete [] tempbuffer;

			SetEndOfFile (rif_file);
		}
		else{

			SetFilePointer (rif_file, sfpl() ,0,FILE_BEGIN);
			if (!deleted)
			{
				size_chunk();
				output_chunk(rif_file);
			}
			SetEndOfFile (rif_file);
		}

	}
	else {

		SetFilePointer (rif_file,0 ,0,FILE_END);
		if (!deleted)
		{
			size_chunk();
			output_chunk(rif_file);
		}
		SetEndOfFile (rif_file);
	}

	local_lock = FALSE;

	updated = FALSE;
	
	int file_length = GetFileSize(rif_file,0);
	SetFilePointer (rif_file,8,0,FILE_BEGIN);

	WriteFile (rif_file, (long *) &file_length, 4, &bytes_read, 0);

	
	// DO NOT PUT ANY CODE AFTER THIS
	
	if (deleted) delete this;
	
	// OR ELSE !!!
	
	return TRUE;

}


size_t Lockable_Chunk_With_Children::size_chunk_for_process()
{
	if (output_chunk_for_process)
		return size_chunk();
	return(chunk_size = 0);
}


void Lockable_Chunk_With_Children::fill_data_block_for_process(char * data_start)
{
	if (output_chunk_for_process)
	{
		fill_data_block(data_start);
		output_chunk_for_process = FALSE;
	}
}




///////////////////////////////////////

// Class File_Chunk functions

/*
Children for File_Chunk :
"REBSHAPE"		Shape_Chunk
"RSPRITES"		AllSprites_Chunk
"RBOBJECT"		Object_Chunk
"RIFVERIN"		RIF_Version_Num_Chunk
"REBENVDT"		Environment_Data_Chunk
"REBENUMS"		Enum_Chunk
"OBJCHIER"		Object_Hierarchy_Chunk
"OBHALTSH"		Object_Hierarchy_Alternate_Shape_Set_Chunk
"HIDEGDIS"		Hierarchy_Degradation_Distance_Chunk
"INDSOUND"		Indexed_Sound_Chunk
"HSETCOLL"		Hierarchy_Shape_Set_Collection_Chunk
"DUMMYOBJ"		Dummy_Object_Chunk

*/

File_Chunk::File_Chunk(const char * file_name)
: Chunk_With_Children (NULL, "REBINFF2")
{
// Load in whole chunk and traverse
	char rifIsCompressed = FALSE;
	char *uncompressedData = NULL;
	HANDLE rif_file;
	DWORD file_size;
	DWORD file_size_from_file;
	unsigned long bytes_read;
	char * buffer;
	char * buffer_ptr;
	char id_buffer[9];

	Parent_File = this;

	error_code = 0;
	object_array_size=0;
	object_array=0;

	filename = new char [strlen(file_name) + 1];

	strcpy (filename, file_name);

	rif_file = CreateFileA (file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, 
					FILE_FLAG_RANDOM_ACCESS, 0);

	if (rif_file == INVALID_HANDLE_VALUE) {
		error_code = CHUNK_FAILED_ON_LOAD;
		return;
	}

	file_size = GetFileSize (rif_file, NULL);	

	
	if (!ReadFile(rif_file, id_buffer, 8, &bytes_read, 0)) {
		error_code = CHUNK_FAILED_ON_LOAD;
		CloseHandle (rif_file);
		return;
	}	

	/* KJL 16:46:14 19/09/98 - check for a compressed rif */
	if (!strncmp (id_buffer, COMPRESSED_RIF_IDENTIFIER, 8))
	{
		rifIsCompressed = TRUE;
	}	
	else if (strncmp (id_buffer, "REBINFF2", 8))
	{
		error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
		CloseHandle (rif_file);
		return;
	}	
	buffer = new char [file_size];

	/* KJL 17:57:44 19/09/98 - if the rif is compressed, we must load the whole
	file in and then pass it to the decompression routine, which will return a
	pointer to the original data. */
	if (rifIsCompressed)
	{
		if (!ReadFile(rif_file, buffer+8, (file_size-8), &bytes_read, 0))
		{
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			return;
		}	
		uncompressedData = HuffmanDecompress((HuffmanPackage*)buffer); 		
		file_size = ((HuffmanPackage*)buffer)->UncompressedDataSize;
		
		delete [] buffer; // kill the buffer the compressed file was loaded into

		buffer_ptr = buffer = uncompressedData+12; // skip header data
	}
	else // the normal uncompressed approach:
	{
		if (!ReadFile(rif_file, &file_size_from_file, 4, &bytes_read, 0)) {
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			return;
		}	

		if (file_size != file_size_from_file) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			CloseHandle (rif_file);
			return;
		}	

		if (!ReadFile(rif_file, buffer, (file_size-12), &bytes_read, 0))
		{
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			return;
		}
		buffer_ptr = buffer;
	}

	// Process the RIF
	// The start of the first chunk

	while ((buffer_ptr-buffer)< ((signed) file_size-12) && !error_code) {

		if ((*(int *)(buffer_ptr + 8)) + (buffer_ptr-buffer) > ((signed)file_size-12)) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		DynCreate(buffer_ptr);
		buffer_ptr += *(int *)(buffer_ptr + 8);

	}

	/* KJL 17:59:42 19/09/98 - release the memory holding the rif */
	if (rifIsCompressed)
	{
		free(uncompressedData);
	}
	else
	{
		delete [] buffer;
	}

	CloseHandle (rif_file);

	post_input_processing();

}

File_Chunk::File_Chunk()
: Chunk_With_Children (NULL, "REBINFF2")
{
// Empty File chunk
	new RIF_Version_Num_Chunk (this);
	filename = 0;

	object_array_size=0;
	object_array=0;
}


File_Chunk::~File_Chunk()
{
	if (filename)
		delete [] filename;

	if(object_array)
		free(object_array);
}


BOOL File_Chunk::write_file (const char * fname)
{
	if(!fname) return FALSE;
	//if a read_only file exists with this filename , then abort attempt to save
	DWORD attributes = GetFileAttributesA(fname);
	if (0xffffffff!=attributes)
	{
		if (attributes & FILE_ATTRIBUTE_READONLY)
		{
			return FALSE;
		}
	}
	
	
	
	HANDLE rif_file;

	if (filename) delete [] filename;

	filename = new char [strlen(fname) + 1];
	strcpy (filename, fname);
		
	//save under a temporary name in case a crash occurs during save;
	int filename_start_pos=0;
	int pos=0;
	while(fname[pos])
	{
		if(fname[pos]=='\\' || fname[pos]==':')
		{
			filename_start_pos=pos+1;
		}
		//go to next MBCS character in string
		pos+=_mbclen((unsigned const char*)&fname[pos]);
	}
	if(!fname[filename_start_pos]) return FALSE;
	
	char* temp_name=new char[strlen(fname)+7];
	strcpy(temp_name,fname);
	strcpy(&temp_name[filename_start_pos],"~temp~");
	strcpy(&temp_name[filename_start_pos+6],&fname[filename_start_pos]);

	prepare_for_output();

	
	//create a block containing the uncompressed rif file
	unsigned char* uncompressedData = (unsigned char*) make_data_block_from_chunk();
	if(!uncompressedData) return FALSE;

	//do the compression thing
	HuffmanPackage *outPackage = HuffmanCompression(uncompressedData,chunk_size);
	delete [] uncompressedData;
	if(!outPackage) return FALSE;

	//and now try to write the file
	rif_file = CreateFileA (temp_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 
					FILE_FLAG_RANDOM_ACCESS, 0);

	if (rif_file == INVALID_HANDLE_VALUE) {
		delete [] temp_name;
		free (outPackage);
		return FALSE;
	}
		
	unsigned long junk;
	BOOL ok;

	ok = WriteFile (rif_file, (long *) outPackage,outPackage->CompressedDataSize+sizeof(HuffmanPackage), &junk, 0);

	CloseHandle (rif_file);

	free (outPackage);

	if(!ok) return FALSE;
	
	

	//Delete the old file with this name (if it exists) , and rename the temprary file
	DeleteFileA(fname);
	MoveFileA(temp_name,fname);

	delete [] temp_name;

	return TRUE;
}	

// the file_chunk must link all of its shapes & objects together

void File_Chunk::post_input_processing()
{
	List<Shape_Chunk *> shplist;
	List<Object_Chunk *> objlist;

	List<Chunk *> child_lists;

	lookup_child("REBSHAPE",child_lists);

	while (child_lists.size()) {
		shplist.add_entry((Shape_Chunk *)child_lists.first_entry());
		child_lists.delete_first_entry();
	}

	lookup_child("RBOBJECT",child_lists);

	while (child_lists.size()) {
		objlist.add_entry((Object_Chunk *)child_lists.first_entry());
		child_lists.delete_first_entry();
	}

	for (LIF<Shape_Chunk *> sli(&shplist); !sli.done(); sli.next())
	{
		Shape_Chunk::max_id = max (Shape_Chunk::max_id,sli()->get_header()->file_id_num);	
	}
	Shape_Chunk** shape_array=new Shape_Chunk*[Shape_Chunk::max_id+1];

	for(sli.restart();!sli.done();sli.next())
	{
		shape_array[sli()->get_header()->file_id_num]=sli();
	}
	
	for (LIF<Object_Chunk *> ol(&objlist); !ol.done(); ol.next())
	{
		ol()->assoc_with_shape(shape_array[ol()->get_header()->shape_id_no]);
	}

	delete shape_array;
	

	Chunk_With_Children::post_input_processing();	
}



BOOL File_Chunk::check_file()
{
	if (!filename) return TRUE;
	return(TRUE);
// adj 
}



extern File_Chunk * Env_Chunk;
	

	
BOOL File_Chunk::update_file()
{

	if (!filename) return FALSE;

	char tempname [256];
	strcpy (tempname, filename);

	List<Shape_Chunk *> slist;
	list_shapes(&slist);
	List<Object_Chunk *> olist;
	list_objects(&olist);
	
	for (LIF<Shape_Chunk *> sli(&slist); !sli.done(); sli.next())
	{
		if (sli()->deleted)
		{
			delete sli();
		}
	}
	
	for (LIF<Object_Chunk *> oli(&olist); !oli.done(); oli.next())
	{
		if (oli()->deleted)
		{
			delete oli();
		}
	}
	
	
	return(write_file(tempname));
	

}

BOOL File_Chunk::update_chunks_from_file()
{
	return(TRUE);
	
// adj Deleted unreachable code

}

void File_Chunk::list_objects(List<Object_Chunk *> * pList)
{
	Chunk * child_ptr = children;
	
	while (pList->size())
		pList->delete_first_entry();

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("RBOBJECT", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				pList->add_entry((Object_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

}

void File_Chunk::list_shapes(List<Shape_Chunk *> * pList)
{
	Chunk * child_ptr = children;

	while (pList->size())
		pList->delete_first_entry();

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("REBSHAPE", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				pList->add_entry((Shape_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

}

void File_Chunk::list_dummy_objects(List<Dummy_Object_Chunk *> * pList){
	Chunk * child_ptr = children;
	
	while (pList->size())
		pList->delete_first_entry();

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("DUMMYOBJ", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				pList->add_entry((Dummy_Object_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

}

Environment_Data_Chunk * File_Chunk::get_env_data()
{
	List<Environment_Data_Chunk *> e_list;
	Chunk * child_ptr = children;

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("REBENVDT", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				e_list.add_entry((Environment_Data_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

	// There can be only ONE.
	assert (e_list.size() < 2);
	
	if (e_list.size())
		return e_list.first_entry();
	else
	{
		return(0);
	}	
}

void File_Chunk::build_object_array()
{
	List<Object_Chunk*> oblist;
	list_objects(&oblist);

	if(object_array)
	{
		free(object_array);
		object_array=0;
	}	
	object_array_size=0;

	LIF<Object_Chunk*> oblif(&oblist);

	//find the highest object index
	for(oblif.restart();!oblif.done();oblif.next())
	{
		object_array_size=max(object_array_size,oblif()->object_data.index_num+1);
	}

	if(object_array_size<=0) return;

	object_array = (Object_Chunk**) malloc(sizeof(Object_Chunk*)*object_array_size);
	for(int i=0;i<object_array_size;i++)
	{
		object_array[i]=0;
	}

	//now fill in object array

	for(oblif.restart();!oblif.done();oblif.next())
	{
		int index=oblif()->object_data.index_num;
		if(index>=0)
		{
			object_array[index]=oblif();
		}
	}
}

Object_Chunk* File_Chunk::get_object_by_index(int index)
{
	if(!object_array) build_object_array();
	if(index<0 || index>=object_array_size)return 0;
	return object_array[index];
}

void File_Chunk::assign_index_to_object(Object_Chunk* object)
{
	assert(object);

	if(!object_array) build_object_array();
	//see if there is a free index

	for(int i=0;i<object_array_size;i++)
	{
		if(!object_array[i])
		{
			object->object_data_store->index_num=i;
			object_array[i]=object;
			return;
		}
	}
	
	//add a new entry on the end of the array
	object_array_size++;
		
	object_array=(Object_Chunk**) realloc(object_array,sizeof(Object_Chunk*)*object_array_size);
	

	object->object_data_store->index_num=object_array_size-1;;
	object_array[object_array_size-1]=object;
}

/////////////////////////////////////////

// Class GodFather_Chunk functions

/*
Children for GodFather_Chunk :
"REBSHAPE"		Shape_Chunk
"RSPRITES"		AllSprites_Chunk
"RBOBJECT"		Object_Chunk
"RIFVERIN"		RIF_Version_Num_Chunk
"REBENVDT"		Environment_Data_Chunk
"REBENUMS"		Enum_Chunk
"OBJCHIER"		Object_Hierarchy_Chunk
"OBHALTSH"		Object_Hierarchy_Alternate_Shape_Set_Chunk

*/

GodFather_Chunk::GodFather_Chunk(char * buffer, size_t size)
: Chunk_With_Children (NULL, "REBINFF2")
{
	Parent_File = this;

	char * buffer_ptr = buffer;

	// The start of the first chunk

	while ((buffer_ptr-buffer)< ((signed)size-12) && !error_code) {

		if ((*(int *)(buffer_ptr + 8)) + (buffer_ptr-buffer) > ((signed)size-12)) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		DynCreate(buffer_ptr);
		buffer_ptr += *(int *)(buffer_ptr + 8);

	}

}

/////////////////////////////////////////

// Class RIF_Version_Num_Chunk functions

RIF_IMPLEMENT_DYNCREATE("RIFVERIN",RIF_Version_Num_Chunk)

void RIF_Version_Num_Chunk::fill_data_block(char * data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;

	*((int *) data_start) = file_version_no;

}


/////////////////////////////////////////

// Class RIF_Name_Chunk functions
RIF_IMPLEMENT_DYNCREATE("RIFFNAME",RIF_Name_Chunk)

RIF_Name_Chunk::RIF_Name_Chunk (Chunk_With_Children * parent, const char * rname)
: Chunk (parent, "RIFFNAME")
{
	rif_name = new char [strlen(rname)+1];
	strcpy (rif_name, rname);
}

RIF_Name_Chunk::RIF_Name_Chunk (Chunk_With_Children * parent, const char * rdata, size_t /*rsize*/)
: Chunk (parent, "RIFFNAME")
{
	rif_name = new char [strlen(rdata)+1];
	strcpy (rif_name, rdata);
}

RIF_Name_Chunk::~RIF_Name_Chunk ()
{
	if (rif_name)
		delete [] rif_name;
}


void RIF_Name_Chunk::fill_data_block (char * data_start)
{
	strncpy (data_start, identifier, 8);

	data_start += 8;

	*((int *) data_start) = chunk_size;

	data_start += 4;
	
	strcpy (data_start, rif_name);

}


///////////////////////////////////////

/*
Children for RIF_File_Chunk :
"REBSHAPE"		Shape_Chunk
"RSPRITES"		AllSprites_Chunk
"RBOBJECT"		Object_Chunk
"RIFVERIN"		RIF_Version_Num_Chunk
"REBENVDT"		Environment_Data_Chunk
"OBJCHIER"		Object_Hierarchy_Chunk
"OBHALTSH"		Object_Hierarchy_Alternate_Shape_Set_Chunk

*/


RIF_File_Chunk::RIF_File_Chunk (Chunk_With_Children * parent, const char * file_name)
: Chunk_With_Children (parent, "SUBRIFFL")
{
	char rifIsCompressed = FALSE;
	char *uncompressedData = NULL;
	HANDLE rif_file;
	DWORD file_size;
	DWORD file_size_from_file;
	unsigned long bytes_read;
	char * buffer;
	char * buffer_ptr;
	char id_buffer[9];


	Chunk * ParentFileStore = Parent_File;
	
	Parent_File = this;
	
	error_code = 0;
	
	rif_file = CreateFileA (file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, 
					FILE_FLAG_RANDOM_ACCESS, 0);


	if (rif_file == INVALID_HANDLE_VALUE) {
		error_code = CHUNK_FAILED_ON_LOAD;
		Parent_File = ParentFileStore;
		return;
	}

	file_size = GetFileSize (rif_file, NULL);	

	if (!ReadFile(rif_file, id_buffer, 8, &bytes_read, 0)) {
		error_code = CHUNK_FAILED_ON_LOAD;
		CloseHandle (rif_file);
		Parent_File = ParentFileStore;
		return;
	}	

	//check for compressed rif
	if (!strncmp (id_buffer, COMPRESSED_RIF_IDENTIFIER, 8))
	{
		rifIsCompressed = TRUE;
	}	
	else if (strncmp (id_buffer, "REBINFF2", 8)) {
		error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
		CloseHandle (rif_file);
		Parent_File = ParentFileStore;
		return;
	}	

	buffer = new char [file_size];
	
	/* KJL 17:57:44 19/09/98 - if the rif is compressed, we must load the whole
	file in and then pass it to the decompression routine, which will return a
	pointer to the original data. */
	if (rifIsCompressed)
	{
		if (!ReadFile(rif_file, buffer+8, (file_size-8), &bytes_read, 0))
		{
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			Parent_File = ParentFileStore;
			delete [] buffer;
			return;
		}	
		uncompressedData = HuffmanDecompress((HuffmanPackage*)buffer); 		
		file_size = ((HuffmanPackage*)buffer)->UncompressedDataSize;
		
		delete [] buffer; // kill the buffer the compressed file was loaded into

		buffer_ptr = buffer = uncompressedData+12; // skip header data
	}
	else // the normal uncompressed approach:
	{
		//get the file size stored in the rif file
		if (!ReadFile(rif_file, &file_size_from_file, 4, &bytes_read, 0)) {
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			Parent_File = ParentFileStore;
			delete [] buffer;
			return;
		}	

		//and compare with the actual file size
		if (file_size != file_size_from_file) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			CloseHandle (rif_file);
			Parent_File = ParentFileStore;
			delete [] buffer;
			return;
		}	

		//read the rest of the file into the buffer
		if (!ReadFile(rif_file, buffer, (file_size-12), &bytes_read, 0))
		{
			error_code = CHUNK_FAILED_ON_LOAD;
			CloseHandle (rif_file);
			Parent_File = ParentFileStore;
			delete [] buffer;
			return;
		}
		buffer_ptr = buffer;
	}

	
	

	// Process the RIF

	// The start of the first chunk

	while ((buffer_ptr-buffer)< ((signed) file_size-12) && !error_code) {

		if ((*(int *)(buffer_ptr + 8)) + (buffer_ptr-buffer) > ((signed)file_size-12)) {
			error_code = CHUNK_FAILED_ON_LOAD_NOT_RECOGNISED;
			break;
		}

		DynCreate(buffer_ptr);
		buffer_ptr += *(int *)(buffer_ptr + 8);
	}

	/* KJL 17:59:42 19/09/98 - release the memory holding the rif */
	if (rifIsCompressed)
	{
		free(uncompressedData);
	}
	else
	{
		delete [] buffer;
	}

	CloseHandle (rif_file);

	post_input_processing();

	Parent_File = ParentFileStore;
	
}

void RIF_File_Chunk::post_input_processing()
{
	List<Shape_Chunk *> shplist;
	List<Object_Chunk *> objlist;

	List<Chunk *> child_lists;

	lookup_child("REBSHAPE",child_lists);

	while (child_lists.size()) {
		shplist.add_entry((Shape_Chunk *)child_lists.first_entry());
		child_lists.delete_first_entry();
	}

	lookup_child("RBOBJECT",child_lists);


	while (child_lists.size()) {
		objlist.add_entry((Object_Chunk *)child_lists.first_entry());
		child_lists.delete_first_entry();
	}

	for (LIF<Object_Chunk *> ol(&objlist); !ol.done(); ol.next()) {
		
		if (ol()->get_header()) {
			
			for (LIF<Shape_Chunk *> sl(&shplist); 
					 !sl.done(); sl.next()) {
				if (sl()->get_header())
					if (sl()->get_header()->file_id_num == ol()->get_header()->shape_id_no){
						ol()->assoc_with_shape(sl());
						break;
					}
			}
		}

	}	

	for (LIF<Shape_Chunk *> sli(&shplist); !sli.done(); sli.next())
	{
		Shape_Chunk::max_id = max (Shape_Chunk::max_id,sli()->get_header()->file_id_num);	
	}

	Chunk_With_Children::post_input_processing();	
}


void RIF_File_Chunk::list_objects(List<Object_Chunk *> * pList)
{
	Chunk * child_ptr = children;
	
	while (pList->size())
		pList->delete_first_entry();

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("RBOBJECT", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				pList->add_entry((Object_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}
}

void RIF_File_Chunk::list_shapes(List<Shape_Chunk *> * pList)
{
	Chunk * child_ptr = children;

	while (pList->size())
		pList->delete_first_entry();

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("REBSHAPE", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				pList->add_entry((Shape_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

}

Environment_Data_Chunk * RIF_File_Chunk::get_env_data()
{
	List<Environment_Data_Chunk *> e_list;
	Chunk * child_ptr = children;

	if (children)	
		while	(child_ptr != NULL) {
			if (strncmp ("REBENVDT", child_ptr->identifier, 8) == NULL)
			{
				assert (!child_ptr->r_u_miscellaneous());
				e_list.add_entry((Environment_Data_Chunk *)child_ptr);
			}
			child_ptr = child_ptr->next;
		}

	// There can be only ONE.
	assert (e_list.size() < 2);
	
	if (e_list.size())
		return e_list.first_entry();
	else
	{
		return(0);
	}	
}
