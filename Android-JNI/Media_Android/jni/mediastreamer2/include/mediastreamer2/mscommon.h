#ifndef mscommon_h
#define mscommon_h

#define MAX_CHANNEL 100
#define MIXER_MAX_CHANNELS MAX_CHANNEL  //最多100路

#include <Stdint.h>
typedef void(*cb_onSend)(void* param,uint8_t *buf, int size);
#define MS_SENDER_SETSINK	MS_FILTER_METHOD(MS_AUDIO_SENDER_ID,0,void*)
#define MS_SENDER_SETCB  	MS_FILTER_METHOD(MS_AUDIO_SENDER_ID,1,void*)


typedef struct MSAudioNode{
	int pcm;
	int channel;
	int size;
	uint8_t *buf;
} MSAudioNode;

#define MS_MIXER_PUTDATA	MS_FILTER_METHOD(MS_AUDIO_MIXER_ID,0,void*)

#include <ortp/logging.h>
#include <ortp/port.h>
#include <ortp/str_utils.h>
#include <time.h>
#if defined(__APPLE__) 
#include "TargetConditionals.h"
#endif

#define MS_UNUSED(x) ((void)(x))

#define ms_malloc	ortp_malloc
#define ms_malloc0	ortp_malloc0
#define ms_realloc	ortp_realloc
#define ms_new		ortp_new
#define ms_new0		ortp_new0
#define ms_free		ortp_free
#define ms_strdup	ortp_strdup
#define ms_strdup_printf	ortp_strdup_printf

#define ms_mutex_t		ortp_mutex_t
#define ms_mutex_init		ortp_mutex_init
#define ms_mutex_destroy	ortp_mutex_destroy
#define ms_mutex_lock		ortp_mutex_lock
#define ms_mutex_unlock		ortp_mutex_unlock

#define ms_cond_t		ortp_cond_t
#define ms_cond_init		ortp_cond_init
#define ms_cond_wait		ortp_cond_wait
#define ms_cond_signal		ortp_cond_signal
#define ms_cond_broadcast	ortp_cond_broadcast
#define ms_cond_destroy		ortp_cond_destroy

#if defined(_MSC_VER)
#define MS2_PUBLIC	__declspec(dllexport)
#else
#define MS2_PUBLIC
#endif

#if defined(_WIN32_WCE)
time_t ms_time (time_t *t);
#else
#define ms_time time
#endif

#ifdef WIN32
static inline void ms_debug(const char *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  ortp_logv(ORTP_DEBUG, fmt, args);
  va_end (args);
}
#else
#ifdef DEBUG
static inline void ms_debug(const char *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  ortp_logv(ORTP_DEBUG, fmt, args);
  va_end (args);
}
#else
#define ms_debug(...)
#endif	
#endif


#if 0
#define ms_message	ortp_message
#define ms_warning	ortp_warning
#define ms_error	ortp_error
#define ms_fatal	ortp_fatal
#else
#include <android/log.h>
#define TAG "mediastreamer2-"
//#define ms_debug(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define ms_message(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define ms_warning(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__)
#define ms_error(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define ms_fatal(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__)
#endif

