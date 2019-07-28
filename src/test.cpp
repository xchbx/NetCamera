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
#define LOG_TAG    "TASKMAIN"

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


#define PRINT_AEC_INFO
#define FORMAT_INFO

#define DBG_STR_SIZE 4096
static char dbg_str[DBG_STR_SIZE];
static void DEBUG_INFO(const char* InfoStr);

#ifdef FORMAT_INFO
#define SPRINTF_S sprintf
#else
#define SPRINTF_S
#endif//FORMAT_INFO

static void DEBUG_INFO(const char* InfoStr)
{
    std::cout << InfoStr << std::endl;
}

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
	return (curr.tv_sec - firstTime.tv_sec)*1000000 + (curr.tv_usec - firstTime.tv_usec);
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
//@@@mic queue
static RecPkgQueue g_pkgQueueMic;
static RecPkgQueue g_pkgQueueRef;
static RecPkgQueue g_pkgQueueROut;

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
	pthread_t aecthread;
	int status = 0;
	ElogInit();
	gettimeofday(&firstTime, NULL);

	strcpy(aecparams.sfilepath, "data/aec16k.pcm");
	aecparams.nmicdelaysamples = 40*16000/1000;
	aecparams.nrefdelaysamples = 0;
	aecparams.micframesamples  = MIC_OUT_FRAMESAMPLES;
	aecparams.refframesamples  = REF_OUT_FRAMESAMPLES;
	aecparams.aecframesamples  = AEC_FRAMESAMPLES;
	aecparams.worksamplerate   = 16000;
	aecparams.framesamples     = AEC_FRAMESAMPLES;
	aecparams.pbcontinue = (volatile bool *)(((long)&(aecparams.bcontinue[0]) + 127) & (~127));
	aecparams.pbfinished = (volatile bool *)(((long)&(aecparams.bfinished[0]) + 127) & (~127));
	aecparams.pbcreated  = (volatile bool *)(((long)&(aecparams.bcreated[0]) + 127) & (~127));
	*aecparams.pbcontinue = true;
	*aecparams.pbfinished = false;
	*aecparams.pbcreated = false;
	printf("[main] aecparams addr:%p\n", &aecparams);
	status = pthread_create(&aecthread, NULL, aecpthreadfunc, &aecparams);
	if (status)
	{
		log_a("[main] pthread_create for aec failed, return %d\n", status);
		goto exitproc;
	}
	*aecparams.pbcreated = true;

	strcpy(micparams.sfilepath, "data/mic16k.pcm");
	micparams.worksamplerate = MIC_SAMPLERATE;
	micparams.framesamples = MIC_IN_FRAMESAMPLES;
	micparams.pbcontinue = (volatile bool *)(((long)&(micparams.bcontinue[0]) + 127) & (~127));
	micparams.pbfinished = (volatile bool *)(((long)&(micparams.bfinished[0]) + 127) & (~127));
	micparams.pbcreated  = (volatile bool *)(((long)&(micparams.bcreated[0]) + 127) & (~127));
	*micparams.pbcontinue = true;
	*micparams.pbfinished = false;
	*micparams.pbcreated = false;
	log_i("[main] micparams addr:%p\n", &micparams);
	status = pthread_create(&micthread, NULL, micpthreadfunc, &micparams);
	if (status)
	{
		log_a("[main] pthread_create for mic failed, return %d\n", status);
		goto exitproc;
	}
	*micparams.pbcreated = true;

	strcpy(refparams.sfilepath, "data/ref16k.pcm");
	refparams.worksamplerate = REF_SAMPLERATE;
	refparams.framesamples = REF_IN_FRAMESAMPLES;
	refparams.pbcontinue = (volatile bool *)(((long)&(refparams.bcontinue[0]) + 127) & (~127));
	refparams.pbfinished = (volatile bool *)(((long)&(refparams.bfinished[0]) + 127) & (~127));
	refparams.pbcreated  = (volatile bool *)(((long)&(refparams.bcreated[0]) + 127) & (~127));
	*refparams.pbcontinue = true;
	*refparams.pbfinished = false;
	*refparams.pbcreated = false;
	printf("[main] refparams addr:%p\n", &refparams);
	status = pthread_create(&refthread, NULL, refpthreadfunc, &refparams);
	if (status)
	{
		log_a("[main] pthread_create for ref failed, return %d\n", status);
		goto exitproc;
	}
	*refparams.pbcreated = true;

exitproc:
	while(*refparams.pbcreated && !*refparams.pbfinished)
	{
		usleep(1000);
	}
	while(*micparams.pbcreated && !*micparams.pbfinished)
	{
		usleep(1000);
	}

	// finish read all data for mic and ref, so we could finish aec	
	*aecparams.pbcontinue = false;

	while(*aecparams.pbcreated && !*aecparams.pbfinished)
	{
		usleep(1000);
	}

	pthread_join(refthread,NULL);
	pthread_join(micthread,NULL);
	pthread_join(aecthread,NULL);

    return 0;
}

