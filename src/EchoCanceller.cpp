#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <array>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>

#include "AudioCommon.h"
#include "echo_cancellation.h"
#include "EchoCanceller.h"
#include "elog.h"

using namespace webrtc;

#define LOG_TAG    "AEC"
int EchoCanceller::m_init_done = ALG_NO_INIT;

#define MIN_AEC_DELAY_TIME  (-5)
#define MAX_AEC_DELAY_TIME  (5)

static unsigned long g_session_start_time = 0;

extern void GenarateFloatFrame(const short *src,float* const* float_frames,int frameNum);
extern void GenarateShortFrame(float* const* float_frames,short *dst,int frameNum);

static unsigned long GetSysTimeInMsec()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return ((unsigned long)(tv.tv_sec*1000 + tv.tv_usec/1000) - g_session_start_time);
}

EchoCanceller::EchoCanceller()
{
    handle_aec = NULL;
    m_delay_ms = 0;

    //@@@ DEBUG

    #if 0
    fmic = fopen("./debug/alg_mic.pcm", "wb+");
    if (NULL == fmic)
	{
		printf("[EchoCanceller::EchoCanceller] fopen debug/aec_mic.pcm failed\n");
	}
	fref = fopen("./debug/aec_ref.pcm", "wb+");
	if (NULL == fref)
	{
		printf("[EchoCanceller::EchoCanceller] fopen debug/aec_ref.pcm failed\n");
	}
    faec = fopen("./debug/aec_aec.pcm", "wb+");
	if (NULL == faec)
	{
        perror("fopen");
		printf("[EchoCanceller::EchoCanceller] fopen debug/aec_aec.pcm failed\n");
	}
    #endif
}

EchoCanceller::~EchoCanceller()
{
     //@@@ DEBUG
    #if 0
    if (faec) fclose(faec);
    if (fmic) fclose(fmic);
    if (fref) fclose(fref);
    #endif

}

bool EchoCanceller::IsInit()
{
	return (ALG_OK_INIT == m_init_done);
}

void EchoCanceller::Init()
{
    log_d("[EchoCanceller::Init] init aec BEGIN");
    if (true == __sync_bool_compare_and_swap(&m_init_done, ALG_NO_INIT, ALG_DO_INIT))
	{
        g_session_start_time = GetSysTimeInMsec();
        handle_aec = WebRtcAec_Create();
        if (nullptr == handle_aec)
        {
            log_e("[EchoCanceller::Init] create aec failed");
            m_init_done = ALG_NO_INIT;
        }else{
            int status = WebRtcAec_Init(handle_aec,MAX_SAMPLERATE, MAX_SAMPLERATE);
            if(status)
            {
                log_e("[EchoCanceller::Init] init aec failed status = %d",status);
            }    
        }
        AecConfig config;
        //config.nlpMode = kAecNlpConservative;
        config.skewMode  = kAecTrue;
        WebRtcAec_set_config(handle_aec,config);
        m_init_done = ALG_OK_INIT;
    }
    log_d("[EchoCanceller::Init] init aec END SUCC");
    return;
}

int  EchoCanceller::SndRefFrame(char* pdata)
{
    int   status = 0;
    if(nullptr == pdata) 
    {
        return -1;
    }
    short buffer[160];

    std::array<float,MAX_SAMPLES_10MS> m_dframe_0;
    memset(&m_dframe_0,0x0,MAX_SAMPLES_10MS*sizeof(float));   
    std::array<float *const,1> float_frame_ptrs = {
        &m_dframe_0[0],
    };

    float* const* ptr_to_float_frame = &float_frame_ptrs[0];
    memcpy(buffer,pdata,MAX_SAMPLES_10MS*sizeof(short));
    GenarateFloatFrame(buffer,ptr_to_float_frame,MAX_SAMPLES_10MS);
    status = WebRtcAec_BufferFarend(this->handle_aec, ptr_to_float_frame[0], MAX_SAMPLES_10MS);
    if (status) 
    {
        log_w("[EchoCanceller::DoAEC] WebRtcAec_BufferFarend failed, return %d", status);
    }
    return 0;
}

int EchoCanceller::DoAEC(char* pdata, size_t tSize)
{
    //printf("EchoCanceller::DoAEC|this=%p,this->handle_aec = %p\r\n",this,this->handle_aec);
    int   status = 0;
    int16_t smic[MAX_SAMPLES_10MS];
    int16_t saec[MAX_SAMPLES_10MS];
    if(nullptr == pdata) 
    {
        return -1;
    }

    //@@@do aec process
    char s8mic[MAX_SAMPLES_10MS*sizeof(int16_t)] = {0};
    memcpy(s8mic,pdata,MAX_SAMPLES_10MS*sizeof(int16_t));
    for(int i = 0; i<MAX_SAMPLES_10MS; ++i)
    {
        smic[i] =( (s8mic[2*i+1]<<8) | (s8mic[2*i]&0xFF) );
    }

    std::array<float,MAX_SAMPLES_10MS> m_dframe_0;
    memset(&m_dframe_0,0x0,MAX_SAMPLES_10MS*sizeof(float));

    std::array<float *const,1> float_frame_ptrs = {
        &m_dframe_0[0],
    };

    float* const* ptr_to_float_frame = &float_frame_ptrs[0];
    GenarateFloatFrame(smic,ptr_to_float_frame,MAX_SAMPLES_10MS);
    status = WebRtcAec_Process(handle_aec, ptr_to_float_frame, 1,ptr_to_float_frame, MAX_SAMPLES_10MS, m_delay_ms, 0);
    if (status) 
    {
        log_w("[EchoCanceller::DoAEC] WebRtcAec_Process failed, return %d", status);
    }

    memset(saec,0x0,MAX_SAMPLES_10MS*sizeof(int16_t));
	GenarateShortFrame(ptr_to_float_frame,saec,MAX_SAMPLES_10MS);
    memcpy(pdata,saec,sizeof(int16_t)*MAX_SAMPLES_10MS);
 
    return 0;
}

void EchoCanceller::DeInit()
{
    if(NULL != handle_aec)
    {
        WebRtcAec_Free(handle_aec);
        handle_aec = NULL;
    }
}

int EchoCanceller::setDelayTime(int16_t s16DelayMs)
{
    m_delay_ms = s16DelayMs;
    return 0;
}