// $Id$

#include "reader.h"

#define WAV_ID_RIFF 0x52494646
#define WAV_ID_WAVE 0x57415645
#define WAV_ID_FMT 0x666D7420
#define WAV_ID_DATA 0x64617461

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

		if (!in.read_short_le(mChannels, &mChunkSize)) {
			log("rwave: fmt: no channel count.");
			return false;
		} else {
			log("rwave: fmt: channels = %u", mChannels);
		}

		if (!in.read_int_le(mSampleRate, &mChunkSize)) {
			log("rwave: fmt: no sample rate.");
			return false;
		} else {
			log("rwave: fmt: sample rate = %u", mSampleRate);
		}

		if (!in.read_int_le(itmp, &mChunkSize)) {
			log("rwave: fmt: avg. bytes per second not found.");
			return false;
		} else {
			log("rwave: fmt: bytes per second: %u.", itmp);
		}

		if (!in.read_short_le(mBlockAlign, &mChunkSize)) {
			log("rwave: block align not found.");
			return false;
		} else {
			log("rwave: fmt: block align = %u", mBlockAlign);
		}

		if (!in.read_short_le(mSampleSize, &mChunkSize)) {
			log("rwave: fmt: bit-per-sample not found.");
			return false;
		} else if (mSampleSize != 16) {
			log("rwave: fmt: unsupported bps: %u.", mSampleSize);
			return false;
		} else {
			log("rwave: fmt: sample size: %u.", mSampleSize);
		}
	}

	else if (id == WAV_ID_DATA) {
		mSamplesLeft = mSamplesTotal = mChunkSize / mChannels;
		log("rwave: found the data chunk, %u bytes long (%u wide samples).", mChunkSize, mSamplesTotal / (mSampleSize / 8));
	}

	else {
		log("rwave: unknown chunk %08X, %u bytes long.", id, mChunkSize);
		return false;
	}

	return true;
}
