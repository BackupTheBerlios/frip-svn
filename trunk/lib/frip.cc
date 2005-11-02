// $Id$

#include <memory>
#include <string.h>
#include "fripint.h"
#include "log.h"
#include "readers/all.h"
#include "writers/all.h"
#include "filters/all.h"

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

static writer * mkwriter(const char *fname)
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

void * frip_create(const char *iname, const char *oname)
{
	ruler *f = new ruler(iname, oname);
	return f;
}

void frip_set_quality(void *f, int q)
{
	reinterpret_cast<ruler *>(f)->set_quality(q);
}

void frip_set_log(void *f, const char *name)
{
	reinterpret_cast<ruler *>(f)->set_log(name);
}

void frip_set_filter(void *f, const char *name, const char *args)
{
	reinterpret_cast<ruler *>(f)->add_filter(name, args);
}

void frip_run(void *f)
{
	reinterpret_cast<ruler *>(f)->run();
}

void frip_finish(void **f)
{
	ruler **ff = reinterpret_cast<ruler **>(f);
	delete *ff;
	*ff = NULL;
}

ruler::ruler(const char *iname, const char *oname)
{
	mNameIn = iname;
	mNameOut = oname;
	mReader = mkreader(iname, NULL);
	mWriter = mkwriter(oname);
}

ruler::~ruler()
{
	for (filters::iterator it = mFilters.begin(); it != mFilters.end(); ++it)
		delete *it;
	if (mReader != NULL)
		delete mReader;
	if (mWriter != NULL)
		delete mWriter;
}

void ruler::set_log(const char *name)
{
}

void ruler::set_quality(int)
{
}

void ruler::add_filter(const char *name, const char *arg)
{
	if (strcmp(name, "downmix") == 0)
		mFilters.push_back(new downmix(arg));
}

void ruler::run()
{
	samples smp;

	try {
		if (!mReader->open(mNameIn.c_str())) {
			log("read or format error in file '%s'.", mNameIn.c_str());
			return;
		}

		while (mReader->read(smp, 2048)) {
			flowspec fs = mReader->GetFlowSpec();

			for (filters::iterator it = mFilters.begin(); it != mFilters.end(); ++it)
				(*it)->process(fs, smp);

			mWriter->write(fs, smp);
		}
	} catch (fripex &e) {
		fprintf(stderr, "exception: %s.\n", e.str().c_str());
		log("exception: %s.\n", e.str().c_str());
	}
}
