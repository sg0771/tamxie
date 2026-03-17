#include "mediastreamer2/mscommon.h"
#include "mediastreamer2/mscodecutils.h"
#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mssndcard.h"


extern MSFilterDesc ms_audio_sender_desc;
extern MSFilterDesc ms_opus_enc_desc;
extern MSFilterDesc ms_opus_dec_desc;
extern MSFilterDesc ms_audio_mixer_desc;

static MSFilterDesc * ms_voip_filter_descs[]={
&ms_opus_enc_desc,
&ms_opus_dec_desc,
&ms_audio_sender_desc,
&ms_audio_mixer_desc,
NULL
};

extern MSSndCardDesc android_native_snd_opensles_card_desc;
static MSSndCardDesc * ms_snd_card_descs[]={
	&android_native_snd_opensles_card_desc,
	NULL
};


void ms_voip_init(){
	MSSndCardManager *cm;
	int i;
	for (i=0;ms_voip_filter_descs[i]!=NULL;i++){
		ms_filter_register(ms_voip_filter_descs[i]);
	}
	cm=ms_snd_card_manager_get();
	for (i=0;ms_snd_card_descs[i]!=NULL;i++){
		ms_snd_card_manager_register_desc(cm,ms_snd_card_descs[i]);
	}
}

void ms_voip_exit(){
	ms_snd_card_manager_destroy();
}
