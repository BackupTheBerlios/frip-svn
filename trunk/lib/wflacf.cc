// $Id$

#ifdef HAVE_flac
#include "writer.h"

wflacf::wflacf(const reader *r) :
	writer(r)
{
	mIsOpen = false;
}

wflacf::~wflacf()
{
	if (mIsOpen)
		finish();
}

bool wflacf::write(samples &smp)
{
	if (smp.size() % mChannels == 0) {
		if (!process_interleaved(&smp[0], smp.size() / mChannels)) {
			log("wflacf: encoding error: %s.", get_state().as_cstring());
			return false;
		} else {
			smp.clear();
		}
	}

	return true;
}

bool wflacf::open(const char *fname)
{
	if (!writer::open(fname))
		return false;

	if (!is_valid()) {
		log("wflacf: not a valid flac stream.");
		return false;
	}

	if (!set_filename(fname)) {
		log("wflacf: could not set file name.");
		return false;
	}

	if (!set_verify(true)) {
		log("wflacf: could not enable verification.");
		return false;
	}

	if (!set_blocksize(4608)) {
		log("wflacf: could not set blocksize.");
		return false;
	}

	if (!set_channels(mChannels = mReader->GetChannels())) {
		log("wflacf: could not set channels.");
		return false;
	}

	if (mReader->GetChannels() == 2) {
		if (!set_do_mid_side_stereo(true)) {
			log("wflacf: could not set do_mid_side_stereo.");
			return false;
		}

		if (!set_loose_mid_side_stereo(true)) {
			log("wflacf: could not set loose_mid_side_stereo.");
			return false;
		}
	}

	if (!set_do_exhaustive_model_search(true)) {
		log("wflacf: could not set do_exhaustive_model_search.");
		return false;
	}

	if (!set_bits_per_sample(mReader->GetSampleSize())) {
		log("wflacf: could not set sample size.");
		return false;
	}

	if (!set_sample_rate(static_cast<unsigned>(mReader->GetSampleRate()))) {
		log("wflacf: could not set sample rate.");
		return false;
	}

	if (!set_min_residual_partition_order(0)) {
		log("wlac: could not set min_residual_partition_order in the output stream.");
		return false;
	}

	if (!set_max_residual_partition_order(6)) {
		log("wflacf: could not set max_residual_partition_order in the output stream.");
		return false;
	}

	if (!set_max_lpc_order(12)) {
		log("wflacf: could not set max_lpc_order in the output stream.");
		return false;
	}

	if (!set_streamable_subset(true)) {
		log("wflacf: could not set streamable_subset in the output stream.");
		return false;
	}

	if (!set_qlp_coeff_precision(0)) {
		log("wflacf: could not set qlp_coeff_precision in the output stream.");
		return false;
	}

	if (!set_do_qlp_coeff_prec_search(true)) {
		log("wflacf: could not set do_qlp_coeff_prec_search.");
		return false;
	}

	show_stat();

	if (init() != ::FLAC__STREAM_ENCODER_OK) {
		log("wflacf: the output stream failed to initialize:       %s", get_state().as_cstring());
		return false;
	}

	return mIsOpen = true;
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
