// $Id$

#ifndef __writer_h
#define __writer_h

#ifdef HAVE_flac
# include <FLAC++/all.h>
#endif
#ifdef HAVE_lame
# include <lame/lame.h>
#endif
#include "file.h"
#include "reader.h"

class writer
{
protected:
	file out;
	const reader *mReader;
	unsigned mSampleSize;
	int mQuality;
public:
	writer(const reader *);
	virtual ~writer();
	virtual bool write(samples &smp) = 0;
	virtual bool open(const char *fname);
	virtual const char * name() const = 0;
	virtual void set_quality(int);
};

class wraw : public writer
{
public:
	wraw(const reader *);
	~wraw();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "raw"; }
};

class wwave : public writer
{
public:
	wwave(const reader *);
	~wwave();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "wave"; }
};

class waiff : public writer
{
public:
	waiff(const reader *);
	~waiff();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "aiff"; }
};

#ifdef HAVE_flac
// The streamed version of FLAC.
class wflacs : public writer, public FLAC::Encoder::Stream
{
	bool mIsOpen;
	unsigned mChannels;
protected:
	void show_stat() const;
public:
	wflacs(const reader *);
	~wflacs();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "flac"; }
protected:
	// Callback handlers.
	::FLAC__StreamEncoderWriteStatus write_callback(const FLAC__byte buffer[], unsigned bytes, unsigned samples, unsigned current_frame);
	void metadata_callback(const ::FLAC__StreamMetadata *metadata);
};

// The file version of flac.
class wflacf : public writer, public FLAC::Encoder::File
{
	bool mIsOpen;
	unsigned mChannels;
protected:
	void show_stat() const;
public:
	wflacf(const reader *);
	~wflacf();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "flac"; }
};
#endif

#ifdef HAVE_lame
class wlame : public writer
{
	lame_global_flags *mLame;
	void set_tags();
	// Set when the file is ready for writing.
	bool mReady;
public:
	wlame(const reader *);
	~wlame();
	bool write(samples &smp);
	bool open(const char *fname);
	const char * name() const { return "lame"; }
};
#endif

#endif
