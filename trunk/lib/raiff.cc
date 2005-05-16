// $Id$

#include "reader.h"

#define IFF_ID_FORM 0x464F524D /* "FORM" */
#define IFF_ID_AIFF 0x41494646 /* "AIFF" */
#define IFF_ID_AIFC 0x41494643 /* "AIFC" */
#define IFF_ID_COMM 0x434f4d4d /* "COMM" */
#define IFF_ID_SSND 0x53534e44 /* "SSND" */
#define IFF_ID_MPEG 0x4d504547 /* "MPEG" */

#define IFF_ID_NONE 0x4e4f4e45 /* "NONE" */ /* AIFF-C data format */
#define IFF_ID_2CBE 0x74776f73 /* "twos" */ /* AIFF-C data format */
#define IFF_ID_2CLE 0x736f7774 /* "sowt" */ /* AIFF-C data format */

raiff::raiff(frip_callback cb) :
	rpcm(cb)
{
	mFormat = 0;
}

raiff::~raiff()
{
}

bool raiff::open(const char *fname)
{
	int tmp;

	if (!reader::open(fname))
		return false;

	if (!in.read_int_be(tmp) || tmp != IFF_ID_FORM) {
		log("raiff: missing FORM signature.");
		return false;
	}

	if (!in.read_int_be(mChunkSize)) {
		log("raiff: missing chunk size.");
		return false;
	}

	if (!in.read_int_be(mFormat, &mChunkSize)) {
		log("raiff: missing format identifier.");
		return false;
	}

	if (mFormat != IFF_ID_AIFF && mFormat != IFF_ID_AIFC) {
		log("raiff: unknown format: %08X.", mFormat);
		return false;
	}

	// Read additional chunks.
	while (mChunkSize > 0) {
		unsigned subtype, subsize;

		if (!in.read_int_be(subtype, &mChunkSize)) {
			log("raiff: failed to read subchunk id.");
			return false;
		}

		if (subtype == IFF_ID_COMM) {
			double ftmp;

			if (!in.read_int_be(subsize, &mChunkSize)) {
				log("raiff: could not read the COMM chunk size.");
				return false;
			}

			mChunkSize -= subsize;
			log("raiff: COMM chunk size: %u.", subsize);

			if (!in.read_short_be(mChannels, &subsize)) {
				log("raiff: COMM: could not read the number of channels.");
				return false;
			}

			if (!in.read_int_be(mSamplesTotal, &subsize)) {
				log("raiff: COMM: could not read the number of sample frames.");
				return false;
			} else {
				mSamplesTotal *= mChannels;
			}

			if (!in.read_short_be(mSampleSize, &subsize)) {
				log("raiff: COMM: could not read the size of a sample.");
				return false;
			}

			if (!in.read_float_be(ftmp, &subsize)) {
				log("raiff: COMM: could not read sample rate.");
				return false;
			} else {
				mSampleRate = static_cast<unsigned>(ftmp);
			}

			if (mFormat == IFF_ID_AIFC) {
				int tmp;

				if (!in.read_int_be(tmp, &subsize)) {
					log("raiff: COMM: could not read aifc extra.");
					return false;
				}

				switch (tmp) {
				case IFF_ID_NONE:
				case IFF_ID_2CBE:
					break;
				case IFF_ID_2CLE:
					mLittleEndian = true;
					break;
				default:
					log("raiff: unknown format: %08X.", tmp);
					return false;
				}
			}

			mSamplesLeft = mSamplesTotal;

			if (subsize != 0) {
				log("raiff: COMM: skipping %u bytes.", subsize);
				in.skip(subsize);
			}
		}

		else if (subtype == IFF_ID_SSND) {
			if (!in.read_int_be(subsize, &mChunkSize)) {
				log("raiff: could not read the SSND chunk size.");
				return false;
			}

			mChunkSize -= subsize;
			log("raiff: SSND chunk size: %u.", subsize);

			if (!in.read_int_be(mBlockOffset, &subsize)) {
				log("raiff: could not read block align from the SSND chunk.");
				return false;
			}

			if (!in.read_int_be(mBlockSize, &subsize)) {
				log("raiff: could not read block size from the SSND chunk.");
				return false;
			}

			log("raiff: channels: %u.", mChannels);
			log("raiff: sample size: %u.", mSampleSize);
			log("raiff: sample rate: %u.", (unsigned)mSampleRate);
			log("raiff: frames: %u.", mSamplesTotal);
			log("raiff: block offset: %u.", mBlockOffset);
			log("raiff: block size: %u.", mBlockSize);
			log("raiff: byte order: %s.", mLittleEndian ? "le" : "be");

			in.skip(mBlockOffset);

			mReady = (mSamplesTotal != 0);
			break;
		}

		else {
			if (!in.read_int_be(subsize, &mChunkSize)) {
				log("raiff: could not read the size of an unknown chunk.");
				return false;
			}

			mChunkSize -= subsize;
			log("raiff: unknown chunk id: %08X, size: %u.", subtype, subsize);

			in.skip(subsize);
		}
	}

	return mReady;
}
