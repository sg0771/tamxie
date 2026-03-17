#ifndef AVUTIL_LOG_H
#define AVUTIL_LOG_H

#define AV_LOG_QUIET    -8
#define AV_LOG_PANIC     0
#define AV_LOG_FATAL     8
#define AV_LOG_ERROR    16
#define AV_LOG_WARNING  24
#define AV_LOG_INFO     32
#define AV_LOG_VERBOSE  40
#define AV_LOG_SKIP_REPEATED 1

typedef struct {
    const char* class_name;
    const char* (*item_name)(void* ctx);
    const struct AVOption *option;
    int version;
    int log_level_offset_offset;
    int parent_log_context_offset;
} AVClass;
static const char* av_default_item_name(void* ptr){
    return (*(AVClass**)ptr)->class_name;
};
#define  av_log(...)
#define av_vlog(...)
#define av_log_get_level(...)
#define av_log_set_level(...)
#define av_log_set_callback(...)
#define av_log_default_callback(...)
#define av_log_set_flags(...)

#endif /* AVUTIL_LOG_H */
