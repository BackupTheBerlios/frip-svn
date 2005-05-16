// $Id$

#include <memory>
#include <string.h>
#include "frip.h"
#include "log.h"
#include "reader.h"
#include "writer.h"

static reader * mkreader(const char *fname, frip_callback cb)
{
	const char *ext = strchr(fname, '.');

	if (ext++ != NULL) {
		if (strcasecmp(ext, ".aiff") == 0 || strcasecmp(ext, ".aif") == 0)
			return new raiff(cb);
		else if (strcasecmp(ext, ".wav") == 0)
			return new rwave(cb);
	}

	return NULL;
}

static writer * mkwriter(const char *fname, const reader *r)
{
	const char *ext = strchr(fname, '.');

	if (ext++ != NULL) {
		if (strcasecmp(ext, ".raw") == 0)
			return new wraw(r);
		else if (strcasecmp(ext, ".wav") == 0 || strcasecmp(ext, ".wave") == 0)
			return new wwave(r);
#ifdef HAVE_lame
		else if (strcasecmp(ext, ".mp3") == 0)
			return new wlame(r);
#endif
#ifdef HAVE_flac
		else if (strcasecmp(ext, ".flac") == 0)
			return new wflacf(r);
		else if (strcasecmp(ext, ".flacs") == 0)
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
		w->write(smp);
	}

	return true;
}
