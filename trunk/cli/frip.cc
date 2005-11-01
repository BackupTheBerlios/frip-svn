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
	"  -d ext     : target encoding when processing directories recursively\n"
	"  -l name    : write conversion log to the named file\n"
	"  -m channel : downmix to one channels (left, right)\n"
	"  -Q         : do not show conversion progress\n"
	"  -q val     : set output quality (float, defaults to 0.5)\n"
	"  -r         : recurse directories\n"
	"  -s ext     : only process input files with this suffix\n"
	"\n"
	"Send your bug reports to justin.forest@gmail.com\n"
	"";

static void show_stat(int percentage)
{
	const char *pgs = "XXXXXXXXXXXXXXXXXXXX....................";

	if (percentage < 0) {
		fprintf(stdout, "    status: failed.                                \r");
	} else if (percentage != 100) {
		char tmp[21];

		strncpy(tmp, pgs + 20 - (percentage / 5), 20);
		tmp[20] = 0;

		fprintf(stdout, "    status: %u%% (%20s)    \r", percentage, tmp);
	} else {
		fprintf(stdout, "    status: done.                                 \r");
	}

	fflush(stdout);
}

frip::frip()
{
	mVerbose = true;
	mLogName = "frip.log";
	mDefaultSuffix = "mp3";
	mQuality = 5;
	mRecurse = false;
	mMixType = mtNone;
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
	for (int ch; (ch = getopt(argc, argv, "d:l:m:Qq:rs:")) != -1; ) {
		switch (ch) {
		case 'd':
			mDefaultSuffix = optarg;
			break;
		case 'l':
			mLogName = optarg;
			break;
		case 'm':
			if (strcmp(optarg, "none") == 0)
				mMixType = mtNone;
			if (strcmp(optarg, "left") == 0)
				mMixType = mtLeft;
			else if (strcmp(optarg, "right") == 0)
				mMixType = mtRight;
			else if (strcmp(optarg, "both") == 0)
				mMixType = mtBoth;
			else {
				fprintf(stderr, "Unsupported -m argument, supported values are: none, left, right, both.\n");
				return false;
			}
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

	frip_set_log(mLogName.c_str());

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
	bool rc;

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

	rc = frip_encode(src.c_str(), dst.c_str(), mQuality, mMixType, mVerbose ? show_stat : NULL);

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