#define ms_return_val_if_fail(_expr_,_ret_)\
	if (!(_expr_)) { ms_error("assert "#_expr_ "failed"); return (_ret_);}

#define ms_return_if_fail(_expr_) \
	if (!(_expr_)){ ms_error("assert "#_expr_ "failed"); return ;}

#define ms_thread_t		ortp_thread_t
#define ms_thread_create 	ortp_thread_create
#define ms_thread_join		ortp_thread_join

typedef ortpTimeSpec MSTimeSpec;

#define ms_get_cur_time ortp_get_cur_time

struct _MSList {
	struct _MSList *next;
	struct _MSList *prev;
	void *data;
};

typedef struct _MSList MSList;


#define ms_list_next(elem) ((elem)->next)


typedef int (*MSCompareFunc)(const void *a, const void *b);
typedef void (*MSIterateFunc)(void *a);
typedef void (*MSIterate2Func)(void *a, void *b);

#ifdef __cplusplus
extern "C"{
#endif

MS2_PUBLIC void ms_thread_exit(void* ret_val);
MS2_PUBLIC MSList * ms_list_append(MSList *elem, void * data);
MS2_PUBLIC MSList *ms_list_append_link(MSList *elem, MSList *new_elem);
MS2_PUBLIC MSList * ms_list_prepend(MSList *elem, void * data);
MS2_PUBLIC MSList * ms_list_free(MSList *elem);
MS2_PUBLIC MSList * ms_list_concat(MSList *first, MSList *second);
MS2_PUBLIC MSList * ms_list_remove(MSList *first, void *data);
MS2_PUBLIC int ms_list_size(const MSList *first);
MS2_PUBLIC void ms_list_for_each(const MSList *list, MSIterateFunc iterate_func);
MS2_PUBLIC void ms_list_for_each2(const MSList *list, MSIterate2Func iterate_func, void *user_data);
MS2_PUBLIC MSList *ms_list_remove_link(MSList *list, MSList *elem);
MS2_PUBLIC MSList *ms_list_find(MSList *list, void *data);
MS2_PUBLIC MSList *ms_list_find_custom(MSList *list, MSCompareFunc compare_func, const void *user_data);
MS2_PUBLIC void * ms_list_nth_data(const MSList *list, int index);
MS2_PUBLIC int ms_list_position(const MSList *list, MSList *elem);
MS2_PUBLIC int ms_list_index(const MSList *list, void *data);
MS2_PUBLIC MSList *ms_list_insert_sorted(MSList *list, void *data, MSCompareFunc compare_func);
MS2_PUBLIC MSList *ms_list_insert(MSList *list, MSList *before, void *data);
MS2_PUBLIC MSList *ms_list_copy(const MSList *list);

#undef MIN
#define MIN(a,b)	((a)>(b) ? (b) : (a))
#undef MAX
#define MAX(a,b)	((a)>(b) ? (a) : (b))
#define ms_init()	ms_base_init(), ms_voip_init(), ms_plugins_init()
#define ms_exit()	ms_voip_exit(), ms_base_exit()
MS2_PUBLIC void ms_base_init(void);
MS2_PUBLIC void ms_voip_init(void);
MS2_PUBLIC void ms_plugins_init(void);
MS2_PUBLIC int ms_load_plugins(const char *directory);
MS2_PUBLIC void ms_base_exit(void);
MS2_PUBLIC void ms_voip_exit(void);
struct _MSSndCardDesc;
MS2_PUBLIC void ms_sleep(int seconds);
MS2_PUBLIC void ms_usleep(uint64_t usec);
MS2_PUBLIC int ms_get_payload_max_size();
MS2_PUBLIC void ms_set_payload_max_size(int size);
MS2_PUBLIC int ms_discover_mtu(const char *destination_host);
MS2_PUBLIC void ms_set_mtu(int mtu);
MS2_PUBLIC int ms_get_mtu(void);
MS2_PUBLIC void ms_set_cpu_count(unsigned int c);
 MS2_PUBLIC unsigned int ms_get_cpu_count();

#ifdef __cplusplus
}
#endif

#ifdef MS2_INTERNAL
#  ifdef HAVE_CONFIG_H
#  include "mediastreamer-config.h" /*necessary to know if ENABLE_NLS is there*/
#  endif

#ifdef WIN32
#include <malloc.h> //for alloca
#ifdef _MSC_VER
#define alloca _alloca
#endif
#endif

#  if defined(ENABLE_NLS)
#    include <libintl.h>
#    define _(String) dgettext (GETTEXT_PACKAGE, String)
#  else
#    define _(String) (String)
#  endif // ENABLE_NLS
#define N_(String) (String)
#endif // MS2_INTERNAL

#ifdef ANDROID
#include "mediastreamer2/msjava.h"
#endif
#endif

