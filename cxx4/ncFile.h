#include <string>
#include "ncGroup.h"
#include "netcdf.h"
#include "netcdf_mem.h"

#ifndef NcFileClass
#define NcFileClass


//!  C++ API for netCDF4.
namespace netCDF
{

  /*!
    Class represents a netCDF root group.
    The Ncfile class is the same as the NcGroup class with the additional functionality for opening
    and closing files.
   */
  class NcFile : public NcGroup
   {
   public:

      enum FileMode
	 {
	    read,	//!< File exists, open read-only.
	    write,      //!< File exists, open for writing.
	    replace,	//!< Create new file, even if already exists.
	    newFile	//!< Create new file, fail if already exists.
	 };

      enum FileFormat
         {
	    classic,    //!< Classic format, classic data model
	    classic64,  //!< 64-bit offset format, classic data model
	    nc4,        //!< (default) netCDF-4/HDF5 format, enhanced data model
	    nc4classic  //!< netCDF-4/HDF5 format, classic data model
         };


      /*! Constructor generates a \ref isNull "null object". */
      NcFile();

      /*!
        Opens a netCDF file.
        \param filePath    Name of netCDF optional path.
        \param ncFileFlags File flags from netcdf.h
      */

      NcFile(const std::string& filePath, int ncFileFlags);

      /*!
      Opens an in-memory netCDF file of a specified format.
      \param path           Name of netCDF optional path.
      \param fMode          The file mode:
                             - 'read'    File of size bytes exists at memory, open for read-only.
                             - 'write'   File of size bytes exists at memory, open for writing.
                             - 'newFile' Creates a new file in memory, allocating an initial buffer of size bytes.
      \param size           Either the size in bytes of a file already in memory, or number of bytes to allocate when creating a memory file
      \param memory         Pointer to the start of a file in memory
      \param memory_locked  If true libnetCDF is not given control of the passed in memory - it will never attempt to allocate more memory or free. 
                            If false libnetCDF will reallocate memory as needed, ownership of the provided buffer is given to libnetCDF.
      */
      NcFile(const std::string& path, const FileMode fMode, const FileFormat fFormat, size_t size, void* memory, bool memory_locked );


      /*!
	Opens a netCDF file.
	\param filePath    Name of netCDF optional path.
	\param fMode       The file mode:
	                    - 'read'    File exists, open for read-only.
	                    - 'write'   File exists, open for writing.
	                    - 'replace' Create new file, even it already exists.
	                    - 'newFile' Create new file, fail it exists already.
      */
      NcFile(const std::string& filePath, FileMode fMode);

      /*!
        Create a netCDF file.
        \param filePath    Name of netCDF optional path.
        \param ncFileFlags File flags from netcdf.h
      */
      void create(const std::string& filePath, int ncFileFlags);

      /*!
        Opens a netCDF file.
        \param filePath    Name of netCDF optional path.
        \param ncFileFlags File flags from netcdf.h
      */
      void open(const std::string& filePath, int ncFileFlags);


      /*!
        Opens a netCDF file.
        \param filePath    Name of netCDF optional path.
        \param fMode       The file mode:
        - 'read'    File exists, open for read-only.
        - 'write'   File exists, open for writing.
        - 'replace' Create new file, even it already exists.
        - 'newFile' Create new file, fail it exists already.
      */
      void open(const std::string& filePath, FileMode fMode);

      /*!
	Creates a netCDF file of a specified format.
	\param filePath    Name of netCDF optional path.
	\param fMode       The file mode:
	                    - 'replace' Create new file, even it already exists.
	                    - 'newFile' Create new file, fail it exists already.
      */
      NcFile(const std::string& filePath, FileMode fMode, FileFormat fFormat);
      /*!
      Creates a netCDF file of a specified format.
      \param filePath    Name of netCDF optional path.
      \param fMode       The file mode:
                          - 'replace' Create new file, even it already exists.
                          - 'newFile' Create new file, fail it exists already.
      */
      void open(const std::string& filePath, FileMode fMode, FileFormat fFormat);
      /*!
      Opens or creates an in-memory netCDF file of a specified format.
      \param path           Name of netCDF optional path.
      \param fMode          The file mode:
                             - 'read'    File of size bytes exists at memory, open for read-only.
                             - 'write'   File of size bytes exists at memory, open for writing.
                             - 'newFile' Creates a new file in memory, allocating an initial buffer of size bytes.
      \param size           Either the size in bytes of a file already in memory, or number of bytes to allocate when creating a memory file
      \param memory         Pointer to the start of a file in memory
      \param memory_locked  If true libnetCDF is not given control of the passed in memory - it will never attempt to allocate more memory or free. 
                            If false libnetCDF will reallocate memory as needed, ownership of the provided buffer is given to libnetCDF.
      */
      void open(const std::string& path, const FileMode fMode, const FileFormat fFormat, size_t size, void* memory, bool memory_locked );

      //! Close a file before destructor call
      void close();
      
      /*!
      Attempts to close a memory IO netCDF file, returning the size and a pointer to memory if successful.
      The caller takes ownership of that memory.
      \return Tuple of success, memory buffer size, pointer to memory buffer
      */
      std::tuple<bool, size_t, void*> close_memio();

      /*! destructor */
      virtual ~NcFile(); //closes file and releases all resources

      //! Synchronize an open netcdf dataset to disk
      void sync();

      //! Leave define mode, used for classic model
      void enddef();

   private:
	   /* Do not allow definition of NcFile involving copying any NcFile or NcGroup.
		  Because the destructor closes the file and releases al resources such 
		  an action could leave NcFile objects in an invalid state */
	   NcFile& operator =(const NcGroup & rhs);
	   NcFile& operator =(const NcFile & rhs);
	   NcFile(const NcGroup& rhs);
	   NcFile(const NcFile& rhs);
       bool isMemIOFile = false;
   };

}


#endif
