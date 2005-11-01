// $Id$

#ifndef __frip_api_h
#define __frip_api_h

typedef void (*frip_callback)(int percent);

typedef enum {
	mtNone,
	mtBoth,
	mtLeft,
	mtRight,
} mixtype_t;

bool frip_encode(const char *iname, const char *oname, int quality, mixtype_t mix, frip_callback);
void frip_set_log(const char *);

#endif
