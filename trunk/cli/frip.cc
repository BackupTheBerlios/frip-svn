// $Id$

#include <dirent.h>
#include <libgen.h>
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
	"  -d ext      : target encoding when processing directories recursively\n"
	"  -f name     : add the named filter\n"
	"  -F arg      : a parameter to the filter\n"
	"  -l name     : write conversion log to the named file\n"
	"  -Q          : do not show conversion progress\n"
	"  -q val      : set output quality (float, defaults to 0.5)\n"
	"  -r          : recurse directories\n"
	"  -s ext      : only process input files with this suffix\n"
	"\n"
	"Send your bug reports to justin.forest@gmail.com\n"
	"";

frip::frip()
{
	mVerbose = true;
	mLogName = "frip.log";
	mDefaultSuffix = "mp3";
	mQuality = 5;
	mRecurse = false;
}

frip::~frip()
{
}

bool frip::passes(const string &fn) const
{
	if (mFilter.length() == 0)
		return true;

	if (fn.length() < mFilter.length()) {
		if (mVerbose)
			printf("  - \"%s\" != \"%s\"\n", fn.c_str(), mFilter.c_str());
		return false;
	}

	if (fn.compare(fn.length() - mFilter.length(), mFilter.length(), mFilter) != 0) {
		if (mVerbose)
			printf("  - \"%s\" != \"%s\"\n", fn.c_str(), mFilter.c_str());
		return false;
	}
	
	return true;
}

bool frip::run(int argc, char * const * argv)
{
	const char *filter = NULL;

	for (int ch; (ch = getopt(argc, argv, "d:f:F:l:Qq:rs:")) != -1; ) {
		switch (ch) {
		case 'd':
			mDefaultSuffix = optarg;
			break;
		case 'f':
			filter = optarg;
			break;
		case 'F':
			if (filter == NULL) {
				fprintf(stderr, "Filter name not set, -F not expected here.\n");
				return false;
			}
			mFilters[filter] = optarg;
			filter = NULL;
			break;
		case 'l':
			mLogName = optarg;
			break;
		case 'Q':
			mVerbose = false;
			break;
		case 'q':
			mQuality = static_cast<int>(atoi(optarg));
			break;
		case 'r':
			mRecurse = true;
			break;
		case 's':
			mFilter = optarg;
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

	while (argc >= 2) {
		if (is_file(argv[0])) {
			do_file(argv[0], argv[1]);
		} else if (is_dir(argv[0])) {
			if (!mRecurse) {
				fprintf(stderr, "Please specify the -r switch to convert directories.\n");
				return false;
			}

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
	void *frip;

	if (!passes(src))
		return true;

	if (mVerbose) {
		const char *tsrc = strrchr(src.c_str(), '/');
		const char *tdst = strrchr(dst.c_str(), '/');

		if (tsrc == NULL)
			tsrc = src.c_str();
		else
			++tsrc;

		if (tdst == NULL)
			tdst = dst.c_str();
		else
			++tdst;

		fprintf(stdout, "Converting: %s\n        to: %s (%d)\n      done: n/a\r", tsrc, tdst, mQuality);
		fflush(stdout);
	}

	if ((frip = frip_create(src.c_str(), dst.c_str())) == NULL)
		return false;

	for (filters::const_iterator it = mFilters.begin(); it != mFilters.end(); ++it) {
		frip_set_filter(frip, it->first.c_str(), it->second.c_str());
	}

	frip_run(frip);
	frip_finish(&frip);

	return true;
}

bool frip::do_dir(string src, string dst)
{
	DIR *d;
	dirent *e;

	if (strcmp(dst.c_str(), ".") == 0)
		dst = basename(src.c_str());

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
