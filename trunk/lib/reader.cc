// $Id$

#include "reader.h"

reader::reader(frip_callback cb)
{
	mReady = false;
	mCallBack = cb;
	mSamplesTotal = 0;
	mSamplesLeft = 0;
	mLastReport = 0;
}

reader::~reader()
{
}

bool reader::open(const char *fname)
{
	mLastReport = 0;
	return in.open(fname);
}

void reader::AddTag(const string &name, const string &value)
{
	mTags.push_back(std::pair<string, string>(name, value));
	log("reader: tag: %s = \"%s\"", name.c_str(), value.c_str());
}

void reader::UpdateStatus()
{
	if (mCallBack != NULL) {
		unsigned one = mSamplesTotal / 100;
		unsigned now = (mSamplesTotal - mSamplesLeft) / one;

		if (now != mLastReport) {
			mLastReport = now;
			mCallBack(now);
		}
	}
}

bool reader::GetTag(const string &name, string &value) const
{
	for (tagset::const_iterator it = mTags.begin(); it != mTags.end(); ++it) {
		if (it->first.compare(name) == 0) {
			value = it->second;
			log("reader: returning value \"%s\" for %s.", it->second.c_str(), it->first.c_str());
			return true;
		}
	}

	return false;
}
