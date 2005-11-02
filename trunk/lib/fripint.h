// $Id$

#ifndef __frip_lib_fripint_h
#define __frip_lib_fripint_h

#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include "frip.h"
#include "log.h"

using std::string;

typedef std::vector<int> samples;
typedef std::vector< std::pair< string, string > > tagset;

struct flowspec
{
	unsigned int mChannels;
	// Bit per channel.
	unsigned int mSampleSize;
	// Samples per second in one channel.
	unsigned int mSampleRate;
	flowspec();
	flowspec(const flowspec &src) { *this = src; }
	~flowspec() { }
	flowspec& operator = (const flowspec &);
	bool isnull() const { return (mChannels == 0 && mSampleSize == 0 && mSampleRate == 0);  }
	bool operator == (const flowspec &src) const
	{
		if (mChannels != src.mChannels)
			return false;
		if (mSampleSize != src.mSampleSize)
			return false;
		if (mSampleRate != src.mSampleRate)
			return false;
		return true;
	}
	bool operator != (const flowspec &src) const
	{
		return !(*this == src);
	}
};

class fripex
{
public:
	fripex() { }
	virtual ~fripex() { }
	virtual const std::string & str() const = 0;
};

class ruler
{
	typedef std::list<class filter *> filters;
	class reader *mReader;
	class writer *mWriter;
	filters mFilters;
	std::string mNameIn, mNameOut;
public:
	ruler(const char *iname, const char *oname);
	~ruler();
	void set_log(const char *);
	void set_quality(int);
	void add_filter(const char *name, const char *arg);
	void run();
};

#endif
