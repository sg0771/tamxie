#ifndef _FAAC_H_
#define _FAAC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define FAACAPI

#pragma pack(push, 1)

typedef struct {
  void *ptr;
  char *name;
}
psymodellist_t;

#include "faaccfg.h"


typedef void *faacEncHandle;

#ifndef HAVE_INT32_T
typedef signed int int32_t;
#endif

/*
	Allows an application to get FAAC version info. This is intended
	purely for informative purposes.

	Returns FAAC_CFG_VERSION.
*/
int FAACAPI faacEncGetVersion(char **faac_id_string,
			      char **faac_copyright_string);


faacEncConfigurationPtr FAACAPI
  faacEncGetCurrentConfiguration(faacEncHandle hEncoder);


int FAACAPI faacEncSetConfiguration(faacEncHandle hEncoder,
				    faacEncConfigurationPtr config);


faacEncHandle FAACAPI faacEncOpen(unsigned long sampleRate,
				  unsigned int numChannels,
				  unsigned long *inputSamples,
				  unsigned long *maxOutputBytes);


int FAACAPI faacEncGetDecoderSpecificInfo(faacEncHandle hEncoder, unsigned char **ppBuffer,
					  unsigned long *pSizeOfDecoderSpecificInfo);


int FAACAPI faacEncEncode(faacEncHandle hEncoder, int32_t * inputBuffer, unsigned int samplesInput,
			 unsigned char *outputBuffer,
			 unsigned int bufferSize);


int FAACAPI faacEncClose(faacEncHandle hEncoder);



#pragma pack(pop)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FAAC_H_ */
