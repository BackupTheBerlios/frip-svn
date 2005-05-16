// $Id$

#include <memory>
#include <string.h>
#include "frip.h"
#include "log.h"
#include "reader.h"
#include "writer.h"

static inline bool equals(const char *a, const char *b)
{
	return strcasecmp(a, b) == 0;
}

static reader * mkreader(const char *fname, frip_callback cb)
{
	const char *ext = strchr(fname, '.');

	if (ext != NULL) {
		if (equals(ext, ".aiff") || equals(ext, ".aif"))
			return new raiff(cb);
		else if (strcasecmp(ext, ".wav") == 0)
			return new rwave(cb);
	}

	return NULL;
}

static writer * mkwriter(const char *fname, const reader *r)
{
	const char *ext = strchr(fname, '.');

	if (ext != NULL) {
		if (equals(ext, ".raw"))
			return new wraw(r);
		else if (equals(ext, ".wav") || equals(ext, ".wave"))
			return new wwave(r);
		else if (equals(ext, ".aiff"))
			return new waiff(r);
#ifdef HAVE_lame
		else if (equals(ext, ".mp3"))
			return new wlame(r);
#endif
#ifdef HAVE_flac
		else if (equals(ext, ".flac"))
			return new wflacf(r);
		else if (equals(ext, ".flacs"))
			return new wflacs(r);
#endif
	}

	return NULL;
}

E bool frip_encode(const char *iname, const char *oname, frip_callback cb)
{
	std::auto_ptr<reader> r(mkreader(iname, cb));
	std::auto_ptr<writer> w(mkwriter(oname, r.get()));
	samples smp;

	if (r.get() == NULL) {
		log("unknown input type.");
		return false;
	}

	if (w.get() == NULL) {
		log("unknown output type.");
		return false;
	}

	if (!r->open(iname)) {
		log("read or format error in file '%s'.", iname);
		return false;
	}

	if (!w->open(oname)) {
		log("write or config error in file '%s'.", oname);
		return false;
	}

	while (r->read(smp, 2048)) {
		if (!w->write(smp)) {
			return false;
		}
	}

	return true;
}
