// $Id$

#include "writer.h"

waiff::waiff(const reader *r) :
	writer(r)
{
}

waiff::~waiff()
{
}

bool waiff::write(samples &smp)
{
	if (smp.size() & 1)
		return true;

	if (mSampleSize != 16) {
		log("waiff: write failed: unsupported sample size: %u.", mSampleSize);
		return false;
	}

	for (samples::const_iterator it = smp.begin(); it != smp.end(); it++) {
		if (mSampleSize == 16) {
			out.write_short_le(*it);
		} else {
			log("waiff: unsupported output sample size: %u.", mSampleSize);
			return false;
		}
	}

	smp.clear();
	return true;
}

bool waiff::open(const char *fname)
{
	int mode = IFF_ID_AIFC;
	unsigned offset = 2280;
	unsigned sz_ssnd = mReader->GetFrameCount() * mReader->GetFrameSize();

	if (!writer::open(fname))
		return false;

	if (!out.open(fname, true))
		return false;

	log("waiff: sample size = %u", mSampleSize);
	log("waiff: payload size = %u", sz_ssnd);

	if (mode == IFF_ID_AIFC)
		sz_ssnd += offset + 8;

	out.write_int_be(IFF_ID_FORM);
	out.write_int_be(sz_ssnd + 56); // chunk size

	out.write_int_be(mode);

	out.write_int_be(IFF_ID_FVER);
	out.write_int_be(4);
	out.write_int_be(0xA2805140);

	out.write_int_be(IFF_ID_COMM);
	out.write_int_be(24); // chunk size
	out.write_short_be(mReader->GetChannels());
	out.write_int_be(mReader->GetFrameCount());
	out.write_short_be(mReader->GetSampleSize());
	out.write_float_be(mReader->GetSampleRate());
	out.write_int_be(mode == IFF_ID_AIFC ? IFF_ID_2CLE : 0);
	out.write_short_be(0);

	out.write_int_be(IFF_ID_SSND);
	out.write_int_be(sz_ssnd);

	if (mode == IFF_ID_AIFC) {
		out.write_int_be(offset); // block offset
		out.write_int_be(0); // block size
	}

	while (offset-- != 0) {
		char x = 0;
		out.write(&x, 1);
	}

	return true;
}
