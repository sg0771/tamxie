#include "mediastreamer2/mscommon.h"
#include "AudioTrack.h"
#include "AudioSystem.h"

namespace fake_android{
	AudioTrack::AudioTrack( audio_stream_type_t streamType,
                                    uint32_t sampleRate  ,
                                    audio_format_t format,
                                    int channelMask,
                                    int frameCount,
                                    audio_output_flags_t flags,
                                    callback_t cbf,
                                    void* user,
                                    int notificationFrames,
                                    int sessionId, 
                                    transfer_type transferType,
                                    const audio_offload_info_t *offloadInfo,
                                    int uid){
   		mThis=new uint8_t[512];
		mImpl=AudioTrackImpl::get();
		mImpl->mCtor.invoke(mThis,streamType,sampleRate,format,channelMask,frameCount,flags,cbf,user,notificationFrames,sessionId,transferType,(void*)offloadInfo,uid);
	}
	AudioTrack::~AudioTrack(){
		mImpl->mDtor.invoke(mThis);
		delete [] mThis;
	}
	
	void AudioTrack::start(){
		mImpl->mStart.invoke(mThis);
	}
	
	void AudioTrack::stop(){
		mImpl->mStop.invoke(mThis);
	}
	
	status_t AudioTrack::initCheck()const{
		if (mImpl->mInitCheck.isFound()) return mImpl->mInitCheck.invoke(mThis);
		return 0;
	}
	
	bool AudioTrack::stopped()const{
		return mImpl->mStopped.invoke(mThis);
	}
	
	void AudioTrack::flush(){
		return mImpl->mFlush.invoke(mThis);
	}
	
	status_t AudioTrack::getMinFrameCount(int* frameCount,
                                      audio_stream_type_t streamType,
                                      uint32_t sampleRate){
		// Initialize frameCount to a magic value
		*frameCount = 54321;
		if (AudioTrackImpl::get()->mGetMinFrameCount.isFound()){
			status_t ret = AudioTrackImpl::get()->mGetMinFrameCount.invoke(frameCount,streamType,sampleRate);
			if ((ret == 0) && (*frameCount = 54321)) {
				// If no error and the magic value has not been erased then the getMinFrameCount implementation
				// is a dummy one. So perform the calculation manually as it is supposed to be implemented...
				int afSampleRate;
				if (AudioSystem::getOutputSamplingRate(&afSampleRate, streamType) != 0) return -1;
				int afFrameCount;
				if (AudioSystem::getOutputFrameCount(&afFrameCount, streamType) != 0) return -1;
				uint32_t afLatency;
				if (AudioSystem::getOutputLatency(&afLatency, streamType) != 0) return -1;

				// Ensure that buffer depth covers at least audio hardware latency
				uint32_t minBufCount = afLatency / ((1000 * afFrameCount) / afSampleRate);
				if (minBufCount < 2) minBufCount = 2;

				*frameCount = (sampleRate == 0) ? afFrameCount * minBufCount :
					afFrameCount * minBufCount * sampleRate / afSampleRate;
			}
			return ret;
		}else{
			//this method didn't existed in 2.2
			//Use hardcoded values instead (1024 frames at 8khz) 
			*frameCount=(1024*sampleRate)/8000;
			return 0;
		}
	}
	
	uint32_t AudioTrack::latency()const{
		if (mImpl->mLatency.isFound())//merge from opensource-13-12-13,by wanquan
			return mImpl->mLatency.invoke(mThis);
		else return (uint32_t)-1;//merge from opensource-13-12-13,by wanquan
	}
	
	status_t AudioTrack::getPosition(uint32_t *frames) const{
		return mImpl->mGetPosition.invoke(mThis,frames);
	}
	
