// $Id$

#include <memory>
#include <string.h>
#include "frip.h"
#include "log.h"
#include "reader.h"
#include "writer.h"

E bool frip_encode(const char *iname, const char *oname, frip_callback cb)
{
	const char *ext;
	std::auto_ptr<reader> r(0);
	std::auto_ptr<writer> w(0);
	samples smp;

	if ((ext = strrchr(iname, '.')) != NULL) {
		if (strcasecmp(ext, ".aiff") == 0 || strcasecmp(ext, ".aif") == 0)
			r.reset(new raiff(cb));
		else if (strcasecmp(ext, ".wav") == 0)
			r.reset(new rwave(cb));
	}

	if (r.get() == NULL) {
		log("unknown input type.");
		return false;
	}

	if ((ext = strrchr(oname, '.')) != NULL) {
		if (strcasecmp(ext, ".raw") == 0)
			w.reset(new wraw(r.get()));
		else if (strcasecmp(ext, ".wav") == 0 || strcasecmp(ext, ".wave") == 0)
			w.reset(new wwave(r.get()));
#ifdef HAVE_lame
		else if (strcasecmp(ext, ".mp3") == 0)
			w.reset(new wlame(r.get()));
#endif
#ifdef HAVE_flac
		else if (strcasecmp(ext, ".flac") == 0)
			w.reset(new wflacf(r.get()));
		else if (strcasecmp(ext, ".flacs") == 0)
			w.reset(new wflacs(r.get()));
#endif
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
