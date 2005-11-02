// $Id$

#include "all.h"

waiff::waiff(const char *fname)
{
	mFileName = fname;
	mSamplesWritten = 0;
}

waiff::~waiff()
{
	if (mOut.isopen())
		flush();
}

void waiff::write(const flowspec &fs, samples &smp)
{
	size_t todo;

	if (mFlowSpec.isnull())
		mFlowSpec = fs;
	else if (mFlowSpec != fs)
		throw werr("flow specification changed; not yet supported");
	else
		mFlowSpec = fs;

	if (!mOut.isopen())
		open();

	if (mFlowSpec.mSampleSize != 16)
		throw werr("unsupported sample size");

	if ((todo = smp.size() - (smp.size() % mFlowSpec.mChannels)))
		return;

	for (samples::const_iterator it = smp.begin(), lim = it + todo; it != lim; it++) {
		if (mFlowSpec.mSampleSize == 16) {
			mOut.write_short_le(*it);
			mSamplesWritten++;
		}
	}

	smp.erase(smp.begin(), smp.begin() + todo);
}

void waiff::open()
{
	if (!mOut.open(mFileName.c_str(), true))
		throw werr("could not open output for writing");
}

void waiff::flush()
{
	int mode = IFF_ID_AIFC;
	unsigned offset = 2280;
	unsigned sz_ssnd = mSamplesWritten * mFlowSpec.mChannels * mFlowSpec.mSampleSize;

	mOut.rewind(0);

	if (mode == IFF_ID_AIFC)
		sz_ssnd += offset + 8;

	mOut.write_int_be(IFF_ID_FORM);
	mOut.write_int_be(sz_ssnd + 56); // chunk size

	mOut.write_int_be(mode);

	mOut.write_int_be(IFF_ID_FVER);
	mOut.write_int_be(4);
	mOut.write_int_be(0xA2805140);

	mOut.write_int_be(IFF_ID_COMM);
	mOut.write_int_be(24); // chunk size
	mOut.write_short_be(mFlowSpec.mChannels);
	mOut.write_int_be(mFlowSpec.mChannels * mSamplesWritten);
	mOut.write_short_be(mFlowSpec.mSampleSize);
	mOut.write_float_be(mFlowSpec.mSampleRate);
	mOut.write_int_be(mode == IFF_ID_AIFC ? IFF_ID_2CLE : 0);
	mOut.write_short_be(0);

	mOut.write_int_be(IFF_ID_SSND);
	mOut.write_int_be(sz_ssnd);

	if (mode == IFF_ID_AIFC) {
		mOut.write_int_be(offset); // block offset
		mOut.write_int_be(0); // block size
	}

	while (offset-- != 0) {
		char x = 0;
		mOut.write(&x, 1);
	}
}
