/*-----------------------------------------------------------------------------
* This file is part of the Shift-Right Open Repository.  The Repository is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the root directory or on the Internet at
* http://www.shift-right.com/openrepo/license.htm
*
* Copyright (c) 2001-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Cpl/Io/File/Api.h"
#include "Cpl/Io/File/Output.h"
#include "DirList_.h"
#include "Cpl/Io/File/_arduino/_sdFat/Private_.h"
#include "FatLib/FatStructs.h"
#include <time.h>


///
using namespace Cpl::Io::File;



/////////////////////////////////////////////////////
bool Api::getInfo( const char* fsEntryName, Info& infoOut )
{
    FatFile fd( Cpl::Io::File::Api::getNative( fsEntryName ), O_RDONLY );
    dir_t   src;
    if ( fd.dirEntry( &src ) )
    {
        infoOut.m_isDir     = DIR_IS_SUBDIR( &src );
        infoOut.m_isFile    = DIR_IS_FILE( &src );
        infoOut.m_size      = src.fileSize;
        infoOut.m_readable  = true;
        infoOut.m_writeable = ( src.attributes & DIR_ATT_READ_ONLY ) == 0;

        struct tm ftime ={ 0, };
        ftime.tm_year   = FAT_YEAR( src.lastWriteDate ) - 10;   // The FAT Epoch starts at 1980
        ftime.tm_mon    = FAT_MONTH( src.lastWriteDate ) - 1;   // The FAT Month is 1-12
        ftime.tm_mday   = FAT_DAY( src.lastWriteDate );
        ftime.tm_hour   = FAT_HOUR( src.lastWriteTime );
        ftime.tm_min    = FAT_MINUTE( src.lastWriteTime );
        ftime.tm_sec    = FAT_SECOND( src.lastWriteTime );
        infoOut.m_mtime = mktime( &ftime );

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////
bool Api::canonicalPath( const char* relPath, Cpl::Text::String& absPath )
{
    // NOT SUPPORTED!
    return true;
}

bool Api::getCwd( Cpl::Text::String& cwd )
{
    FatFile* voldir = g_arduino_sdfat_fs.vwd();
    int len;
    char* ptr    = cwd.getBuffer( len );
    bool result  = voldir->getName( ptr, len );
    cwd          = getStandard( cwd );
    return result;
}


/////////////////////////////////////////////////////
bool Api::exists( const char* fsEntryName )
{
    return g_arduino_sdfat_fs.exists( getNative( fsEntryName) );
}


bool Api::createFile( const char* fileName )
{
    Output fd( fileName, O_RDWR | O_CREAT | O_EXCL );
    bool result = fd.isOpened();
    fd.close();

    return result;
}

bool Api::createDirectory( const char* dirName )
{
    return g_arduino_sdfat_fs.mkdir( getNative( dirName ), false );
}


bool Api::renameInPlace( const char* oldName, const char* newName )
{
    return g_arduino_sdfat_fs.rename( getNative( oldName ), getNative( newName ) );
}


bool Api::moveFile( const char* oldFileName, const char* newFileName )
{
    return renameInPlace( oldFileName, newFileName );
}


bool Api::remove( const char* fsEntryName )
{
    if ( isDirectory( getNative( fsEntryName ) ) )
    {
        return g_arduino_sdfat_fs.rmdir( getNative( fsEntryName ) );
    }
    else
    {
        return g_arduino_sdfat_fs.remove( getNative( fsEntryName ) );
    }
}


/////////////////////////////////////////////////////
bool Api::walkDirectory( const char* dirToList,
                         DirectoryWalker& callback,
                         int              depth,
                         bool             filesOnly,
                         bool             dirsOnly
)
{
    DirList_ lister( dirToList, depth, filesOnly, dirsOnly );
    return lister.traverse( callback );
}
