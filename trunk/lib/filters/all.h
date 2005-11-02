// $Id$

#ifndef __frip_lib_filter_h
#define __frip_lib_filter_h

#include "../fripint.h"

class filter
{
public:
	filter(const char *) { }
	virtual ~filter() { }
	virtual const char * GetName() const = 0;
	virtual void process(flowspec &, samples &) = 0;
};

class downmix : public filter
{
	enum mixtype {
		mtNone,
		mtLeft,
		mtRight,
		mtBoth,
		mtBlur,
	} mMixType;
public:
	downmix(const char *);
	~downmix();
	const char * GetName() const;
	void process(flowspec &, samples &);
};

#endif
