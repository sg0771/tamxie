#ifndef _WEBRTC_NS_H_
#define _WEBRTC_NS_H_

typedef struct NsHandleT NsHandle;
typedef struct NsxHandleT NsxHandle;

#ifdef __cplusplus
extern "C" {
#endif

int WebRtcNs_Create(NsHandle** NS_inst);
int WebRtcNs_Free(NsHandle* NS_inst);
int WebRtcNs_Init(NsHandle* NS_inst, unsigned int fs);
int WebRtcNs_set_policy(NsHandle* NS_inst, int mode);
int WebRtcNs_Process(NsHandle* NS_inst,
                     short* spframe,
                     short* spframe_H,
                     short* outframe,
                     short* outframe_H);

//NsX
int WebRtcNsx_Create(NsxHandle** nsxInst);
int WebRtcNsx_Free(NsxHandle* nsxInst);
int WebRtcNsx_Init(NsxHandle* nsxInst, unsigned int fs);
int WebRtcNsx_set_policy(NsxHandle* nsxInst, int mode);
int WebRtcNsx_Process(NsxHandle* nsxInst,
					  short* speechFrame,
					  short* speechFrameHB,
					  short* outFrame,
					  short* outFrameHB);

#ifdef __cplusplus
}
#endif

#endif  // _WEBRTC_NS_H_
