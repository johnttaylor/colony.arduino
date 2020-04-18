/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2020  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "colony_config.h"
#include "Cpl/Io/File/Api.h"
#include "Cpl/System/Trace.h"
#include "Cpl/System/_testsupport/Shutdown_TS.h"
#include "Cpl/Text/FString.h"
#include <time.h>
#include "Cpl/System/Assert.h"

#define SECT_     "_0test"

#define REQUIRE   CPL_SYSTEM_ASSERT


#define TESTINPUT_TXT_FILE_LENGTH   101
#define TESTINPUT_TEXT_HELLO_OFFEST 0x58


/// 
using namespace Cpl::Io::File;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
namespace {

class Walker : public Api::DirectoryWalker
{
public:
    int        m_depth;
    bool       m_files;
    bool       m_dirs;
    NameString m_workName;
    bool       m_contentCheck;

public:
    ///
    Walker( int depth, bool files=true, bool dirs=true )
        :m_depth( depth ),
        m_files( files ),
        m_dirs( dirs ),
        m_contentCheck( true )
    {
    }

public:
    Cpl::Type::Traverser::Status_T item( const char* currentParent, const char* fsEntryName, Api::Info& entryInfo )
    {
        // File check
        m_workName = fsEntryName;
        if ( entryInfo.m_isFile )
        {
            if ( !m_files )
            {
                m_contentCheck = false;
            }

            else if ( m_depth > 2 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" && m_workName != "d3.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth > 1 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_workName != "d1.txt" )
            {
                m_contentCheck = false;
            }
        }

        // Dir check
        m_workName = fsEntryName;
        if ( entryInfo.m_isDir )
        {
            if ( !m_dirs )
            {
                m_contentCheck = false;
            }

            else if ( m_depth >= 2 )
            {
                if ( m_workName != "d2" && m_workName != "d3" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth >= 1 )
            {
                if ( m_workName != "d2" )
                {
                    m_contentCheck = false;
                }
            }
            else
            {
                m_contentCheck = false;
            }
        }

        CPL_SYSTEM_TRACE_MSG( SECT_, ( "%s%s Parent=%s, item=%s.  (content=%d)", entryInfo.m_isFile ? "f" : "-", entryInfo.m_isDir ? "d" : "-", currentParent, fsEntryName, m_contentCheck ) );
        return Cpl::Type::Traverser::eCONTINUE;
    }
};

}; // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
void testcase_api1()
{
    CPL_SYSTEM_TRACE_FUNC( SECT_ );

    NameString name;
    NameString name2;

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Copy, Appended..." ) );
    REQUIRE( Api::exists( "copy.txt" ) == false );
    REQUIRE( Api::exists( "copy2.txt" ) == false );
    REQUIRE( Api::copyFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( Api::isFile( "copy.txt" ) );
    REQUIRE( Api::size( "copy.txt" ) == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::appendFile( "testinput.txt", "copy2.txt" ) );
    REQUIRE( Api::size( "copy2.txt" ) == TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::appendFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( Api::size( "copy.txt" ) == 2 * TESTINPUT_TXT_FILE_LENGTH );
    REQUIRE( Api::remove( "copy.txt" ) );
    REQUIRE( Api::isFile( "copy.txt" ) == false );
    REQUIRE( Api::remove( "copy2.txt" ) );
    REQUIRE( Api::exists( "copy2.txt" ) == false );

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "CanonicalPath & getCwd..." ) );
    REQUIRE( Api::canonicalPath( ".", name ) );
    REQUIRE( Api::getCwd( name2 ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "[%s] ('.') =? [%s] ('cwd')", name.getString(), name2.getString() ) );
    REQUIRE( name == name2 );
    REQUIRE( Api::canonicalPath( "..", name ) );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "[%s] (cwd) startsWith? [%s] ('..')", name2.getString(), name.getString() ) );
    REQUIRE( name2.startsWith( name ) );

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Create, move, remove, etc. ..." ) );
    REQUIRE( Api::exists( "bob" ) == false );
    REQUIRE( Api::createDirectory( "bob" ) );
    REQUIRE( Api::exists( "bob" ) );
    REQUIRE( Api::isDirectory( "bob" ) );
    REQUIRE( Api::exists( "uncle.txt" ) == false );
    REQUIRE( Api::createFile( "bob/uncle.txt" ) );
    REQUIRE( Api::exists( "bob/uncle.txt" ) );
    REQUIRE( Api::isFile( "bob/uncle.txt" ) );

    REQUIRE( Api::exists( "charlie" ) == false );
    REQUIRE( Api::renameInPlace( "bob", "charlie" ) );
    REQUIRE( Api::isDirectory( "charlie" ) );
    REQUIRE( Api::isFile( "charlie/uncle.txt" ) );

    REQUIRE( Api::exists( "uncle.txt" ) == false );
    REQUIRE( Api::moveFile( "charlie/uncle.txt", "uncle.txt" ) );
    REQUIRE( Api::isFile( "uncle.txt" ) );
    REQUIRE( Api::renameInPlace( "uncle.txt", "your.txt" ) );
    REQUIRE( Api::isFile( "your.txt" ) );

    REQUIRE( Api::createFile( "charlie/test.txt" ) );
    REQUIRE( Api::exists( "charlie/test.txt" ) );
    REQUIRE( Api::remove( "charlie" ) == false );
    REQUIRE( Api::remove( "charlie/test.txt" ) );
    REQUIRE( Api::remove( "charlie" ) );
    REQUIRE( Api::exists( "charlie" ) == false );
    REQUIRE( Api::remove( "your.txt" ) );
    REQUIRE( Api::exists( "your.txt" ) == false );


    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Info..." ) );
    REQUIRE( Api::exists( ".." ) );
    REQUIRE( Api::exists( "testinput.txt" ) );
    REQUIRE( Api::exists( "nothere.txt" ) == false );
    REQUIRE( Api::isFile( "." ) == false );
    REQUIRE( Api::isFile( "testinput.txt" ) );
    REQUIRE( Api::isDirectory( "." ) == true );
    REQUIRE( Api::isDirectory( ".." ) == true );
    REQUIRE( Api::isDirectory( "testinput.txt" ) == false );
    REQUIRE( Api::isReadable( "." ) );
    REQUIRE( Api::isReadable( ".." ) );
    REQUIRE( Api::isReadable( "testinput.txt" ) );
    REQUIRE( Api::isReadable( "testinput.txte" ) == false );
    REQUIRE( Api::isWriteable( "." ) );
    REQUIRE( Api::isWriteable( ".." ) );
    REQUIRE( Api::isWriteable( "testinput.txt" ) );
    REQUIRE( Api::isWriteable( "testinput.txtdd" ) == false );
    REQUIRE( Api::size( "testinput.txt" ) == TESTINPUT_TXT_FILE_LENGTH );
    time_t t1, t2, t3;
    t1 = Api::timeModified( "." );
    t2 = Api::timeModified( ".." );
    t3 = Api::timeModified( "testinput.txt" );
    name = ctime( &t2 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'..' mtime           := %s", name.getString() ) );
    name = ctime( &t1 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'.' mtime            := %s", name.getString() ) );
    name = ctime( &t3 );
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "'testinput.txt' mtime:= %s", name.getString() ) );
    REQUIRE( t1 != ( (time_t)-1 ) );
    REQUIRE( t2 != ( (time_t)-1 ) );
    REQUIRE( t3 != ( (time_t)-1 ) );

    ///
    CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk directories..." ) );
    REQUIRE( Api::createDirectory( "d1" ) );
    REQUIRE( Api::createFile( "d1/d1.txt" ) );
    REQUIRE( Api::createDirectory( "d1/d2" ) );
    REQUIRE( Api::createFile( "d1/d2/d2.txt" ) );
    REQUIRE( Api::createDirectory( "d1/d2/d3" ) );
    REQUIRE( Api::createFile( "d1/d2/d3/d3.txt" ) );
    {
        Walker iterator( 3 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 100 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 3, true, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 100, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 3, false, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 100, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator( 2 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 2 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 2, true, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 2, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 2, false, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 2, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator( 1 );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 1 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 1, true, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 12" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 1, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 1, false, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 1, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    REQUIRE( Api::createDirectory( "d1/d2/d3/d4" ) );
    REQUIRE( Api::createFile( "d1/d2/d3/d3a.txt" ) );
    {
        Walker iterator( 3, true, false );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 100, true, false ) );
        REQUIRE( iterator.m_contentCheck == false );
    }
    {
        Walker iterator( 3, false, true );
        CPL_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( Api::walkDirectory( "d1", iterator, 100, false, true ) );
        REQUIRE( iterator.m_contentCheck == false );
    }

    REQUIRE( Api::remove( "d1/d2/d3/d4" ) );
    REQUIRE( Api::remove( "d1/d2/d3/d3.txt" ) );
    REQUIRE( Api::remove( "d1/d2/d3/d3a.txt" ) );
    REQUIRE( Api::remove( "d1/d2/d3" ) );
    REQUIRE( Api::remove( "d1/d2/d2.txt" ) );
    REQUIRE( Api::remove( "d1/d2" ) );
    REQUIRE( Api::remove( "d1/d1.txt" ) );
    REQUIRE( Api::remove( "d1" ) );



}
