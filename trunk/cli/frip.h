// $Id$

#ifndef __frip_cli_frip_h
#define __frip_cli_frip_h

#include <string>
#include <map>
#include "../lib/frip.h"

using std::string;

class frip
{
	bool mVerbose;
	// Log file name.
	string mLogName;
	// Prefix filter.
	string mFilter;
	// Default file suffix.
	string mDefaultSuffix;
	// Output quality.
	int mQuality;
	// Recurse into directories.
	bool mRecurse;
	// Filters.
	typedef std::map<std::string, std::string> filters;
	filters mFilters;
protected:
	// Encodes a file.
	bool do_file(string src, string dst);
	// Encodes a directory, recursively.
	bool do_dir(string src, string dst);
	// Determine entry type.
	static bool is_file(const string &);
	static bool is_dir(const string &);
	// Create entries.
	static bool mkdir(const string &);
	// Checks whether a file passes the filters.
	bool passes(const string &) const;
public:
	frip();
	~frip();
	bool run(int, char * const *);
};

#endif
