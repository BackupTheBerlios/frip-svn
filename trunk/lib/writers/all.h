// $Id$
//
// Writer declaration.  The way it works is:
//
// 1. You create an instance of a "writer" subclass.
//    Usually this leads to no visible result, like
//    creation of files or anything else.
//
// 2. You call write().  If the file is not open yet,
//    it will open.  Then the data is written.

#ifndef __writer_h
#define __writer_h

#ifdef HAVE_flac
# include <FLAC++/all.h>
#endif
#ifdef HAVE_lame
# include <lame/lame.h>
#endif
#include "../fripint.h"
#include "../file.h"

class werr : public fripex
{
	std::string mText;
public:
	werr(const std::string &msg);
	~werr() { }
	const std::string& str() const { return mText; }
};

class writer
{
protected:
	int mQuality;
	flowspec mFlowSpec;
public:
	writer();
	virtual ~writer();
	virtual void write(const flowspec &fs, samples &smp) = 0;
	virtual const char * name() const = 0;
	virtual void set_quality(int);
};

class wraw : public writer
{
	std::string mFileName;
	file mOut;
public:
	wraw(const char *fname);
	~wraw();
	void write(const flowspec &fs, samples &smp);
	const char * name() const { return "raw"; }
};

class wwave : public writer
{
	file mOut;
	unsigned int mBytesWritten;
	std::string mFileName;
private:
	void open();
	void flush();
public:
	wwave(const char *fname);
	~wwave();
	void write(const flowspec &fs, samples &smp);
	const char * name() const { return "wave"; }
};

class waiff : public writer
{
	file mOut;
	std::string mFileName;
	unsigned int mSamplesWritten;
private:
	void open();
	void flush();
public:
	waiff(const char *fname);
	~waiff();
	void write(const flowspec &fs, samples &smp);
	const char * name() const { return "aiff"; }
};

#ifdef HAVE_flac
// The streamed version of FLAC.
class wflacs : public writer, public FLAC::Encoder::Stream
{
	bool mIsOpen;
protected:
	void show_stat() const;
public:
	wflacs();
	~wflacs();
	void write(const flowspec &fs, samples &smp);
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
	std::string mFileName;
	void open();
protected:
	void show_stat() const;
public:
	wflacf(const char *fname);
	~wflacf();
	void write(const flowspec &fs, samples &smp);
	const char * name() const { return "flac"; }
};
#endif

#ifdef HAVE_lame
class wlame : public writer
{
	lame_global_flags *mLame;
	void set_tags();
	bool mReady;
	file mOut;
	std::string mFileName;
	void open();
public:
	wlame(const char *fname);
	~wlame();
	void write(const flowspec &fs, samples &smp);
	const char * name() const { return "lame"; }
};
#endif

#endif
