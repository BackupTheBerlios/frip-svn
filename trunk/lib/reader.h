// $Id$

#ifndef __reader_h
#define __reader_h

#include <stdio.h>
#include <vector>
#include "file.h"
#include "frip.h"
#include "log.h"

typedef std::vector<int> samples;

#define IFF_ID_FORM 0x464F524D /* "FORM" */
#define IFF_ID_AIFF 0x41494646 /* "AIFF" */
#define IFF_ID_AIFC 0x41494643 /* "AIFC" */
#define IFF_ID_COMM 0x434f4d4d /* "COMM" */
#define IFF_ID_SSND 0x53534e44 /* "SSND" */
#define IFF_ID_MPEG 0x4d504547 /* "MPEG" */
#define IFF_ID_FVER 0x46564552 /* "FVER" */

#define IFF_ID_NONE 0x4e4f4e45 /* "NONE" */ /* AIFF-C data format */
#define IFF_ID_2CBE 0x74776f73 /* "twos" */ /* AIFF-C data format */
#define IFF_ID_2CLE 0x736f7774 /* "sowt" */ /* AIFF-C data format */

#define WAV_ID_RIFF 0x52494646
#define WAV_ID_WAVE 0x57415645
#define WAV_ID_FMT  0x666D7420
#define WAV_ID_DATA 0x64617461

// The base class contains information that applies to all kinds of input.
// Anything specific must be moved to a separate class.
class reader
{
	// Last reporteed percentage.  Used to avoid calling the
	// user-defined progress callback when the percentage
	// didn't change.
	unsigned mLastReport;
protected:
	// The data is read from here.
	file in;
	// Set when the file is open and points to a sample.
	bool mReady;
	// The user-defined progress handler.
	frip_callback mCallBack;
	// The number of channels.
	unsigned short mChannels;
	// The number of bits in a sample.
	short mSampleSize;
	// The number of samples played per second, by one channel.
	unsigned mSampleRate;
	// The total number of samples.
	unsigned mSamplesTotal;
	// The number of remaining samples, per file.
	unsigned mSamplesLeft;
	// Report the progress to the host application.
	void UpdateStatus();
public:
	reader(frip_callback);
	virtual ~reader();
	virtual bool open(const char *fname);
	virtual bool read(samples &, size_t preferred_wide_sample_count = 2048) = 0;
	virtual const char *name() const = 0;
	// The number of channels.
	unsigned GetChannels() const { return mChannels; }
	// The number of bits in a sample.
	unsigned GetSampleSize() const { return mSampleSize; }
	// The number of samples in the file, total (for all channels).
	unsigned GetSampleCount() const { return mSamplesTotal; }
	// The number of samples played in a channel per second.
	unsigned GetSampleRate() const { return mSampleRate; }
	// The number of frames in the file (samples in one channel).
	unsigned GetFrameCount() const { return mSamplesTotal / mChannels; }
	// The size of a frame, in bytes (sum of all samples in a channel).
	unsigned GetFrameSize() const { return mSampleSize / 8 * mChannels; }
};

// Base class for linear PCM formats (aiff and wav)
class rpcm : public reader
{
protected:
	// Remaining size of the current chunk, in bytes.
	unsigned mChunkSize;
	// Set when the input data is LE.
	bool mLittleEndian;
public:
	rpcm(frip_callback);
	virtual ~rpcm();
	virtual bool open(const char *fname) = 0;
	virtual bool read(samples &, size_t);
	virtual const char *name() const = 0;
};

class raiff : public rpcm
{
	int mFormat;
	unsigned mChunkSize;
	unsigned mBlockOffset;
	unsigned mBlockSize;
public:
	raiff(frip_callback);
	~raiff();
	bool open(const char *fname);
	const char *name() const { return "aiff"; }
};

class rwave : public rpcm
{
	unsigned mRiffSize;
	unsigned short mBlockAlign;
protected:
	bool subchunk(unsigned id);
public:
	rwave(frip_callback);
	~rwave();
	bool open(const char *fname);
	const char *name() const { return "wave"; }
};

#endif
