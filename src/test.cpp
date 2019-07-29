#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/prctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

#include "PackageQueue.h"
#include "EchoCanceller.h"

#include "elog.h"
#define LOG_TAG    "TESTTASK"

#define MAX_SAMPLERATE   (16000)
#define MAX_SAMPLES_10MS ((MAX_SAMPLERATE*10)/1000)
#define MAX_SUBBANS_NUM  (1)
#define MIC_SAMPLERATE			16000
#define REF_SAMPLERATE			16000
#define AEC_SAMPLERATE			16000
#define MIC_IN_FRAMESAMPLES 	160
#define REF_IN_FRAMESAMPLES 	160
#define MIC_OUT_FRAMESAMPLES 	160		  	// 10ms
#define REF_OUT_FRAMESAMPLES 	160		  	// 10ms
#define AEC_FRAMESAMPLES 		160		   	// 10ms
#define MIC_RINGBUFF_LEN (MIC_SAMPLERATE*100/1000) // 100ms
#define REF_RINGBUFF_LEN (REF_SAMPLERATE*140/1000) // 140ms


class AAudioBuffer;
using namespace std;

typedef struct 
{
    char sfilepath[80];
	int  worksamplerate;
    int  framesamples;
    volatile bool bcontinue[256];
	volatile bool bfinished[256];
	volatile bool bcreated[256];
	volatile bool *pbcontinue;
	volatile bool *pbfinished;
	volatile bool *pbcreated;
}MicThreadParams;

typedef struct
{
    char sfilepath[80];
	int  worksamplerate;
    int  framesamples;
    volatile bool bcontinue[256];
	volatile bool bfinished[256];
	volatile bool bcreated[256];
	volatile bool *pbcontinue;
	volatile bool *pbfinished;
	volatile bool *pbcreated;
}RefThreadParams;

typedef struct
{

    char sfilepath[80];
    int  nmicdelaysamples;
    int  nrefdelaysamples;
    int  micframesamples;
    int  refframesamples;
    int  aecframesamples;
    int  worksamplerate;
    int  framesamples;
    volatile bool bcontinue[256];
	volatile bool bfinished[256];
	volatile bool bcreated[256];
	volatile bool *pbcontinue;
	volatile bool *pbfinished;
	volatile bool *pbcreated;	
}AecThreadParams;

static void S16ToFloatS16(const short *src, size_t size, float *dest);
static void *micpthreadfunc(void *args);
static void *refpthreadfunc(void *args);
static void *aecpthreadfunc(void *args);

static int difftimeval(struct timeval *curr, struct timeval *last)
{
    return (curr->tv_sec - last->tv_sec)*1000000 + (curr->tv_usec - last->tv_usec);
}

static struct timeval firstTime = {0};
static long long getSysTimeInMsec()
{
	struct timeval curr;
	gettimeofday(&curr, NULL);
	return (curr.tv_sec - firstTime.tv_sec)*1000 + (curr.tv_usec - firstTime.tv_usec)/1000;
}

static int ElogInit(void) {
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();

    return 0;
}

static MicThreadParams micparams;
static RefThreadParams refparams;
static AecThreadParams aecparams;

static EchoCanceller g_aec;
/*
log_a("Hello EasyLogger!");
log_e("Hello EasyLogger!");
log_w("Hello EasyLogger!");
log_i("Hello EasyLogger!");
log_d("Hello EasyLogger!");
log_v("Hello EasyLogger!"); 
*/ 

