#ifndef _AEE_BSON_H_
#define _AEE_BSON_H_

#define AEE_BSON_BYTE 	0x01
#define AEE_BSON_SHORT 	0x02
#define AEE_BSON_INT	0x03
#define AEE_BSON_FLOAT	0x04
#define AEE_BSON_DOUBLE	0x05
#define AEE_BSON_NULL	0x06
#define AEE_BSON_STRING	0x07
#define AEE_BSON_STRIDX	0x08
#define AEE_BSON_BINARY	0x09
#define AEE_BSON_ARRAY  0x0A
#define AEE_BSON_OBJECT 0x0B
#define AEE_BSON_END	0x0E

#include "AEEBsonValue.h"
#include "AEEBsonStream.h"

#endif//_AEE_BSON_H_
