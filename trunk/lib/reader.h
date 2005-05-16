// $Id$

#ifndef __reader_h
#define __reader_h

#include <stdio.h>
#include <vector>
#include "file.h"
#include "frip.h"
#include "log.h"

typedef std::vector<int> samples;

class reader
{
	// Last reporteed percentage.
	unsigned mLastReport;
protected:
	file in;
	bool mReady;
	unsigned short mChannels;
	frip_callback mCallBack;
	short mSampleSize;
	unsigned mSampleRate;
	// Report the progress to the host application.
	void UpdateStatus();
	// The total number of samples.
	unsigned mSamplesTotal;
	// The number of remaining samples.
	unsigned mSamplesLeft;
public:
	reader(frip_callback);
	virtual ~reader();
	virtual bool open(const char *fname);
	virtual bool read(samples &, size_t preferred_wide_sample_count = 2048) = 0;
	virtual const char *name() const = 0;
	virtual unsigned GetChannels() const { return mChannels; }
	virtual unsigned GetSampleSize() const { return mSampleSize; }
	virtual unsigned GetSampleCount() const { return mSamplesTotal; }
	virtual unsigned GetSampleRate() const { return mSampleRate; }
};

// Base class for linear PCM formats (aiff and wav)
class rpcm : public reader
{
protected:
	// Remaining size of the current chunk.
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
