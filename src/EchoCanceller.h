#pragma once
#ifndef ECHO_CANCELLER_H
#define ECHO_CANCELLER_H

#include "echo_cancellation.h"

#define MAX_SAMPLERATE   (16000)
#define MAX_SAMPLES_10MS ((MAX_SAMPLERATE*10)/1000)

class EchoCanceller
{
public:
    EchoCanceller(void);
    virtual ~EchoCanceller(void);

    void Init();
    void DoAEC(char* mic_buf, char* ref_buf, char* out_buf);
    void Uninit();

private:
    void *handle_aec = NULL;
    //@@@DEBUG
    FILE *faec = NULL;
	FILE *fmic = NULL;
	FILE *fref = NULL;
};

#endif//ECHO_CANCELLER_H
