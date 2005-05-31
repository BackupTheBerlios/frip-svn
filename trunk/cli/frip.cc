// $Id$

#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "frip.h"

static const char *usagemsg =
	"Usage: frip [options] [in out [...]]\n"
	"\n"
	"Options:\n"
	"  -l name : write conversion log to the named file\n"
	"  -q      : do not show conversion progress\n"
	"  -s ext  : target encoding when processing directories recursively\n"
	"\n"
	"Send your bug reports to justin.forest@gmail.com\n"
	"";

static void show_stat(int percentage)
{
	static unsigned rot = 0;
	const char *pgs = "XXXXXXXXXXXXXXXXXXXX....................";
	const char pgt[] = "tHe rEvOlUtIoN wIlL bE sYnThEsIzEd-+-+";

	if (percentage < 0) {
		fprintf(stdout, "      done: failed.                                \r");
	} else if (percentage != 100) {
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
	mDefaultSuffix = "mp3";
}

frip::~frip()
{
}

bool frip::run(int argc, char * const * argv)
{
	for (int ch; (ch = getopt(argc, argv, "l:qs:")) != -1; ) {
		switch (ch) {
		case 'l':
			mLogName = optarg;
			break;
		case 'q':
			mVerbose = false;
			break;
		case 's':
			mDefaultSuffix = optarg;
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

	frip_set_log(mLogName.c_str());

	while (argc >= 2) {
		if (is_file(argv[0])) {
			do_file(argv[0], argv[1]);
		} else if (is_dir(argv[0])) {
			do_dir(argv[0], argv[1]);
		} else {
			fprintf(stderr, "Unknown node type: %s, skipped.\n", argv[0]);
		}

		argc -= 2;
		argv += 2;
	}

	return true;
}

bool frip::do_file(string src, string dst)
{
	bool rc;

	if (mVerbose) {
		fprintf(stdout, "Converting: %s\n        to: %s\n      done: n/a\r", src.c_str(), dst.c_str());
		fflush(stdout);
	}

	rc = frip_encode(src.c_str(), dst.c_str(), mVerbose ? show_stat : NULL);

	if (mVerbose) {
		show_stat(rc ? 100 : -1);
		fprintf(stdout, "\n");
	}

	return true;
}

bool frip::do_dir(string src, string dst)
{
	DIR *d;
	dirent *e;

	if (is_dir(dst)) {
		;
	} else if (mkdir(dst)) {
		;
	} else {
		fprintf(stderr, "Destination is not a directory: %s\n", dst.c_str());
		return false;
	}

	if ((d = opendir(src.c_str())) == NULL)
		return false;

	while ((e = readdir(d)) != NULL) {
		if (e->d_namlen != 0) {
			string tmp = e->d_name;

			if (tmp[0] == '.')
				continue;

			if (is_dir(src + "/" + tmp)) {
				do_dir(src + "/" + tmp, dst + "/" + tmp);
			} else {
				string ndst = dst + "/" + tmp;

				for (std::string::iterator it = ndst.end(); it != ndst.begin(); ) {
					--it;

					if (*it == '.') {
						ndst.resize(it - ndst.begin());
						break;
					}
				}

				ndst += "." + mDefaultSuffix;
				do_file(src + "/" + tmp, ndst);
			}
		}
	}

	closedir(d);
	return true;
}

bool frip::is_file(const string &fname)
{
	struct stat st;

	if (stat(fname.c_str(), &st) != 0)
		return false;

	return (st.st_mode & (S_IFREG|S_IFLNK)) ? true : false;
}

bool frip::is_dir(const string &fname)
{
	struct stat st;

	if (stat(fname.c_str(), &st) != 0)
		return false;

	return (st.st_mode & S_IFDIR) ? true : false;
}

bool frip::mkdir(const string &fname)
{
	return ::mkdir(fname.c_str(), 0755) == 0;
}

int main(int argc, char * const * argv)
{
	frip f;
	return f.run(argc, argv);
}
