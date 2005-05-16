// $Id$

#include "writer.h"

wwave::wwave(const reader *r) :
	writer(r)
{
}

wwave::~wwave()
{
}

void wwave::write(samples &smp)
{
	if (smp.size() & 1)
		return;

	if (mSampleSize != 16) {
		log("wwave: write failed: unsupported sample size: %u.", mSampleSize);
		return;
	}

	for (samples::const_iterator it = smp.begin(); it != smp.end(); it++) {
		if (mSampleSize == 16) {
			out.write_short_le(*it);
		}
	}

	smp.clear();
}

bool wwave::open(const char *fname)
{
	unsigned bps = mReader->GetSampleSize() / 8;
	unsigned pcmbytes = mReader->GetSampleCount() * mReader->GetChannels() * bps;

	if (!writer::open(fname))
		return false;

	if (!out.open(fname, true))
		return false;

	out.write("RIFF");
	out.write_int_le(pcmbytes + 44 - 8);
	out.write("WAVEfmt ");
	out.write_int_le(16);
	out.write_short_le(1);
	out.write_short_le(mReader->GetChannels());
	out.write_int_le(mReader->GetSampleRate());
	out.write_int_le(mReader->GetSampleRate() * mReader->GetChannels() * bps);
	out.write_short_le(mReader->GetChannels() * bps);
	out.write_short_le(mSampleSize = mReader->GetSampleSize());
	out.write("data");
	out.write_int_le(pcmbytes);

	return true;
}
