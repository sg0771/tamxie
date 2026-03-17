#include "WaterMarkRemoveAPI.h"
#include "FFmpegDelogoConvert.h"

WXDELOGO_CAPI void* CreateConvert() {
    FFmpegDelogoConvert* ptr = new FFmpegDelogoConvert();
    return ptr;
}

WXDELOGO_CAPI void DestroyConvert(void* ptr) {
    if (!ptr) return;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    delete ff;
}

WXDELOGO_CAPI void StartConvert(void* ptr) {
    if (!ptr) return;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    ff->StartConvert();
}

WXDELOGO_CAPI void StopConvert(void* ptr) {
    if (!ptr) return;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    ff->StopConvert();
}

WXDELOGO_CAPI int InitSource(void* ptr, WXCTSTR in_file, WXCTSTR out_file, int(*logo_rectangles)[4], int nb_rects) {
    if (!ptr) return 0;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    return ff->InitSource(in_file, out_file, logo_rectangles, nb_rects);
}

WXDELOGO_CAPI void SetVideoConvertTime(void* ptr, int64_t _start_time, int64_t duration) {
    if (!ptr) return;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    return ff->SetVideoConvertTime(_start_time, duration);
}

WXDELOGO_CAPI int GetConvertProgress(void* ptr) {
    if (!ptr) return -1;
    FFmpegDelogoConvert* ff = (FFmpegDelogoConvert*)ptr;
    return ff->GetConvertProgress();
}
