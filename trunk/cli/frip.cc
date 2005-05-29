// $Id$

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "frip.h"

static const char *usagemsg =
	"Usage: frip [options] [in out [...]]\n"
	"\n"
	"Options:\n"
	"  -l name : write conversion log to the named file\n"
	"  -q      : do not show conversion progress\n"
	"\n"
	"Send your bug reports to justin.forest@gmail.com\n"
	"";

static void show_stat(unsigned int percentage)
{
	static unsigned rot = 0;
	const char *pgs = "XXXXXXXXXXXXXXXXXXXX....................";
	const char pgt[] = "tHe rEvOlUtIoN wIlL bE sYnThEsIzEd-+-+";

	if (percentage != 100) {
		char tmp[21];

		strncpy(tmp, pgs + 20 - (percentage / 5), 20);
		tmp[20] = 0;

		fprintf(stdout, "      done: %u%% (%20s) (%c)    \r", percentage, tmp, pgt[rot++ % (sizeof(pgt) - 1)]);
	} else {
		fprintf(stdout, "      done: 100%%                                 \r");
	}

	fflush(stdout);
}

frip::frip()
{
	mVerbose = true;
	mLogName = "frip.log";
}

frip::~frip()
{
}

bool frip::run(int argc, char * const * argv)
{
	for (int ch; (ch = getopt(argc, argv, "ql:")) != -1; ) {
		switch (ch) {
		case 'l':
			mLogName = optarg;
			break;
		case 'q':
			mVerbose = false;
			break;
		default:
			fprintf(stderr, "%s", usagemsg);
			return false;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		fprintf(stdout, "%s", usagemsg);
		return false;
	}

	if (argc % 2 != 0) {
		fprintf(stderr, "Not enough file names.\n");
		return false;
	}

	frip_set_log(mLogName);

	while (argc >= 2) {
		if (mVerbose) {
			fprintf(stdout, "Converting: %s\n        to: %s\n      done: n/a\r", argv[0], argv[1]);
			fflush(stdout);
		}

		do_file(argv[0], argv[1]);

		if (mVerbose)
			fprintf(stdout, "\n");

		argc -= 2;
		argv += 2;
	}

	return true;
}

bool frip::do_file(const char *src, const char *dst)
{
	frip_encode(src, dst, mVerbose ? show_stat : NULL);
	return true;
}

int main(int argc, char * const * argv)
{
	frip f;
	return f.run(argc, argv);
}
