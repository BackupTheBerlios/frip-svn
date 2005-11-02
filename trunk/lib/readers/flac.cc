// $Id$

#ifdef HAVE_flac

#include <FLAC++/all.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "all.h"

rflac::rflac(frip_callback cb) :
	reader(cb)
{
	mSamples = NULL;
	mFileSize = 0;
	mProcessedSize = 0;
	mEOF = false;
}

rflac::~rflac()
{
}

::FLAC__StreamDecoderReadStatus rflac::read_callback(FLAC__byte buffer[], unsigned *bytes)
{
	bool rc = in.read_raw(buffer, *bytes);

	if (mCallBack != NULL) {
		mProcessedSize += *bytes;
		mCallBack((unsigned)((double)mProcessedSize / ((double)mFileSize / 100)));
	}

	return (rc && *bytes != 0) ? FLAC__STREAM_DECODER_READ_STATUS_CONTINUE : FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
}

::FLAC__StreamDecoderWriteStatus rflac::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
{
	size_t osz;
	samples::iterator it;

	if (frame->header.channels != mFlowSpec.mChannels) {
		log("rflac: bad frame: channel number mismatch: expected %u, received %u.", mFlowSpec.mChannels, frame->header.channels);
		return ::FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	osz = mSamples->size();
	mSamples->resize(osz + frame->header.blocksize * mFlowSpec.mChannels);
	it = mSamples->begin() + osz;

	for (unsigned fidx = 0, lim = frame->header.blocksize; fidx < lim; ++fidx) {
		for (unsigned int ch = 0; ch < mFlowSpec.mChannels; ++ch) {
			*it++ = buffer[ch][fidx];
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void rflac::metadata_callback(const ::FLAC__StreamMetadata *m)
{
	switch (m->type) {
	case FLAC__METADATA_TYPE_STREAMINFO:
		mFlowSpec.mSampleRate = m->data.stream_info.sample_rate;
		mFlowSpec.mChannels = m->data.stream_info.channels;
		mFlowSpec.mSampleSize = m->data.stream_info.bits_per_sample;
		log("rflac: stream info read.");
		break;
	case FLAC__METADATA_TYPE_VORBIS_COMMENT:
		log("rflac: vorbis comment (%u items).", m->data.vorbis_comment.num_comments);

		for (unsigned int idx = 0; idx < m->data.vorbis_comment.num_comments; ++idx) {
			FLAC__StreamMetadata_VorbisComment_Entry &e = m->data.vorbis_comment.comments[idx];
			const char *tag = reinterpret_cast<const char *>(e.entry);
			const char *sep = strchr(tag, '=');

			if (sep != NULL)
				AddTag(string(tag, sep - tag), string(sep + 1, e.length - (sep - tag) - 1));
		}

		break;
	default:
		log("rflac: unsupported metadata block ignored.");
	}
}

void rflac::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
	log("rflac: error callback.");
}

bool rflac::read(samples &smp, size_t preferred_wide_sample_count)
{
	size_t osz = smp.size();
	size_t need = osz + preferred_wide_sample_count * mFlowSpec.mChannels;

	if (mEOF)
		return false;

	if (mFlowSpec.mSampleRate != 44100) {
		log("rflac: sample rate mismatch (%u vs. 44100), aborting.", mFlowSpec.mSampleRate);
		return false;
	}

	mSamples = &smp;

	while (smp.size() < need) {
		if (!process_single()) {
			log("rflac: flac decoder failed.");
			return false;
		}

		if (get_state() == FLAC__STREAM_DECODER_END_OF_STREAM) {
			log("rflac: finished the stream.");
			mEOF = true;
			break;
		}
	}

	return true;
}

bool rflac::open(const char *fname)
{
	if (!reader::open(fname)) {
		log("rflac: could not open %s for reading.\n", fname);
		return false;
	} else {
		struct stat st;

		if (stat(fname, &st) != 0) {
			log("rflac: could not retreive file statistics.");
		} else {
			mFileSize = st.st_size;
			mProcessedSize = 0;

			log("rflac: file size = %u", mFileSize);
		}
	}

	if (!is_valid()) {
		log("rflac: not a valid flac file.");
		return false;
	}

	if (!set_metadata_respond_all()) {
		log("rflac: failed to enable metadata processing.");
		return false;
	}

	if (init() != ::FLAC__STREAM_DECODER_SEARCH_FOR_METADATA) {
		log("rflac: failed to initialise the flac stream decoder: %s.", get_state().as_cstring());
		return false;
	}

	if (!process_until_end_of_metadata()) {
		log("rflac: failed to process metadata: %s.", get_state().as_cstring());
		return false;
	}

	return true;
}

#endif
