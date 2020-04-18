
#include "Arduino.h"
#include "Bsp/Api.h"
#include "Cpl/System/Api.h"
#include "Cpl/System/FreeRTOS/Thread.h"
#include "Cpl/Io/InputOutput.h"
#include <Adafruit_SPIFlash.h>
#include <SdFat.h>

extern int testcase_readwrite();

extern Cpl::Io::InputOutput& Bsp_Serial( void );

// file system object from SdFat
FatFileSystem fatfs;

File myFile;

Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash( &flashTransport );


// the setup function runs once when you press reset or power the board
// NOTE: FreeRTOS is RUNNING at this point
void setup()
{
    // Initialize CPL
    Cpl::System::Api::initialize();

    // Make the current/main thread a CPL Thread
    Cpl::System::FreeRTOS::Thread::makeNativeMainThreadACplThread();

    // Initialize the board (for use with CPL)
    Bsp_Api_initialize();
    Bsp_beginArduinoSerialObject( 115200, SERIAL_8N1 );

    // Artificially delay so there is time to connect to the device in order to see error message
    Cpl::System::Api::sleep( 5000 );  
    
    // Initialize the File System
    Bsp_beginFileSystem();

    // Run test (if there is failed -->it trip a fatal ASSERT
    ::Serial.println( "Running: readwrite() ..." ); testcase_readwrite();

    // If I get here EVERYTHING WORKED! 
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );
    ::Serial.println( "ALL Test Cases PASSED." );
    ::Serial.println( "!!!!!!!!!!!!!!!!!!!!!!!!!!." );
}



// the loop function runs over and over again forever
void loop()
{
    // Nothing to do in the loop.
    delay( 100 );
}
