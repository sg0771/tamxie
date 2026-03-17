#ifndef _WEBRTC_VAD_H_
#define _WEBRTC_VAD_H_

typedef struct WebRtcVadInst VadInst;

#ifdef __cplusplus
extern "C" {
#endif

size_t WebRtcVad_AssignSize();
int WebRtcVad_Assign(void* memory, VadInst** handle);

int WebRtcVad_Create(VadInst** handle);
int WebRtcVad_Free(VadInst* handle);
int WebRtcVad_Init(VadInst* handle);
int WebRtcVad_set_mode(VadInst* handle, int mode);
short WebRtcVad_Process(VadInst* vad_inst, short fs, short* speech_frame,short frame_length);

#ifdef __cplusplus
}
#endif

#endif  // _WEBRTC_VAD_H_
