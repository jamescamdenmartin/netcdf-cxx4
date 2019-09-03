#include "ncFile.h"
#include "ncCheck.h"
#include "ncException.h"
#include "ncByte.h"
#include<iostream>
#include<string>
#include<sstream>
using namespace std;
using namespace netCDF;
using namespace netCDF::exceptions;

int g_ncid = -1;

// destructor
NcFile::~NcFile()
{
  // destructor may be called due to an exception being thrown
  // hence throwing an exception from within a destructor
  // causes undefined behaviour! so just printing a warning message
  try
  {
    if(!isMemIOFile){
        close();
    }else{ // If it's an in memory file, we need to free the memory
        bool memclose_success; 
        size_t size;           
        void* memory;
        tie(memclose_success, size, memory) = close_memio();
        if(memclose_success) 
            free(memory);
    }
  }
  catch (NcException &e)
  {
    cerr << e.what() << endl;
  }
}

void NcFile::close()
{
  if (!nullObject) {
    ncCheck(nc_close(myId),__FILE__,__LINE__);
    g_ncid = -1;
  }

  nullObject = true;
}

// Close an in-memory netCDF, getting a pointer to the final memory and size.
// Whoever calls this is now in charge of freeing that memory.
tuple<bool, size_t, void*> NcFile::close_memio()
{
  if (!nullObject && isMemIOFile) {
    NC_memio memio = {};
    ncCheck(nc_close_memio(myId, &memio),__FILE__,__LINE__);
    g_ncid = -1;
    nullObject = true;
    return make_tuple(true, memio.size, memio.memory);
  }else{
    return make_tuple(false, 0, nullptr);
  }
}

// Constructor generates a null object.
NcFile::NcFile() :
    NcGroup()  // invoke base class constructor
{}

// constructor
NcFile::NcFile(const string& filePath, const FileMode fMode)
{
  open(filePath, fMode);
}

// constructor
NcFile::NcFile(const string& filePath, const int ncFileFlags)
{
  open(filePath, ncFileFlags);
}

/*! Open a file from a path and an NC_FLAG
 *
 * This will allow for fine-grained control by the user.
 *
 *
 */
void NcFile::open(const string& filePath, int ncFileFlags) {
  if(!nullObject)
    close();

  ncCheck(nc_open(filePath.c_str(), ncFileFlags, &myId),__FILE__,__LINE__);
  g_ncid = myId;

  nullObject=false;
}


// open a file from path and mode
void NcFile::open(const string& filePath, const FileMode fMode)
{
  if (!nullObject)
    close();



  switch (fMode)
    {
    case NcFile::write:
      ncCheck(nc_open(filePath.c_str(), NC_WRITE, &myId),__FILE__,__LINE__);
      break;
    case NcFile::read:
      ncCheck(nc_open(filePath.c_str(), NC_NOWRITE, &myId),__FILE__,__LINE__);
      break;
    case NcFile::newFile:
      ncCheck(nc_create(filePath.c_str(), NC_NETCDF4 | NC_NOCLOBBER, &myId),__FILE__,__LINE__);
      break;
    case NcFile::replace:
      ncCheck(nc_create(filePath.c_str(), NC_NETCDF4 | NC_CLOBBER, &myId),__FILE__,__LINE__);
      break;
    }

  g_ncid = myId;

  nullObject=false;
}

// Either open a file from a passed in chunk of memory, or create a new file in memory
void NcFile::open(const string& path, const FileMode fMode, const FileFormat fFormat, size_t size, void* memory, bool memory_locked )
{
  if (!nullObject)
    close();

  int format;
  
  NC_memio memio = {};
  memio.size = size;
  memio.memory = memory;
  if(memory_locked)
    memio.flags = NC_MEMIO_LOCKED;
  else
    memio.flags = 0;
  
  switch (fFormat)
    {
    case NcFile::classic:
	format = 0;
	break;
    case NcFile::classic64:
	format = NC_64BIT_OFFSET;
	break;
    case NcFile::nc4:
	format = NC_NETCDF4;
	break;
    case NcFile::nc4classic:
	format = NC_NETCDF4 | NC_CLASSIC_MODEL;
	break;
    }
  switch (fMode)
    {
    case NcFile::write:
      ncCheck(nc_open_memio(path.c_str(), format | NC_WRITE, &memio , &myId),__FILE__,__LINE__);
      break;
    case NcFile::read:
      ncCheck(nc_open_memio(path.c_str(), format | NC_WRITE, &memio , &myId),__FILE__,__LINE__);
      break;
    case NcFile::newFile:
    case NcFile::replace:
      ncCheck(nc_create_mem(path.c_str(), format | NC_WRITE, memio.size, &myId),__FILE__,__LINE__);
      break;
    }

  g_ncid = myId;
  isMemIOFile = true;
  nullObject=false;
}

// constructor with file type specified
NcFile::NcFile(const string& filePath, const FileMode fMode, const FileFormat fFormat )
{
  open(filePath, fMode, fFormat);
}


/*! Allow for the explicit creation of a file using a path and NC_ file flags from netcdf.h
 *
 * @author wfisher
 */
void NcFile::create(const string& filePath, const int ncFileFlags) {
  if(!nullObject)
    close();

  ncCheck(nc_create(filePath.c_str(),ncFileFlags,&myId),__FILE__,__LINE__);

  g_ncid = myId;

  nullObject=false;
}

void NcFile::open(const string& filePath, const FileMode fMode, const FileFormat fFormat )
{
  if (!nullObject)
    close();

  int format;
  switch (fFormat)
    {
    case NcFile::classic:
	format = 0;
	break;
    case NcFile::classic64:
	format = NC_64BIT_OFFSET;
	break;
    case NcFile::nc4:
	format = NC_NETCDF4;
	break;
    case NcFile::nc4classic:
	format = NC_NETCDF4 | NC_CLASSIC_MODEL;
	break;
    }
  switch (fMode)
    {
    case NcFile::write:
      ncCheck(nc_open(filePath.c_str(), format | NC_WRITE, &myId),__FILE__,__LINE__);
      break;
    case NcFile::read:
      ncCheck(nc_open(filePath.c_str(), format | NC_NOWRITE, &myId),__FILE__,__LINE__);
      break;
    case NcFile::newFile:
      ncCheck(nc_create(filePath.c_str(), format | NC_NOCLOBBER, &myId),__FILE__,__LINE__);
      break;
    case NcFile::replace:
      ncCheck(nc_create(filePath.c_str(), format | NC_CLOBBER, &myId),__FILE__,__LINE__);
      break;
    }

  g_ncid = myId;
  nullObject=false;
}

// Synchronize an open netcdf dataset to disk
void NcFile::sync(){
  ncCheck(nc_sync(myId),__FILE__,__LINE__);
}

// Leave define mode, used for classic model
void NcFile::enddef() {
    ncCheck(nc_enddef(myId),__FILE__,__LINE__);
}