int main(int argc, char **argv)
{
	pthread_t micthread;
	pthread_t refthread;

	int status = 0;
	ElogInit();
	g_aec.Init();
	gettimeofday(&firstTime, NULL);


	strcpy(refparams.sfilepath, "data/ref16k.pcm");
	refparams.worksamplerate = REF_SAMPLERATE;
	refparams.framesamples = REF_IN_FRAMESAMPLES;
	refparams.pbcontinue = (volatile bool *)(((long)&(refparams.bcontinue[0]) + 127) & (~127));
	refparams.pbfinished = (volatile bool *)(((long)&(refparams.bfinished[0]) + 127) & (~127));
	refparams.pbcreated  = (volatile bool *)(((long)&(refparams.bcreated[0]) + 127) & (~127));
	*refparams.pbcontinue = true;
	*refparams.pbfinished = false;
	*refparams.pbcreated = false;
	log_v("[main] refparams addr:%p", &refparams);
	status = pthread_create(&refthread, NULL, refpthreadfunc, &refparams);
	if (status)
	{
		log_a("[main] pthread_create for ref failed, return %d", status);
		goto exitproc;
	}
	*refparams.pbcreated = true;


	strcpy(micparams.sfilepath, "data/mic16k.pcm");
	micparams.worksamplerate = MIC_SAMPLERATE;
	micparams.framesamples = MIC_IN_FRAMESAMPLES;
	micparams.pbcontinue = (volatile bool *)(((long)&(micparams.bcontinue[0]) + 127) & (~127));
	micparams.pbfinished = (volatile bool *)(((long)&(micparams.bfinished[0]) + 127) & (~127));
	micparams.pbcreated  = (volatile bool *)(((long)&(micparams.bcreated[0]) + 127) & (~127));
	*micparams.pbcontinue = true;
	*micparams.pbfinished = false;
	*micparams.pbcreated = false;
	log_v("[main] micparams addr:%p", &micparams);
	status = pthread_create(&micthread, NULL, micpthreadfunc, &micparams);
	if (status)
	{
		log_a("[main] pthread_create for mic failed, return %d", status);
		goto exitproc;
	}
	*micparams.pbcreated = true;


exitproc:
	while(*refparams.pbcreated && !*refparams.pbfinished)
	{
		usleep(1000);
	}
	while(*micparams.pbcreated && !*micparams.pbfinished)
	{
		usleep(1000);
	}

	pthread_join(refthread,NULL);
	pthread_join(micthread,NULL);

	getchar();

    return 0;
}

static void *micpthreadfunc(void *args)
{
    MicThreadParams *pmicparams = (MicThreadParams *)args;
    char s8mic[MAX_SAMPLES_10MS*2];
    FILE *fmic = NULL;
	FILE *faec = NULL;
	FILE *fdbg = NULL;
	struct timeval time0 = {0};
    struct timeval time1 = {0};
	struct timeval time2 = {0};
	struct timeval time3 = {0};
    int dispatchcycle = 0;
	int intervaltime = 0;
	int dispatchtime = 0;
	int processtime = 0;
    int sleeptime = 0;
	
	prctl(PR_SET_NAME, "micpthread");

	fdbg = fopen("debug/debug_micpthread.log", "w");
	if (NULL == fdbg)
	{
		log_e("[micpthread] fopen debug/debug_micpthread.log failed");
	}

	FILE * fmic16k = fopen("./debug/alg_mic.pcm", "wb+");
	if (NULL == fmic16k)
	{
		log_e("[micpthread] fopen debug/aec_mic.pcm failed");
	}

	faec = fopen("debug/alg_aec.pcm", "wb+");
	if (NULL == faec)
	{
		log_e("[micpthread] fopen debug/cpy_micpthread.log failed");
	}

	log_i("[micpthread] micparams addr: %p, framesamples: %d", args, pmicparams->framesamples);
    if (NULL == pmicparams)
    {
        log_a("[micpthread] mic pthread params is null");
		*pmicparams->pbfinished = true;
        return NULL;
    }

	fmic = fopen(pmicparams->sfilepath, "rb");
	if (NULL == fmic)
    {
        log_e("[micpthread] fopen %s failed", pmicparams->sfilepath);
		*pmicparams->pbfinished = true;
		return NULL;
    }
	log_i("[micpthread] fopen fmic: %p, path: %s", fmic, pmicparams->sfilepath);

	*pmicparams->pbfinished = false;

	dispatchcycle = (pmicparams->framesamples*1000000)/pmicparams->worksamplerate;
	log_i("[micpthread] dispatchcycle: %d", dispatchcycle);
	gettimeofday(&time2, NULL);
	gettimeofday(&time3, NULL);

	static unsigned long ulPkgNo = 0;
    while(*pmicparams->pbcontinue)
    {

		gettimeofday(&time0, NULL);
		intervaltime = difftimeval(&time0, &time3);
		time3 = time0;

        if (fread(s8mic, 1, sizeof(short)*pmicparams->framesamples, fmic) != sizeof(short)*pmicparams->framesamples)
        {
            log_i("[micpthread] finish read all mic data");
			*pmicparams->pbcontinue = false;
            break;
        }

		//@@@audio data send to aecthread
		if (fmic16k) fwrite(s8mic,1,sizeof(short)*MAX_SAMPLES_10MS, fmic16k);

		char* pMicData = (char*)s8mic;
		g_aec.DoAEC(pMicData,MAX_SAMPLES_10MS*sizeof(short));
		if (faec) fwrite(pMicData,1,sizeof(short)*MAX_SAMPLES_10MS, faec);

		ulPkgNo++;
		//@@end

		gettimeofday(&time1, NULL);

		// time0 - time2 dispatch time
		// time1 - time0 process  time
		dispatchtime = difftimeval(&time0, &time2);
		processtime = difftimeval(&time1, &time0);
		sleeptime = dispatchcycle - (dispatchtime + processtime);
		if (intervaltime > dispatchcycle)
		{
			sleeptime -= intervaltime - dispatchcycle;
		}
		if (fdbg)
		{
			fprintf(fdbg, "[micpthread] dispatch: %d, %d, %d, %d\n", dispatchtime, processtime, sleeptime, intervaltime);
		}
		if (sleeptime > 0) 
		{
			usleep(sleeptime);
		}
		gettimeofday(&time2, NULL);
    }

    if (fmic)
    {
        fclose(fmic);
    }
   
    log_i("[micpthread] mic pthread exiting...");

	*pmicparams->pbfinished = true;

    return NULL;
}

