// $Id$

#ifndef __frip_cli_frip_h
#define __frip_cli_frip_h

#include "../lib/frip.h"

class frip
{
	bool mVerbose;
	const char *mLogName;
protected:
	// Encodes a file.
	bool do_file(const char *src, const char *dst);
public:
	frip();
	~frip();
	bool run(int, char * const *);
};

#endif
