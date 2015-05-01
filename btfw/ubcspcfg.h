/*****************************************************************************/
/************************Flaicomm CopyRight 2010.*****************************/ 
/*****************************************************************************/

#ifndef _UBCSP_CONFIG_H_
#define _UBCSP_CONFIG_H_



#define 		__android__	
/* If we wish to use CRC's, then change 0 to 1 in the next line */
#define UBCSP_CRC 1



#if defined(__linux_usb__)

typedef signed char     int8;
typedef short           int16;
typedef long            int32;

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
//typedef unsigned long   uint32_t;
//typedef long			int32_t;
typedef unsigned char   bool_t;

/*typedef unsigned int    uint;*/

#else 
#if defined(__android__)

typedef signed char     int8;
typedef short           int16;
typedef long            int32;
typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned char   bool_t;


#else
#if defined(__sun) && defined(__sparc) && defined(__SVR4)

typedef signed char     int8;
typedef short           int16;
typedef long            int32;

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;
typedef long			int32_t;
typedef unsigned char   bool_t;

typedef unsigned int    uint;

#else /* sun/sparc/SVR4 */
#if defined(__CYGWIN__)

typedef signed char     int8;
typedef short           int16;
typedef long            int32;

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;
//typedef long			int32_t;
typedef unsigned char   bool_t;

typedef unsigned int    uint;
typedef long long int   	int64;

#else /* cygwin */
#if defined(_WIN32)
typedef signed char     int8;
typedef short           int16;
typedef long            int32;

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;
typedef long			int32_t;
typedef unsigned char   bool_t;

typedef unsigned int    uint;
typedef long long int   	int64;

#ifndef __cplusplus
#define inline
#endif

#else /* windows */
#error "must define machine type"

#endif /* windows */
#endif /* cygwin */
#endif /* sun/sparc/SVR4 */
#endif 
#endif /* linux/386 */



/* Do we want to show packet errors in debug output */
#define SHOW_PACKET_ERRORS	0

/* Do we want to show Link Establishment State transitions in debug output */
#define SHOW_LE_STATES		0

#endif
