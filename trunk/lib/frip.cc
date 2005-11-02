// $Id$

#include <memory>
#include <string.h>
#include "frip.h"
#include "log.h"
#include "readers/all.h"
#include "writers/all.h"

static inline bool equals(const char *a, const char *b)
{
	return strcasecmp(a, b) == 0;
}

static reader * mkreader(const char *fname, frip_callback cb)
{
	const char *ext = strrchr(fname, '.');

	if (ext != NULL) {
		if (equals(ext, ".aif") || equals(ext, ".aiff"))
			return new raiff(cb);
		else if (equals(ext, ".wav") || equals(ext, ".wave"))
			return new rwave(cb);
#ifdef HAVE_flac
		else if (equals(ext, ".flac"))
			return new rflac(cb);
#endif
	}

	return NULL;
}

static writer * mkwriter(const char *fname, const reader *r)
{
	const char *ext = strrchr(fname, '.');

	if (ext != NULL) {
		if (equals(ext, ".raw"))
			return new wraw(fname);
		else if (equals(ext, ".wav") || equals(ext, ".wave"))
			return new wwave(fname);
		else if (equals(ext, ".aif") || equals(ext, ".aiff"))
			return new waiff(fname);
#ifdef HAVE_lame
		else if (equals(ext, ".mp3"))
			return new wlame(fname);
#endif
#ifdef HAVE_flac
		else if (equals(ext, ".flac"))
			return new wflacf(fname);
		/*
		else if (equals(ext, ".flacs"))
			return new wflacs(fname);
		*/
#endif
	}

	return NULL;
}

bool frip_encode(const char *iname, const char *oname, int quality, mixtype_t mix, frip_callback cb)
{
	std::auto_ptr<reader> r(mkreader(iname, cb));
	std::auto_ptr<writer> w(mkwriter(oname, r.get()));
	samples smp;

	try {
		if (r.get() == NULL) {
			log("unknown input type: %s.", iname);
			return false;
		}

		if (w.get() == NULL) {
			log("unknown output type: %s.", oname);
			return false;
		}

		if (!r->open(iname)) {
			log("read or format error in file '%s'.", iname);
			return false;
		}

		while (r->read(smp, 2048)) {
			flowspec fs = r->GetFlowSpec();
			w->write(fs, smp);
		}
	} catch (fripex &e) {
		fprintf(stderr, "exception: %s.\n", e.str().c_str());
		log("exception: %s.\n", e.str().c_str());
	}

	return true;
}
