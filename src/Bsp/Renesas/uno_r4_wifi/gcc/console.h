#ifndef Bsp_uno_r4_wifi_Console_h_
#define Bsp_uno_r4_wifi_Console_h_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2017  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file

    This file exposes the CPL InputOutput object for the primary serial port
    on the Arduino board.  This file is separate from the Api.h header file 
    because of conflicts with the LHeader pattern (aka colony_map.h)

*----------------------------------------------------------------------------*/


#include "Cpl/Io/InputOutput.h"


/** Returns a reference to the Primary Serial Port Object.
 */
extern Cpl::Io::InputOutput& Bsp_Serial(void);

#endif  // end header latch
