// $Id$

#ifndef __frip_lib_filter_h
#define __frip_lib_filter_h

#include "../fripint.h"

class filter
{
public:
	filter(const char *) { }
	virtual ~filter() { }
	virtual const char * GetName() = 0;
	virtual void process(samples &, const flowspec &) = 0;
};

class downmix : public filter
{
	enum mixtype {
		mtNone,
		mtLeft,
		mtRight,
		mtBoth,
	} mMixType;
public:
	downmix(const char *);
	~downmix();
	void process(samples &, const flowspec &);
};

#endif
