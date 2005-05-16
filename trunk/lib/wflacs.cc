// $Id$

#ifdef HAVE_flac
#include "writer.h"

wflacs::wflacs(const reader *r) :
	writer(r)
{
	mIsOpen = false;
}

wflacs::~wflacs()
{
	if (mIsOpen)
		finish();
}

void wflacs::write(samples &smp)
{
	if (smp.size() % mChannels == 0) {
		if (!process_interleaved(&smp[0], smp.size() / mChannels)) {
			log("wflacs: encoding error: %s.", get_state().as_cstring());
		} else {
			smp.clear();
		}
	}
}

bool wflacs::open(const char *fname)
{
	if (!writer::open(fname))
		return false;

	if (!out.open(fname, true))
		return false;

	if (!is_valid()) {
		log("wflacs: not a valid flac stream.");
		return false;
	}

	if (!set_verify(true)) {
		log("wflacs: could not enable verification.");
		return false;
	}

	if (!set_blocksize(4608)) {
		log("wflacs: could not set blocksize.");
		return false;
	}

	if (!set_channels(mChannels = mReader->GetChannels())) {
		log("wflacs: could not set channels.");
		return false;
	}

	if (mReader->GetChannels() == 2) {
		if (!set_do_mid_side_stereo(true)) {
			log("wflacs: could not set do_mid_side_stereo.");
			return false;
		}

		if (!set_loose_mid_side_stereo(true)) {
			log("wflacs: could not set loose_mid_side_stereo.");
			return false;
		}
	}

	if (!set_do_exhaustive_model_search(true)) {
		log("wflacs: could not set do_exhaustive_model_search.");
		return false;
	}

	if (!set_bits_per_sample(mReader->GetSampleSize())) {
		log("wflacs: could not set sample size.");
		return false;
	}

	if (!set_sample_rate(static_cast<unsigned>(mReader->GetSampleRate()))) {
		log("wflacs: could not set sample rate.");
		return false;
	}

	if (!set_min_residual_partition_order(0)) {
		log("wlac: could not set min_residual_partition_order in the output stream.");
		return false;
	}

	if (!set_max_residual_partition_order(6)) {
		log("wflacs: could not set max_residual_partition_order in the output stream.");
		return false;
	}

	if (!set_max_lpc_order(12)) {
		log("wflacs: could not set max_lpc_order in the output stream.");
		return false;
	}

	if (!set_streamable_subset(true)) {
		log("wflacs: could not set streamable_subset in the output stream.");
		return false;
	}

	if (!set_qlp_coeff_precision(0)) {
		log("wflacs: could not set qlp_coeff_precision in the output stream.");
		return false;
	}

	if (!set_do_qlp_coeff_prec_search(true)) {
		log("wflacs: could not set do_qlp_coeff_prec_search.");
		return false;
	}

	show_stat();

	if (init() != ::FLAC__STREAM_ENCODER_OK) {
		log("wflacs: the output stream failed to initialize:       %s", get_state().as_cstring());
		return false;
	}

	return mIsOpen = true;
}

void wflacs::show_stat() const
{
	log("wflacs: verify = %s", get_verify() ? "true" : "false");
	log("wflacs: streamable_subset = %s", get_streamable_subset() ? "true" : "false");
	log("wflacs: do_mid_side_stereo = %s", get_do_mid_side_stereo() ? "true" : "false");
	log("wflacs: loose_mid_side_stereo = %s", get_loose_mid_side_stereo() ? "true" : "false");
	log("wflacs: channels = %u", get_channels());
	log("wflacs: bits_per_sample = %u", get_bits_per_sample());
	log("wflacs: sample_rate = %u", get_sample_rate());
	log("wflacs: max_lpc_order = %u", get_max_lpc_order());
	log("wflacs: qlp_coeff_precision = %u", get_qlp_coeff_precision());
	log("wflacs: blocksize = %u", get_blocksize());
	log("wflacs: do_qlp_coeff_prec_search = %s", get_do_qlp_coeff_prec_search() ? "true" : "false");
	log("wflacs: do_escape_coding = %s", get_do_escape_coding() ? "true" : "false");
	log("wflacs: do_exhaustive_model_search = %s", get_do_exhaustive_model_search() ? "true" : "false");
	log("wflacs: min_residual_partition_order = %u", get_min_residual_partition_order());
	log("wflacs: max_residual_partition_order = %u", get_max_residual_partition_order());
	log("wflacs: rice_parameter_search_dist = %u", get_rice_parameter_search_dist());
}

::FLAC__StreamEncoderWriteStatus wflacs::write_callback(const FLAC__byte buffer[], unsigned bytes, unsigned samples, unsigned current_frame)
{
	log(" : bytes=%u samples=%u frame=%u", bytes, samples, current_frame);
	out.write(buffer, bytes);
	return ::FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

void wflacs::metadata_callback(const ::FLAC__StreamMetadata *)
{
}
#endif
