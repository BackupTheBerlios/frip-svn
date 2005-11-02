// $Id$

#ifdef HAVE_flac
#include "all.h"

wflacf::wflacf(const char *fname)
{
	mFileName = fname;
	mIsOpen = false;
}

wflacf::~wflacf()
{
}

void wflacf::write(const flowspec &fs, samples &smp)
{
	size_t todo;

	if (mFlowSpec.isnull())
		mFlowSpec = fs;
	else if (mFlowSpec != fs)
		throw werr("flow specification mismatch");

	if (!mIsOpen)
		open();

	if ((todo = smp.size() - (smp.size() % mFlowSpec.mChannels)) == 0)
		return;

	if (!process_interleaved(&smp[0], todo / mFlowSpec.mChannels))
		throw(get_state().as_cstring());
	else
		smp.erase(smp.begin(), smp.begin() + todo);
}

void wflacf::open()
{
	// encoder settings.
	bool midLoose = false, midSide = false, exModel = false;
	int lpcOrder = 0, blockSize = 1152;
	int resMin = 0, resMax = 0;

	if (!is_valid())
		throw werr("not a valid flac stream");

	if (!set_filename(mFileName.c_str()))
		throw werr(std::string("could not set file name to ") + mFileName);

	if (!set_verify(true))
		throw werr("could not enable flac verification");

	switch (mQuality) {
	case 0:
		resMin = resMax = 2;
		break;
	case 1:
		resMin = resMax = 2;
		midLoose = true;
		break;
	case 2:
		resMax = 3;
		midSide = true;
		break;
	case 3:
		lpcOrder = 6;
		blockSize = 4608;
		resMin = resMax = 3;
		break;
	case 4:
		lpcOrder = 8;
		blockSize = 4608;
		resMin = resMax = 3;
		midLoose = true;
		break;
	case 5:
		lpcOrder = 8;
		blockSize = 4608;
		resMin = resMax = 3;
		midSide = true;
		break;
	case 6:
		lpcOrder = 8;
		blockSize = 4608;
		resMax = 4;
		midSide = true;
		break;
	case 7:
		lpcOrder = 8;
		blockSize = 4608;
		resMax = 6;
		midSide = true;
		exModel = true;
		break;
	case 8:
		lpcOrder = 12;
		blockSize = 4608;
		resMax = 6;
		midSide = true;
		exModel = true;
		break;
	}

	set_do_exhaustive_model_search(true);

	if (!set_blocksize(blockSize))
		throw werr("could not set blocksize");

	if (!set_channels(mFlowSpec.mChannels))
		throw werr("could not set channels");

	if (mFlowSpec.mChannels == 2) {
		if (!set_do_mid_side_stereo(midSide))
			throw werr("could not set do_mid_side_stereo");
		if (!set_loose_mid_side_stereo(midLoose))
			throw werr("could not set loose_mid_side_stereo");
	}

	if (!set_do_exhaustive_model_search(exModel))
		throw werr("could not set do_exhaustive_model_search");

	if (!set_bits_per_sample(mFlowSpec.mSampleSize))
		throw werr("could not set sample size");

	if (!set_sample_rate(static_cast<unsigned>(mFlowSpec.mSampleRate)))
		throw werr("could not set sample rate");

	if (!set_min_residual_partition_order(resMin))
		throw werr("could not set min_residual_partition_order in the output stream");

	if (!set_max_residual_partition_order(resMax))
		throw werr("could not set max_residual_partition_order in the output stream");

	if (!set_max_lpc_order(lpcOrder))
		throw werr("could not set max_lpc_order in the output stream");

	if (!set_streamable_subset(true))
		throw werr("could not set streamable_subset in the output stream");

	if (!set_qlp_coeff_precision(0))
		throw werr("could not set qlp_coeff_precision in the output stream");

	if (!set_do_qlp_coeff_prec_search(true))
		throw werr("could not set do_qlp_coeff_prec_search");

	show_stat();

	if (init() != ::FLAC__STREAM_ENCODER_OK)
		throw werr(get_state().as_cstring());

	mIsOpen = true;
}

void wflacf::show_stat() const
{
	log("wflacf: verify = %s", get_verify() ? "true" : "false");
	log("wflacf: streamable_subset = %s", get_streamable_subset() ? "true" : "false");
	log("wflacf: do_mid_side_stereo = %s", get_do_mid_side_stereo() ? "true" : "false");
	log("wflacf: loose_mid_side_stereo = %s", get_loose_mid_side_stereo() ? "true" : "false");
	log("wflacf: channels = %u", get_channels());
	log("wflacf: bits_per_sample = %u", get_bits_per_sample());
	log("wflacf: sample_rate = %u", get_sample_rate());
	log("wflacf: max_lpc_order = %u", get_max_lpc_order());
	log("wflacf: qlp_coeff_precision = %u", get_qlp_coeff_precision());
	log("wflacf: blocksize = %u", get_blocksize());
	log("wflacf: do_qlp_coeff_prec_search = %s", get_do_qlp_coeff_prec_search() ? "true" : "false");
	log("wflacf: do_escape_coding = %s", get_do_escape_coding() ? "true" : "false");
	log("wflacf: do_exhaustive_model_search = %s", get_do_exhaustive_model_search() ? "true" : "false");
	log("wflacf: min_residual_partition_order = %u", get_min_residual_partition_order());
	log("wflacf: max_residual_partition_order = %u", get_max_residual_partition_order());
	log("wflacf: rice_parameter_search_dist = %u", get_rice_parameter_search_dist());
}
#endif
