#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include "EchoCanceller.h"
#include <array>

using namespace webrtc;
using namespace std;

static void GenarateFloatFrame(const short *src,float* const* float_frames,int frameNum);
static void GenarateShortFrame(float* const* float_frames,short *dst,int frameNum);


static void GenarateFloatFrame(const short *src,float* const* float_frames,int frameNum)
{
	for(int i = 0;i<frameNum;i++)
	{
		float_frames[0][i] = static_cast<float>(src[i]);
	}
	return;
}

static void GenarateShortFrame(float* const* float_frames,short *dst,int frameNum)
{
	for(int i = 1;i<frameNum;i++)
	{
		dst[i] = static_cast<short>(float_frames[0][i]+0.5);
	}
}

EchoCanceller::EchoCanceller()
{
    handle_aec = NULL;

    //@@@ DEBUG
    fmic = fopen("./debug/aec_mic.pcm", "wb+");
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
}

EchoCanceller::~EchoCanceller()
{
     //@@@ DEBUG
    if (faec) fclose(faec);
    if (fmic) fclose(fmic);
    if (fref) fclose(fref);

}

void EchoCanceller::Init()
{
    handle_aec = WebRtcAec_Create();
    if (NULL == handle_aec)
    {
        printf("[EchoCanceller::Init] create aec failed\n");
    }else{
        int status = WebRtcAec_Init(handle_aec,MAX_SAMPLERATE, MAX_SAMPLERATE);
        if(status)
        {
            printf("[EchoCanceller::Init] init aec failed status = %d\n",status);
        }    
    }
    return;
}

void EchoCanceller::DoAEC(char* mic_buf, char* ref_buf, char* out_buf)
{
    int   status = 0;
    short smic[MAX_SAMPLES_10MS];
	short sref[MAX_SAMPLES_10MS];
    short saec[MAX_SAMPLES_10MS];

	char s8mic[MAX_SAMPLES_10MS*2];
	char s8ref[MAX_SAMPLES_10MS*2];
    char s8aec[MAX_SAMPLES_10MS*2];

    //@@@do aec process
    memcpy(s8mic,mic_buf,MAX_SAMPLES_10MS*sizeof(short));
    memcpy(s8ref,ref_buf,MAX_SAMPLES_10MS*sizeof(short));
    for(int i = 0;i<MAX_SAMPLES_10MS;i++)
    {
        smic[i] = ( (s8mic[i*2+1]<<8) | (s8mic[2*i]&0XFF) );
        sref[i] = ( (s8ref[i*2+1]<<8) | (s8ref[2*i]&0XFF) );
    }

    if (fmic) fwrite(smic, sizeof(short), MAX_SAMPLES_10MS, fmic);
    if (fref) fwrite(sref, sizeof(short), MAX_SAMPLES_10MS, fref);

    std::array<float,MAX_SAMPLES_10MS> m_dframe_0;
    std::array<float,MAX_SAMPLES_10MS> m_dframe_1;
    memset(&m_dframe_0,0x0,MAX_SAMPLES_10MS*sizeof(float));
    memset(&m_dframe_1,0x0,MAX_SAMPLES_10MS*sizeof(float));
    std::array<float *const,2> float_frame_ptrs = {
        &m_dframe_0[0],
        &m_dframe_1[0],
    };

    float* const* ptr_to_float_frame = &float_frame_ptrs[0];
    GenarateFloatFrame(sref,ptr_to_float_frame,MAX_SAMPLES_10MS);
    status = WebRtcAec_BufferFarend(handle_aec, ptr_to_float_frame[0], MAX_SAMPLES_10MS);
    if (status) 
    {
        printf("[EchoCanceller::DoAEC] WebRtcAec_BufferFarend failed, return %d\n", status);
    }

    GenarateFloatFrame(smic,ptr_to_float_frame,MAX_SAMPLES_10MS);
    status = WebRtcAec_Process(handle_aec, ptr_to_float_frame, 1,ptr_to_float_frame, MAX_SAMPLES_10MS, 0, 0);
    if (status) 
    {
        printf("[EchoCanceller::DoAEC] WebRtcAec_Process failed, return %d\n", status);
    }

    GenarateShortFrame(ptr_to_float_frame,saec,MAX_SAMPLES_10MS);
    fwrite(saec, sizeof(short),MAX_SAMPLES_10MS, faec);
    memcpy(out_buf,saec,sizeof(short)*MAX_SAMPLES_10MS);

    return;
}

void EchoCanceller::Uninit()
{
    if(NULL != handle_aec)
    {
        WebRtcAec_Free(handle_aec);
        handle_aec = NULL;
    }
}
