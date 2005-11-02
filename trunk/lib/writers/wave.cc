// $Id$

#include "all.h"

wwave::wwave(const char *fname)
{
	mFileName = fname;
	mBytesWritten = 0;
}

wwave::~wwave()
{
	if (mOut.isopen())
		flush();
}

void wwave::write(const flowspec &fs, samples &smp)
{
	size_t todo;

	if (mFlowSpec.isnull())
		mFlowSpec = fs;
	else if (mFlowSpec != fs)
		throw werr("flow specification changed; not yet supported");

	if (!mOut.isopen())
		open();

	if ((todo = (smp.size() - (smp.size() % mFlowSpec.mChannels))) == 0)
		return;

	if (mFlowSpec.mSampleSize != 16)
		throw werr("wave writer only supports 16 bits per sample");

	for (samples::const_iterator it = smp.begin(), lim = it + todo; it != lim; it++) {
		if (mFlowSpec.mSampleSize == 16) {
			mOut.write_short_le(*it);
			mBytesWritten += 2;
		}
	}

	smp.erase(smp.begin(), smp.begin() + todo);
}

void wwave::open()
{
	if (!mOut.open(mFileName.c_str(), true))
		throw werr("could not open output for writing");
	mOut.skip(44);
}

void wwave::flush()
{
	mOut.rewind(0);
	mOut.write("RIFF");
	mOut.write_int_le(mBytesWritten + 44 - 8);
	mOut.write("WAVEfmt ");
	mOut.write_int_le(16);
	mOut.write_short_le(1);
	mOut.write_short_le(mFlowSpec.mChannels);
	mOut.write_int_le(mFlowSpec.mSampleRate);
	mOut.write_int_le(mFlowSpec.mSampleRate * mFlowSpec.mChannels * mFlowSpec.mSampleSize);
	mOut.write_short_le(mFlowSpec.mChannels * mFlowSpec.mSampleSize);
	mOut.write_short_le(mFlowSpec.mSampleSize);
	mOut.write("data");
	mOut.write_int_le(mBytesWritten);
}
