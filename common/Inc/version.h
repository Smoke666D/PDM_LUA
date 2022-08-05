#ifndef VERSION_H
#define VERSION_H
/*----------------- BOOTLOADER VERSION -----------------*/
#define BOOTLOADER_VERSION_ADR  0x8007FFCU
/*------------------ HARDWARE VERSION ------------------*/
#define HARDWARE_VERSION_MAJOR  1
#define HARDWARE_VERSION_MINOR  0
#define HARDWARE_VERSION_PATCH  0
#if ( HARDWARE_VERSION_MAJOR > 255U )
  #error( "Major hardware version too big" )
#endif
#if ( HARDWARE_VERSION_MINOR > 255U )
  #error( "Minor hardware version too big" )
#endif
#if ( HARDWARE_VERSION_PATCH > 255U )
  #error( "Patch hardware version too big" )
#endif
/*------------------ FIRMWARE VERSION ------------------*/
#define FIRMWARE_VERSION_MAJOR  0
#define FIRMWARE_VERSION_MINOR  1
#define FIRMWARE_VERSION_PATCH  0
#if ( FIRMWARE_VERSION_MAJOR > 255U )
  #error( "Major firmware version too big" )
#endif
#if ( FIRMWARE_VERSION_MINOR > 255U )
  #error( "Minor firmware version too big" )
#endif
#if ( FIRMWARE_VERSION_PATCH > 255U )
  #error( "Patch firmware version too big" )
#endif
/*------------------------------------------------------*/
#endif