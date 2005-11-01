// $Id$

#ifndef __frip_lib_fripint_h
#define __frip_lib_fripint_h

#include <stdio.h>
#include <string>
#include <vector>
#include "frip.h"
#include "log.h"

using std::string;

typedef std::vector<int> samples;
typedef std::vector< std::pair< string, string > > tagset;

struct flowspec
{
	unsigned int mChannels;
	unsigned int mBitPerChannel;
	flowspec()
	{
		mChannels = 0;
		mBitPerChannel = 0;
	}
};

#endif
