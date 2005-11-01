// $Id$

#include "all.h"

rpcm::rpcm(frip_callback cb) :
	reader(cb)
{
	mLittleEndian = false;
}

rpcm::~rpcm()
{
}

bool rpcm::read(samples &smp, size_t count)
{
	size_t osize = smp.size();

	count *= mChannels;

	if (count > mSamplesLeft)
		count = mSamplesLeft;

	if (count == 0) {
		log("rpcm: finished reading.");
		return false;
	}

	if (count % mChannels != 0) {
		log("rpcm: incomplete samples (%u), reading aborted.", count);
		return false;
	}

	smp.resize(osize + count);

	if (mLittleEndian) {
		for (samples::iterator it = smp.begin() + osize; it != smp.end(); ++it) {
			if (mSampleSize == 16) {
				short tmp;

				if (!in.read_short_le(tmp, &mChunkSize)) {
					log("rpcm: failed reading a sample (expecting %u more samples; count = %u).", mSamplesLeft, count);
					return false;
				}

				*it = tmp;
			}
		}
	} else {
		for (samples::iterator it = smp.begin() + osize; it != smp.end(); ++it) {
			if (mSampleSize == 16) {
				short tmp;

				if (!in.read_short_be(tmp, &mChunkSize)) {
					log("rpcm: failed reading a sample (expecting %u more samples; count = %u).", mSamplesLeft, count);
					return false;
				}

				*it = tmp;
			}
		}
	}

	mSamplesLeft -= count;

	UpdateStatus();

	return true;
}
