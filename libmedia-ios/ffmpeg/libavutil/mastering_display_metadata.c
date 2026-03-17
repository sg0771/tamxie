#include <stdint.h>
#include <string.h>

#include "mastering_display_metadata.h"
#include "mem.h"

AVMasteringDisplayMetadata *av_mastering_display_metadata_alloc(void)
{
    return av_mallocz(sizeof(AVMasteringDisplayMetadata));
}

AVMasteringDisplayMetadata *av_mastering_display_metadata_create_side_data(AVFrame *frame)
{
    AVFrameSideData *side_data = av_frame_new_side_data(frame,
                                                        AV_FRAME_DATA_MASTERING_DISPLAY_METADATA,
                                                        sizeof(AVMasteringDisplayMetadata));
    if (!side_data)
        return NULL;

    memset(side_data->data, 0, sizeof(AVMasteringDisplayMetadata));

    return (AVMasteringDisplayMetadata *)side_data->data;
}