static void *micpthreadfunc(void *args)
{
    MicThreadParams *pmicparams = (MicThreadParams *)args;
    char s8mic[MAX_SAMPLES_10MS*2];
    FILE *fmic = NULL;
	FILE *fcpy = NULL;
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
		log_e("[micpthread] fopen debug/debug_micpthread.log failed\n");
	}
	fcpy = fopen("debug/cpy_micpthread.pcm", "wb");
	if (NULL == fcpy)
	{
		log_e("[micpthread] fopen debug/cpy_micpthread.log failed\n");
	}

	log_i("[micpthread] micparams addr: %p, framesamples: %d\n", args, pmicparams->framesamples);
    if (NULL == pmicparams)
    {
        log_a("[micpthread] mic pthread params is null\n");
		*pmicparams->pbfinished = true;
        return NULL;
    }

	fmic = fopen(pmicparams->sfilepath, "rb");
	if (NULL == fmic)
    {
        log_e("[micpthread] fopen %s failed\n", pmicparams->sfilepath);
		*pmicparams->pbfinished = true;
		return NULL;
    }
	log_i("[micpthread] fopen fmic: %p, path: %s\n", fmic, pmicparams->sfilepath);

	*pmicparams->pbfinished = false;

	dispatchcycle = (pmicparams->framesamples*1000000)/pmicparams->worksamplerate;
	log_i("[micpthread] dispatchcycle: %d\n", dispatchcycle);
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
            log_i("[micpthread] finish read all mic data\n");
			*pmicparams->pbcontinue = false;
            break;
        }

		//@@@audio data send to aecthread
		long long lCurTime = getSysTimeInMsec();
		RecPkgQueue::element_type bufMIC(new AAudioBuffer((char*)s8mic, sizeof(short)*pmicparams->framesamples, lCurTime));
		g_pkgQueueMic.push_back(bufMIC);

#if 0
        SPRINTF_S(dbg_str,"Audio data: mic send= %d""\ttime: %lld", ulPkgNo, lCurTime);
        DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
		ulPkgNo++;
		//@@end

		if (fcpy)
		{
			fwrite(s8mic, 1, sizeof(short)*pmicparams->framesamples, fcpy);
		}

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
   
    log_i("[micpthread] mic pthread exiting...\n");

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
		log_e("[refpthread] fopen debug/debug_refpthread.log failed\n");
	}
	fcpy = fopen("debug/cpy_refpthread.pcm", "wb");
	if (NULL == fcpy)
	{
		log_e("[refpthread] fopen debug/cpy_refpthread.log failed\n");
	}

	log_i("[refpthread] refparams addr: %p, framesamples: %d\n", args, prefparams->framesamples);
    if (NULL == prefparams)
    {
        log_e("[refpthread] ref pthread params is null\n");
		*prefparams->pbfinished = true;
        return NULL;
    }

	fref = fopen(prefparams->sfilepath, "rb");
	if (NULL == fref)
    {
        log_e("[refpthread] fopen %s failed\n", prefparams->sfilepath);
		*prefparams->pbfinished = true;
		return NULL;
    }

	log_i("[refpthread] fopen fref: %p, path: %s\n", fref, prefparams->sfilepath);
	
	*prefparams->pbfinished = false;

	dispatchcycle = (prefparams->framesamples*1000000)/prefparams->worksamplerate;
	log_i("[refpthread] dispatchcycle: %d\n", dispatchcycle);
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
            log_i("[refpthread] finish read all ref data\n");
			*prefparams->pbcontinue = false;
            break;
        }

		//@@@audio data send to aecthread
		long long lCurTime = getSysTimeInMsec();
		RecPkgQueue::element_type bufREF(new AAudioBuffer((char*)s8ref, sizeof(short)*prefparams->framesamples, lCurTime));
		g_pkgQueueRef.push_back(bufREF);
#if 0
                SPRINTF_S(dbg_str,
                    "Audio data: ref send= %d"
                    "\ttime: %lld"
                    , ulPkgNo_ref
                    , lCurTime);
                DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
		ulPkgNo_ref++;

		//@@end
		if (fcpy)
		{
			fwrite(s8ref, 1, sizeof(short)*prefparams->framesamples, fcpy);
		}

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
  
    log_i("[refpthread] ref pthread exiting...\n");

	*prefparams->pbfinished = true;

    return NULL;
}

#define AEC_TIME_OFFSET (0.0) // 300 ms, reverberation time for a small room
#define MAX_AEC_TIME_INTERVAL (128.0/8000) // max time interval between record data and play data, 0.0125 for 200 B (100 samples)

