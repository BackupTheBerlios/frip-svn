// $Id$

#ifdef HAVE_lame
#include "all.h"

wlame::wlame(const reader *r) :
	writer(r)
{
	mLame = lame_init();
	mReady = false;
}

wlame::~wlame()
{
	if (mReady) {
		unsigned char tmp[LAME_MAXMP3BUFFER];
		int sz = lame_encode_finish(mLame, tmp, sizeof(tmp));

		if (sz != 0)
			out.write(tmp, sz);
	}
}

bool wlame::open(const char *fname)
{
#ifdef LAME_USES_VBR
	int qs[10] = { 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 };
#endif

	if (!writer::open(fname))
		return false;

	if (!out.open(fname, true))
		return false;

	lame_set_in_samplerate(mLame, mReader->GetSampleRate());
	lame_set_num_channels(mLame, mReader->GetChannels());
	lame_set_quality(mLame, 2);
	lame_set_mode(mLame, STEREO);
#ifdef LAME_USES_VBR
	lame_set_VBR(mLame, vbr_abr);
	lame_set_VBR_q(mLame, 9);
	lame_set_VBR_min_bitrate_kbps(mLame, 32);
	lame_set_VBR_max_bitrate_kbps(mLame, qs[mQuality]);
#endif

	set_tags();

	if (lame_init_params(mLame) == -1) {
		log("wlame: init_params() failed.");
		return false;
	}

	return (mReady = true);
}

void wlame::set_tags()
{
	string val;

	id3tag_init(mLame);
	id3tag_add_v2(mLame);
	id3tag_pad_v2(mLame);

	if (mReader->GetTag("ALBUM", val))
		id3tag_set_album(mLame, val.c_str());
	if (mReader->GetTag("ARTIST", val))
		id3tag_set_artist(mLame, val.c_str());
	if (mReader->GetTag("COMMENT", val))
		id3tag_set_comment(mLame, val.c_str());
	if (mReader->GetTag("DATE", val))
		id3tag_set_year(mLame, val.c_str());
	if (mReader->GetTag("TITLE", val))
		id3tag_set_title(mLame, val.c_str());
	if (mReader->GetTag("TRACKNUMBER", val))
		id3tag_set_track(mLame, val.c_str());
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
