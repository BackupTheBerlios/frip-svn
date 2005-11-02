// $Id$

#ifndef __frip_api_h
#define __frip_api_h

typedef void (*frip_callback)(int percent);

void * frip_create(const char *iname, const char *oname);
void frip_set_quality(void *, int);
void frip_set_log(void *, const char *);
void frip_set_filter(void *, const char *name, const char *args);
void frip_run(void *);
void frip_finish(void **);

#endif
