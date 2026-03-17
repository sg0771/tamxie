/*
SwrResample API
*/
#ifndef __WX_SWR_API_H_
#define __WX_SWR_API_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void *WXResampleCreate(int out_bFloat, int out_channels, int out_sample_rate,
	int in_bFloat, int  in_channels, int  in_sample_rate);

void WXResampleDestroy(void **ptr);

int WXResampleConvert(void *ptr, uint8_t **out, int out_count,
		const uint8_t **in, int in_count);


#ifdef __cplusplus
}
#endif

#endif /* __WX_SWR_API_H_ */
