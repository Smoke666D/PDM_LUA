#ifndef COMMON_H
#define COMMON_H
/*------------------------ Define --------------------------------------*/
#define     UNIQUE_ID_ADR               0x1FFF7A10U
#define     UNIQUE_ID_LENGTH            6U
/*------------------------ Macros --------------------------------------*/
#define     REVERSE_BYTE( b )           ( ( ( b << 7U ) & 0x80U ) | ( ( b << 5U ) & 0x40U ) | ( ( b << 3U ) & 0x20U ) | ( ( b << 1U ) & 0x10U ) | ( ( b >> 1U ) & 0x08U ) | ( ( b >> 3U ) & 0x04U ) | ( ( b >> 5U ) & 0x02U ) | ( ( b >> 7U ) & 0x01U ) )
#define     GET_UNIQUE_ID0              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x00U ) )
#define     GET_UNIQUE_ID1              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x04U ) )
#define     GET_UNIQUE_ID2              ( *( uint32_t* )( UNIQUE_ID_ADR + 0x08U ) )
/*----------------------- Functions ------------------------------------*/
void     vSYSgetUniqueID32 ( uint32_t* id );
void     vSYSgetUniqueID16 ( uint16_t* id );
/*----------------------------------------------------------------------*/
#endif