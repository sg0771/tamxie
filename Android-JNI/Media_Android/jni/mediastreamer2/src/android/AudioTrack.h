
#ifndef ANDROID_AUDIOTRACK_H
#define ANDROID_AUDIOTRACK_H

#include <stdint.h>

#include "audio.h"
#include "loader.h"

namespace fake_android {
typedef void * audio_offload_info_t;
class AudioTrack
{
public:
    enum channel_index {
        MONO   = 0,
        LEFT   = 0,
        RIGHT  = 1
    };

    /* Events used by AudioTrack callback function (audio_track_cblk_t).
     */
    enum event_type {
        EVENT_MORE_DATA = 0,        // Request to write more data to PCM buffer.
        EVENT_UNDERRUN = 1,         // PCM buffer underrun occured.
        EVENT_LOOP_END = 2,         // Sample loop end was reached; playback restarted from loop start if loop count was not 0.
        EVENT_MARKER = 3,           // Playback head is at the specified marker position (See setMarkerPosition()).
        EVENT_NEW_POS = 4,          // Playback head is at a new position (See setPositionUpdatePeriod()).
        EVENT_BUFFER_END = 5        // Playback head is at the end of the buffer.
    };
  #if 1 //merge from opensource-14-02-19,by wanquan
  enum transfer_type {
        TRANSFER_DEFAULT,   // not specified explicitly; determine from the other parameters
        TRANSFER_CALLBACK,  // callback EVENT_MORE_DATA
        TRANSFER_OBTAIN,    
        TRANSFER_SYNC,      // synchronous write()
        TRANSFER_SHARED,    // shared memory
    };
  #endif
    class OldBuffer //merge from opensource-13-12-13,by wanquan
    {
    public:
        enum {
            MUTE    = 0x00000001
        };
        uint32_t    flags;        // 0 or MUTE
        audio_format_t format; 
        int         channelCount; 
        size_t      frameCount; 
        size_t      size;         // input/output in byte units
        union {
            void*       raw;
            short*      i16;    // signed 16-bit
            int8_t*     i8;     // unsigned 8-bit, offset by 0x80
        };
    };
	class Buffer //Android 4.3 //merge from opensource-13-12-13,by wanquan
    {
    public:
        size_t      frameCount;   // number of sample frames corresponding to size;
                                  // on input it is the number of frames desired,
                                  // on output is the number of frames actually filled
        size_t      size;         // input/output in byte units
        union {
            void*       raw;
            short*      i16;    // signed 16-bit
            int8_t*     i8;     // unsigned 8-bit, offset by 0x80
        };
    };
	static void readBuffer(const void *p_info, Buffer *buffer);//merge from opensource-13-12-13,by wanquan
	static void writeBuffer(void *p_info, const Buffer *buffer);//merge from opensource-13-12-13,by wanquan
    typedef void (*callback_t)(int event, void* user, void *info);
     static status_t getMinFrameCount(int* frameCount,
                                      audio_stream_type_t streamType = AUDIO_STREAM_DEFAULT,
                                      uint32_t sampleRate = 0);
                                      
                        AudioTrack();
                        
			   AudioTrack( audio_stream_type_t streamType,
                                    uint32_t sampleRate  = 0,
                                    audio_format_t format = AUDIO_FORMAT_DEFAULT,
                                    int channelMask      = 0,
                                    int frameCount       = 0,
                                    audio_output_flags_t flags = AUDIO_OUTPUT_FLAG_NONE,
                                    callback_t cbf       = NULL,
                                    void* user           = NULL,
                                    int notificationFrames = 0,
                                    int sessionId        = 0,
									transfer_type transferType = TRANSFER_DEFAULT,
                                    const audio_offload_info_t *offloadInfo = NULL,
                                    int uid = -1);


                        // DEPRECATED
                        explicit AudioTrack( int streamType,
                                    uint32_t sampleRate  = 0,
                                    int format = AUDIO_FORMAT_DEFAULT,
                                    int channelMask      = 0,
                                    int frameCount       = 0,
                                    uint32_t flags       = (uint32_t) AUDIO_OUTPUT_FLAG_NONE,
                                    callback_t cbf       = 0,
                                    void* user           = 0,
                                    int notificationFrames = 0,
                                    int sessionId        = 0);
                        ~AudioTrack();
            status_t    initCheck() const;
            uint32_t     latency() const;
            audio_stream_type_t streamType() const;
            audio_format_t format() const;
            int         channelCount() const;
            uint32_t    frameCount() const;
            size_t      frameSize() const;
            void        start();
            void        stop();
            bool        stopped() const;
            void        flush();
            void        pause();
            void        mute(bool);
            bool        muted() const;
            status_t    setVolume(float left, float right);
            void        getVolume(float* left, float* right) const;
            status_t    setAuxEffectSendLevel(float level);
            void        getAuxEffectSendLevel(float* level) const;
            status_t    setSampleRate(int sampleRate);
            uint32_t    getSampleRate() const;
            status_t    setLoop(uint32_t loopStart, uint32_t loopEnd, int loopCount);
            status_t    setMarkerPosition(uint32_t marker);
            status_t    getMarkerPosition(uint32_t *marker) const;
            status_t    setPositionUpdatePeriod(uint32_t updatePeriod);
            status_t    getPositionUpdatePeriod(uint32_t *updatePeriod) const;
            status_t    setPosition(uint32_t position);
            status_t    getPosition(uint32_t *position) const;
            status_t    reload();
            audio_io_handle_t    getOutput();
            int    getSessionId() const;
            status_t    attachAuxEffect(int effectId);
        enum {
            NO_MORE_BUFFERS = 0x80000001,   // same name in AudioFlinger.h, ok to be different value
            STOPPED = 1
        };

            status_t    obtainBuffer(Buffer* audioBuffer, int32_t waitCount);
            void        releaseBuffer(Buffer* audioBuffer);
            ssize_t     write(const void* buffer, size_t size);

private:
	uint8_t *mThis;
	class AudioTrackImpl *mImpl;

};


class AudioTrackImpl{
public:
	static bool init(Library *lib);
	static AudioTrackImpl *get(){return sImpl;}
	Function14<void,
		void*,
		audio_stream_type_t ,
		uint32_t,
		audio_format_t,
		int,
		int,
		audio_output_flags_t,
		AudioTrack::callback_t,
		void*,
		int,
		int,
		int,
		void*,
		int> mCtor;
	Function1<void,void*> mDtor;
	Function1<status_t,const void *> mInitCheck;
	Function1<void,void *> mStop;
	Function1<void, void *> mStart;
	Function1<bool,const void*> mStopped;
	Function1<void,void *> mFlush;
	Function3<status_t,int*,audio_stream_type_t,int> mGetMinFrameCount;
	Function1<uint32_t,void*> mLatency;
	Function2<status_t,void*,uint32_t*> mGetPosition;
private:
	AudioTrackImpl(Library *lib);
	static AudioTrackImpl *sImpl;
};


}; // namespace android

#endif // ANDROID_AUDIOTRACK_H
