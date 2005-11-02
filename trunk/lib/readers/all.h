// $Id$

#ifndef __reader_h
#define __reader_h

#ifdef HAVE_flac
# include <FLAC++/all.h>
#endif
#include "../fripint.h"
#include "../file.h"

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
	// Input flow specification.
	flowspec mFlowSpec;
	// The total number of samples.
	unsigned mSamplesTotal;
	// The number of remaining samples, per file.
	unsigned mSamplesLeft;
	// Standard tags.
	tagset mTags;
	// Appends a tag to the list.
	void AddTag(const string &, const string &);
	// Report the progress to the host application.
	void UpdateStatus();
public:
	reader(frip_callback);
	virtual ~reader();
	virtual bool open(const char *fname);
	virtual bool read(samples &, size_t preferred_wide_sample_count = 2048) = 0;
	virtual const char *name() const = 0;
	const flowspec & GetFlowSpec() const { return mFlowSpec; }
	// The number of samples in the file, total (for all channels).
	unsigned GetSampleCount() const { return mSamplesTotal; }
	// The number of frames in the file (samples in one channel).
	unsigned GetFrameCount() const { return mSamplesTotal / mFlowSpec.mChannels; }
	// The size of a frame, in bytes (sum of all samples in a channel).
	unsigned GetFrameSize() const { return mFlowSpec.mSampleSize / 8 * mFlowSpec.mChannels; }
	// Returns the value of a tag.
	bool GetTag(const string &name, string &value) const;
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

#ifdef HAVE_flac
class rflac : public reader, public FLAC::Decoder::Stream
{
	// Used to pass the temporary buffer to the callback handler.
	samples *mSamples;
	// Total file size.
	size_t mFileSize;
	// The amount of processed data.
	size_t mProcessedSize;
	// Set when the file is over.
	bool mEOF;
protected:
	// FLAC.
	::FLAC__StreamDecoderReadStatus read_callback(FLAC__byte buffer[], unsigned *bytes);
	::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
	void metadata_callback(const ::FLAC__StreamMetadata *metadata);
	void error_callback(::FLAC__StreamDecoderErrorStatus status);
	// reader.
	bool read(samples &, size_t preferred_wide_sample_count);
	// Parse a tag.
	void add_tag(const char *);
public:
	rflac(frip_callback);
	~rflac();
	bool open(const char *fname);
	const char *name() const { return "flac"; }
};
#endif

#endif