static void *aecpthreadfunc(void *args)
{
    AecThreadParams *paecparams = (AecThreadParams *)args;
    FILE *faec = NULL;
	FILE *fdbg = NULL;
	FILE *fmic0 = NULL;
	FILE *fref0 = NULL;
	void *haec = NULL;
    
    bool  bmicresample = false;
    bool  brefresample = false;
    bool  baecresample = false;
	bool  bmicdelayed  = false;
    bool  brefdelayed  = false;
	int   subframesamples = 0;
	int   framesamples = 0;
    int   numbands = 0;
	int   status = 0;

	struct timeval time0 = {0};
    struct timeval time1 = {0};
	struct timeval time2 = {0};
	struct timeval time3 = {0};
    int dispatchcycle = 0;
	int intervaltime = 0;
	int dispatchtime = 0;
	int processtime = 0;
    int sleeptime = 0;

	prctl(PR_SET_NAME, "aecpthread");

	fdbg = fopen("debug/debug_aecpthread.log", "w");
	if (NULL == fdbg)
	{
		log_e("[aecpthread] fopen debug/debug_aecpthread.log failed\n");
	}

	log_i("[aecpthread] aecparams addr: %p, %d, %d, %d\n", args, paecparams->micframesamples, paecparams->refframesamples, paecparams->aecframesamples);
    if (NULL == paecparams)
    {
        log_e("[aecpthread] aec pthread params is null\n");
		*paecparams->pbfinished = true;
        return NULL;
    }
	

	framesamples = (paecparams->worksamplerate*10)/1000;

	*paecparams->pbfinished = false;
    RecPkgQueue* pQueueMic = &g_pkgQueueMic;
    RecPkgQueue* pQueueRef = &g_pkgQueueRef;
    RecPkgQueue* pQueueOut = &g_pkgQueueROut;

    AECQueue aecQueue(pQueueMic, pQueueRef);

    int nSentMiced = 0;
    double dLastMatchedMic = 0,
           dLastMatchedRef = 0;
    bool bMatchedBefore = false;
	
	char* pAecOut = NULL;
	EchoCanceller aec;
	aec.Init();
	pAecOut = new char[320];


	//@@@aec init begain

	faec = fopen(paecparams->sfilepath, "wb");
	if (NULL == faec)
    {
        log_a("[aecpthread] fopen %s failed\n", paecparams->sfilepath);
		goto exitproc;
    }
	//@@@aec init end

    while(*paecparams->pbcontinue)
    {
		//printf("[aecpthread]------------------------------ 0 \n");

		while (pQueueMic->size())
        {
			//printf("[aecpthread]------------------------------ 1 \n");
            if ( pQueueRef->size() == 0 )
            {
				//TODO: no play data, if never did AEC, record data is ok to send out
				//      otherwise, reserve for sync & AEC
				if (!bMatchedBefore)
				{
#if 0
					SPRINTF_S(dbg_str,
						"Audio data: mic recv= %d"
						, nSentMiced);
					DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
					++nSentMiced;
					pQueueMic->pop_front();
				}
			}
            else
            {
                int idxMic = 0, idxRef = 0;
                bool bMatch = aecQueue.FindMatch(AEC_TIME_OFFSET, MAX_AEC_TIME_INTERVAL, idxMic, idxRef);
				if (bMatch)
				{
					bMatchedBefore = true;

					while (idxMic > 0)
					{
						pQueueMic->pop_front();
						--idxMic;
					}
					while (idxRef > 1) // TODO: reserve 1 play for later match
					{
						pQueueRef->pop_front();
						--idxRef;
					}

					RecPkgQueue::const_reference& pBufMic = pQueueMic->front();
					RecPkgQueue::reference& pBufRef = pQueueRef->front();
					if (idxRef == 1)
						pBufRef = pQueueRef->at(1);

					const int nSizeMic = pBufMic->getSize();
					const int nSizeRef = pBufRef->getSize();
					if(nSizeMic != nSizeRef)
					{
						log_i("[aecpthread] nSizeMic = %d, nSizeRef = %d\n",nSizeMic,nSizeRef);
						continue;
					}
					aec.DoAEC(pBufMic->getBuffer(), pBufRef->getBuffer(), pAecOut);
					fwrite(pAecOut, 1,sizeof(short)*paecparams->aecframesamples, faec);
					pQueueMic->pop_front();
					pQueueRef->pop_front();
				}
				else
				{
					// if can't found matched audio data, reserve it for later use
					break;
#ifdef PRINT_AEC_INFO
					SPRINTF_S(dbg_str,"Audio data match failed, ref queue = %lu", pQueueRef->size());
					DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
				}
			}//pQueueRef->size() >0
            usleep(10*1000);
        }//while (pQueueMic->size())
		usleep(1000*1000);		
    }

exitproc:
    if (faec)
    {
        fclose(faec);
    }

    log_i("[aecpthread] aec pthread exiting...\n");

	*paecparams->pbfinished = true;

    return NULL;    
}