static void *refpthreadfunc(void *args)
{
    RefThreadParams *prefparams = (RefThreadParams *)args;
    char s8ref[MAX_SAMPLES_10MS*sizeof(short)];
    FILE *fref = NULL;
	FILE *fdbg = NULL;
	FILE *fcpy = NULL;
	struct timeval time0 = {0};
    struct timeval time1 = {0};
	struct timeval time2 = {0};
	struct timeval time3 = {0};
    int dispatchcycle = 0;
	int intervaltime = 0;
	int dispatchtime = 0;
	int processtime = 0;
    int sleeptime = 0;

	prctl(PR_SET_NAME, "refpthread");

	fdbg = fopen("debug/debug_refpthread.log", "w");
	if (NULL == fdbg)
	{
		log_e("[refpthread] fopen debug/debug_refpthread.log failed");
	}

	log_i("[refpthread] refparams addr: %p, framesamples: %d", args, prefparams->framesamples);
    if (NULL == prefparams)
    {
        log_e("[refpthread] ref pthread params is null");
		*prefparams->pbfinished = true;
        return NULL;
    }

	FILE* fref16k = fopen("debug/alg_ref.pcm", "wb+");
	if (NULL == fref16k)
	{
		printf("[micpthread] fopen alg_ref.pcm failed\n");
	}

	fref = fopen(prefparams->sfilepath, "rb");
	if (NULL == fref)
    {
        log_e("[refpthread] fopen %s failed", prefparams->sfilepath);
		*prefparams->pbfinished = true;
		return NULL;
    }

	log_i("[refpthread] fopen fref: %p, path: %s", fref, prefparams->sfilepath);
	
	*prefparams->pbfinished = false;

	dispatchcycle = (prefparams->framesamples*1000000)/prefparams->worksamplerate;
	log_i("[refpthread] dispatchcycle: %d", dispatchcycle);
	gettimeofday(&time2, NULL);
	gettimeofday(&time3, NULL);
	static unsigned long ulPkgNo_ref = 0;
    while(*prefparams->pbcontinue)
    {
		gettimeofday(&time0, NULL);
		intervaltime = difftimeval(&time0, &time3);
		time3 = time0;

        if (fread(s8ref, 1, sizeof(short)*prefparams->framesamples, fref) !=sizeof(short)*prefparams->framesamples)
        {
            log_i("[refpthread] finish read all ref data");
			*prefparams->pbcontinue = false;
            break;
        }

		//@@@audio data send to aecthread
	
		char* pRefData = (char*)s8ref;
		g_aec.SndRefFrame(pRefData);
	
		ulPkgNo_ref++;

		//@@end

		gettimeofday(&time1, NULL);

		// time0 - time2 dispatch time
		// time1 - time0 process  time
		dispatchtime = difftimeval(&time0, &time2);
		processtime = difftimeval(&time1, &time0);
		sleeptime = dispatchcycle - (dispatchtime + processtime);
		if (intervaltime > dispatchcycle)
		{
			sleeptime -= intervaltime - dispatchcycle;
		}
		if (fdbg)
		{
			fprintf(fdbg, "[refpthread] dispatch: %d, %d, %d, %d\n", dispatchtime, processtime, sleeptime, intervaltime);
		}
		if (sleeptime > 0) 
		{
			usleep(sleeptime);
		}
		gettimeofday(&time2, NULL);
    }

    if (fref)
    {
        fclose(fref);
    }
  
    log_i("[refpthread] ref pthread exiting...");

	*prefparams->pbfinished = true;

    return NULL;
}
