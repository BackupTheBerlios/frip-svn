// $Id$

#ifndef __frip_api_h
#define __frip_api_h

#ifdef __cplusplus
# define E extern "C"
#else
# define E
# define bool int
#endif

typedef void (*frip_callback)(unsigned int percent);

E bool frip_encode(const char *iname, const char *oname, frip_callback);
E void frip_set_log(const char *);

#endif