	void AudioTrack::readBuffer(const void *p_info, Buffer *buffer){//merge from opensource-13-12-13,by wanquan
		if (AudioSystemImpl::get()->mApi18){
			*buffer=*(const Buffer*)p_info;
		}else{
			const OldBuffer *oldbuf=(const OldBuffer*)p_info;
			buffer->frameCount=oldbuf->frameCount;
			buffer->size=oldbuf->size;
			buffer->raw=oldbuf->raw;
		}
	}
	
	void AudioTrack::writeBuffer(void *p_info, const Buffer *buffer){//merge from opensource-13-12-13,by wanquan
		if (AudioSystemImpl::get()->mApi18){
			*(Buffer*)p_info=*buffer;
		}else{
			OldBuffer *oldbuf=(OldBuffer*)p_info;
			oldbuf->frameCount=buffer->frameCount;
			oldbuf->raw=buffer->raw;
			oldbuf->size=buffer->size;
		}
	}
	
	AudioTrackImpl::AudioTrackImpl(Library *lib) :
		// By default, try to load Android 2.3 symbols
		mCtor(lib,"_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_ii"),
		mDtor(lib,"_ZN7android10AudioTrackD1Ev"),
		mInitCheck(lib,"_ZNK7android10AudioTrack9initCheckEv"),
		mStop(lib,"_ZN7android10AudioTrack4stopEv"),
		mStart(lib,"_ZN7android10AudioTrack5startEv"),
		mStopped(lib,"_ZNK7android10AudioTrack7stoppedEv"),
		mFlush(lib,"_ZN7android10AudioTrack5flushEv"),
		mGetMinFrameCount(lib,"_ZN7android10AudioTrack16getMinFrameCountEPiij"),
		mLatency(lib,"_ZNK7android10AudioTrack7latencyEv"),
		#if 1 //merge from opensource-14-02-19,by wanquan
		mGetPosition(lib,"_ZNK7android10AudioTrack11getPositionEPj") //4.4 symbol
		#else
		mGetPosition(lib,"_ZN7android10AudioTrack11getPositionEPj")
		#endif
	{
		if (!mCtor.isFound()) {
			mCtor.load(lib,"_ZN7android10AudioTrackC1EijiiijPFviPvS1_ES1_i");
			if (!mCtor.isFound())//merge from opensource-13-12-13,by wanquan
				mCtor.load(lib,"_ZN7android10AudioTrackC1E19audio_stream_type_tj14audio_format_tji20audio_output_flags_tPFviPvS4_ES4_ii");
		}

		// Then try some Android 4.1 symbols if still not found
		if (!mGetMinFrameCount.isFound()) {
			mGetMinFrameCount.load(lib,"_ZN7android10AudioTrack16getMinFrameCountEPi19audio_stream_type_tj");
		}
		#if 1 //merge from opensource-14-02-19,by wanquan
		if (!mGetPosition.isFound()){
			mGetPosition.load(lib,"_ZN7android10AudioTrack11getPositionEPj"); //until 4.3 included
		}
		#endif
		
	}
	
	bool AudioTrackImpl::init(Library *lib){//merge from opensource-13-12-13,by wanquan
		bool fail=false;
		AudioTrackImpl *impl=new AudioTrackImpl(lib);
		
		if (!impl->mCtor.isFound()) {
			fail=true;
		}
		if (!impl->mDtor.isFound()) {
			
			fail=true;
		}
		if (!impl->mStart.isFound()) {
			fail=true;
		}
		if (!impl->mStop.isFound()) {
			fail=true;
		}
		if (!impl->mInitCheck.isFound()) {
		}
		if (!impl->mFlush.isFound()) {
			fail=true;
		}
		if (!impl->mLatency.isFound()) {
		}
		if (!impl->mGetPosition.isFound()) {
			fail=true;
		}
		if (!fail){
			sImpl=impl;
			return true;
		}else{
			delete impl;
			return false;
		}
	}
	
	AudioTrackImpl * AudioTrackImpl::sImpl=NULL;
	
}//end of namespace
