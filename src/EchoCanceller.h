#pragma once
#ifndef ECHO_CANCELLER_H
#define ECHO_CANCELLER_H

#define MAX_SAMPLERATE   (16000)
#define MAX_SAMPLES_10MS ((MAX_SAMPLERATE*10)/1000)
#define MAX_RING_BUFFER_LEN   (10)

class EchoCanceller
{
public:
    EchoCanceller(void);
    virtual ~EchoCanceller(void);

    void Init();
    int DoAEC(char* pdata,size_t tSize);
    void DeInit();
	bool IsInit();
    int SndRefFrame(char* pdata);
    int setDelayTime(int16_t s16DalayMs);


private:
    void *handle_aec = NULL;
	static int m_init_done;
    int16_t m_delay_ms;
    //@@@DEBUG
    #if 0
    FILE *faec = NULL;
	FILE *fmic = NULL;
	FILE *fref = NULL;
    #endif
    FILE *fsave = nullptr;
};

#endif//ECHO_CANCELLER_H
