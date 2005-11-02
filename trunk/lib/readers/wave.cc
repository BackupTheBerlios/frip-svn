// $Id$

#include "all.h"

rwave::rwave(frip_callback cb) :
	rpcm(cb)
{
	mRiffSize = 0;
	mLittleEndian = true;
}

rwave::~rwave()
{
}

bool rwave::open(const char *fname)
{
	unsigned tmp;

	if (!reader::open(fname))
		return false;

	if (!in.read_int_be(tmp)) {
		log("rwave: missing RIFF signature.");
		return false;
	} else if (tmp != WAV_ID_RIFF) {
		log("rwave: bad signature (not RIFF).");
		return false;
	}

	if (!in.read_int_le(mRiffSize)) {
		log("rwave: could not read riff size.");
		return false;
	} else {
		log("rwave: riff size = %u.", mRiffSize);
	}

	if (!in.read_int_be(tmp, &mRiffSize)) {
		log("rwave: missing WAVE signature.");
		return false;
	} else if (tmp != WAV_ID_WAVE) {
		log("rwave: invalid WAVE signature (expected %08X, found %08X).", WAV_ID_WAVE, tmp);
		return false;
	}

	while (mRiffSize != 0) {
		if (!in.read_int_be(tmp, &mRiffSize)) {
			log("rwave: could not read chunk id (%u bytes left).", mRiffSize);
			return false;
		}

		if (!in.read_int_le(mChunkSize, &mRiffSize)) {
			log("rwave: could not read subchunk length.");
			return false;
		}

		mRiffSize -= mChunkSize;

		if (!subchunk(tmp))
			return false;

		if (tmp == WAV_ID_DATA)
			return true;

		if (mChunkSize != 0) {
			log("rwave: skipping %u unused bytes from a subchunk.", mChunkSize);
			in.skip(mChunkSize);
		}
	}

	return false;
}

bool rwave::subchunk(unsigned id)
{
	short stmp;
	unsigned itmp;

	if (id == WAV_ID_FMT) {
		log("rwave: found the fmt chunk, %u bytes long.", mChunkSize);

		if (!in.read_short_le(stmp, &mChunkSize)) {
			log("rwave: fmt: no compression code.");
			return false;
		} else if (stmp != 1) {
			log("rwave: fmt: invalid compression code (only '1' is supported, '%u' was found).", stmp);
			return false;
		}

		if (!in.read_short_le(mFlowSpec.mChannels, &mChunkSize)) {
			log("rwave: fmt: no channel count.");
			return false;
		}

		if (!in.read_int_le(mFlowSpec.mSampleRate, &mChunkSize)) {
			log("rwave: fmt: no sample rate.");
			return false;
		}

		if (!in.read_int_le(itmp, &mChunkSize)) {
			log("rwave: fmt: avg. bytes per second not found.");
			return false;
		}

		if (!in.read_short_le(mBlockAlign, &mChunkSize)) {
			log("rwave: block align not found.");
			return false;
		}

		if (!in.read_short_le(mFlowSpec.mSampleSize, &mChunkSize)) {
			log("rwave: fmt: bit-per-sample not found.");
			return false;
		} else if (mFlowSpec.mSampleSize != 16) {
			log("rwave: fmt: unsupported bps: %u.", mFlowSpec.mSampleSize);
			return false;
		}
	}

	else if (id == WAV_ID_DATA) {
		mSamplesLeft = mSamplesTotal = mChunkSize / mFlowSpec.mChannels;
		log("rwave: found the data chunk, %u bytes long (%u wide samples).", mChunkSize, mSamplesTotal / (mFlowSpec.mSampleSize / 8));

		log("rwave: channels = %u", mFlowSpec.mChannels);
		log("rwave: sample rate = %u", mFlowSpec.mSampleRate);
		log("rwave: block align = %u", mBlockAlign);
		log("rwave: sample size = %u.", mFlowSpec.mSampleSize);

		{
			unsigned duration = GetFrameCount() / mFlowSpec.mSampleRate;
			log("rwave: duration = %u:%02u", duration / 60, duration % 60);
		}
	}

	else {
		log("rwave: unknown chunk %08X, %u bytes long.", id, mChunkSize);
		return false;
	}

	return true;
}
