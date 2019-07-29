#pragma once

#ifndef AUDIO_COMMON_H
#define AUDIO_COMMON_H

#include <memory>
#include <deque>
#include <stdint.h>
#include <math.h>
#include <map>
#include <iostream>

#define PRINT_WEBRTC_INFO

#define DBG_STR_SIZE 4096

#ifdef PRINT_WEBRTC_INFO
#define SPRINTF_S sprintf
#else
#define SPRINTF_S
#endif

#define ALG_NO_INIT -1
#define ALG_DO_INIT  0
#define ALG_OK_INIT  1

void DDEBUG(const char* InfoStr);
void GenarateFloatFrame(const short *src,float* const* float_frames,int frameNum);
void GenarateShortFrame(float* const* float_frames,short *dst,int frameNum);

#endif//AUDIO_COMMON_H

