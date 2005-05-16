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
	fprintf(stdout, "      done: %u%%    \r", percentage);
	fflush(stdout);
}

int main(int argc, char * const * argv)
{
	bool verbose = true;
	const char *logname = "stderr";

	for (int ch; (ch = getopt(argc, argv, "ql:")) != -1; ) {
		switch (ch) {
		case 'l':
			logname = optarg;
			break;
		case 'q':
			verbose = false;
			break;
		default:
			fprintf(stderr, "%s", usagemsg);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		fprintf(stdout, "%s", usagemsg);
		return 0;
	}

	if (argc % 2 != 0) {
		fprintf(stderr, "Not enough file names.\n");
		return 1;
	}

	frip_set_log(logname);

	while (argc >= 2) {
		if (verbose) {
			fprintf(stdout, "Converting: %s\n        to: %s\n      done: n/a\r", argv[0], argv[1]);
			fflush(stdout);
		}

		frip_encode(argv[0], argv[1], verbose ? show_stat : NULL);

		if (verbose)
			fprintf(stdout, "\n");

		argc -= 2;
		argv += 2;
	}

	return 0;
}
