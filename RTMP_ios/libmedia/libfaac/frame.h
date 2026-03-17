#ifndef FRAME_H
#define FRAME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "coder.h"
#include "channels.h"
#include "psych.h"
#include "aacquant.h"
#include "fft.h"

#define FAACAPI // __declspec(dllexport)
	
#pragma pack(push, 1)
	
	typedef struct {
		psymodel_t *model;
		char *name;
	} psymodellist_t;

#include "faaccfg.h"

typedef struct {
    /* number of channels in AAC file */
    unsigned int numChannels;

    /* samplerate of AAC file */
    unsigned long sampleRate;
    unsigned int sampleRateIdx;

    unsigned int usedBytes;

    /* frame number */
    unsigned int frameNum;
    unsigned int flushFrame;

    /* Scalefactorband data */
    SR_INFO *srInfo;

    /* sample buffers of current next and next next frame*/
    double *sampleBuff[MAX_CHANNELS];
    double *nextSampleBuff[MAX_CHANNELS];
    double *next2SampleBuff[MAX_CHANNELS];
    double *next3SampleBuff[MAX_CHANNELS];
    double *ltpTimeBuff[MAX_CHANNELS];

    /* Filterbank buffers */
    double *sin_window_long;
    double *sin_window_short;
    double *kbd_window_long;
    double *kbd_window_short;
    double *freqBuff[MAX_CHANNELS];
    double *overlapBuff[MAX_CHANNELS];

    double *msSpectrum[MAX_CHANNELS];

    /* Channel and Coder data for all channels */
    CoderInfo coderInfo[MAX_CHANNELS];
    ChannelInfo channelInfo[MAX_CHANNELS];

    /* Psychoacoustics data */
    PsyInfo psyInfo[MAX_CHANNELS];
    GlobalPsyInfo gpsyInfo;

    /* Configuration data */
    faacEncConfiguration config;

    psymodel_t *psymodel;

    /* quantizer specific config */
    AACQuantCfg aacquantCfg;

	/* FFT Tables */
	FFT_Tables	fft_tables;

    /* output bits difference in average bitrate mode */
    int bitDiff;
} faacEncStruct, *faacEncHandle;

int FAACAPI faacEncGetVersion(char **faac_id_string,
			      char **faac_copyright_string);

int FAACAPI faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder,
                                          unsigned char** ppBuffer,
                                          unsigned long* pSizeOfDecoderSpecificInfo);

faacEncConfigurationPtr FAACAPI faacEncGetCurrentConfiguration(faacEncHandle hEncoder);
int FAACAPI faacEncSetConfiguration (faacEncHandle hEncoder, faacEncConfigurationPtr config);

faacEncHandle FAACAPI faacEncOpen(unsigned long sampleRate,
                                  unsigned int numChannels,
                                  unsigned long *inputSamples,
                                  unsigned long *maxOutputBytes);

int FAACAPI faacEncEncode(faacEncHandle hEncoder,
                          int *inputBuffer,
                          unsigned int samplesInput,
                          unsigned char *outputBuffer,
                          unsigned int bufferSize
                          );

int FAACAPI faacEncClose(faacEncHandle hEncoder);


#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FRAME_H */
