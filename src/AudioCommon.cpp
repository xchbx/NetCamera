#include "AudioCommon.h"

using namespace std;

char dbg_str[DBG_STR_SIZE];

void DDEBUG(const char* InfoStr)
{
    std::cout << InfoStr << std::endl;
}

void GenarateFloatFrame(const short *src,float* const* float_frames,int frameNum)
{
	for(int i = 0;i<frameNum;i++)
	{
		float_frames[0][i] = static_cast<float>(src[i]);
	}
	return;
}

void GenarateShortFrame(float* const* float_frames,short *dst,int frameNum)
{
	for(int i = 0;i<frameNum;i++)
	{
		dst[i] = static_cast<short>(float_frames[0][i]+0.5);
	}
}