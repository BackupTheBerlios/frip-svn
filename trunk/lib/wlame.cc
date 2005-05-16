// $Id$

#ifdef HAVE_lame
#include "writer.h"

wlame::wlame(const reader *r) :
	writer(r)
{
	mLame = lame_init();
}

wlame::~wlame()
{
}

bool wlame::open(const char *fname)
{
	if (!writer::open(fname))
		return false;

	if (!out.open(fname, true))
		return false;

	lame_set_in_samplerate(mLame, mReader->GetSampleRate());
	lame_set_num_channels(mLame, mReader->GetChannels());
	lame_set_quality(mLame, 5);
	lame_set_mode(mLame, STEREO);

	if (lame_init_params(mLame) == -1) {
		log("wlame: init_params() failed.");
		return false;
	}

	return true;
}

bool wlame::write(samples &smp)
{
	int len;
	std::vector<short> mp3;
	std::vector<short>::iterator dit;
	std::vector<unsigned char>coded;

	mp3.resize(smp.size());
	dit = mp3.begin();

	for (samples::const_iterator it = smp.begin(); it != smp.end(); ++it, ++dit) {
		*dit = *it;
	}

	smp.clear();
	coded.resize(mp3.size() * 2 + 7200);

	if ((len = lame_encode_buffer_interleaved(mLame, &mp3[0], mp3.size() / 2, &coded[0], coded.size())) > 0) {
		out.write(&coded[0], len);
	}

	return true;
}

#endif
