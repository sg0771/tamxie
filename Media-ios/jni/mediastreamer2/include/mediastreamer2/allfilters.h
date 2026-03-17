#ifndef allfilters_h
#define allfilters_h

#define V2_PHONE_SUPPORT 1
typedef enum MSFilterId{
	MS_FILTER_NOT_SET_ID,
	MS_FILTER_PLUGIN_ID,	/* no type checking will be performed on plugins */
	MS_FILTER_BASE_ID,
	MS_OPUS_ENC_ID,
	MS_OPUS_DEC_ID,
	MS_AUDIO_MIXER_ID,
	MS_AUDIO_SENDER_ID,
	MS_ANDROID_SOUND_READ_ID,
	MS_ANDROID_SOUND_WRITE_ID,
       MY_FILTER_ID,
       MS_EQUALIZER_ID	
} MSFilterId;

#endif
