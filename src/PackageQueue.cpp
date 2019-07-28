#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "PackageQueue.h"


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

BaseBuffer::BaseBuffer(char* pBuf, int size)
{
    m_nSize = size;
    m_pBuf = new char[m_nSize];
    memcpy(m_pBuf, pBuf, size);
}

BaseBuffer::BaseBuffer(const BaseBuffer& other)
{
    m_nSize = other.m_nSize;
    m_pBuf = new char[m_nSize];
    memcpy(m_pBuf, other.m_pBuf, other.m_nSize);
}

BaseBuffer::~BaseBuffer()
{
    delete[] m_pBuf;
}


AAudioBuffer::AAudioBuffer(char* pBuf, int size, long long sysTimeRecordedOrPlayed)
    : BaseBuffer(pBuf, size)
    , AECInfo(sysTimeRecordedOrPlayed)
{
}

AAudioBuffer::AAudioBuffer(const AAudioBuffer& other)
    : BaseBuffer(other.m_pBuf, other.m_nSize)
    , AECInfo(other.m_sysTimeRecordedOrPlayed)
{
}

AAudioBuffer::~AAudioBuffer()
{
}

PlayBuffer::PlayBuffer(char* pBuf, int size, uint32_t seqNumber, uint32_t extSeqNumber, double sysTimeReceived)
    : BaseBuffer(pBuf, size),
    RTPPackageInfo(seqNumber, extSeqNumber, sysTimeReceived)
{
}

PlayBuffer::PlayBuffer(const PlayBuffer& other)
    : BaseBuffer(other.m_pBuf, other.m_nSize),
    RTPPackageInfo(other.m_seqNumber, other.m_extSeqNumber, other.m_sysTimeReceived)
{
}

PlayBuffer::~PlayBuffer()
{
}

bool AECQueue::FindMatch(double offset, double maxTimeInterval, int& idxMic, int& idxRef)
{
    //const double dMaxInterval = 10000;
    //double minInterval = dMaxInterval;
    double dMic = 0;
    double dRef = 0;
#if 0
    SPRINTF_S(dbg_str,
        "Audio data match: mic queue=%d\tref queue=%d"
        "           offset: %.4f interval: %.4f"
        , m_pQueueMic->size(), m_pQueueRef->size()
        , offset, maxTimeInterval);
    DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO

    bool bMatch = false;
    bool bMicBehindOfPlay = false;
    m_cs.Lock();

    for (size_t i=0; !bMatch && i < m_pQueueMic->size(); ++i)
    {
#if 0 
        SPRINTF_S(dbg_str,"Audio data match: mic index=%d", i);
        DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
        bMicBehindOfPlay = false;
        RecPkgQueue::const_reference& pBufMic = m_pQueueMic->at(i);

        dMic = pBufMic->GetRecordedOrPlayedTime();

        for (size_t j=0; !bMatch && j < m_pQueueRef->size(); ++j)
        //size_t j=0;
        {
#if 0 
            SPRINTF_S(dbg_str,"Audio data match: ref index=%d", j);
            DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO

            RecPkgQueue::const_reference& pBufRef = m_pQueueRef->at(j);

            dRef = pBufRef->GetRecordedOrPlayedTime();

#if 0 
            SPRINTF_S(dbg_str,"Audio data time of mic[%d]: %f\tref[%d]: %f", i, dMic, j, dRef);
            DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO

            //double interval = dMic - dRef/* - maxTimeInterval*/;
            double interval = (dMic - offset) - dRef/* - maxTimeInterval*/;

            if (interval < 0)
            { // mic now ahead of this play
                if (bMicBehindOfPlay) // last play ahead of this mic
                {
                    idxMic = i;
                    idxRef = j - 1;
                    bMatch = true;
                    break;
                }
            }
            else if (interval > 0)
            { // mic now behind of this play
                bMicBehindOfPlay = true;

                if (interval < maxTimeInterval * 1.5)
                {
                    idxMic = i;
                    idxRef = j;
                    bMatch = true;
                    break;
                }
            }
        }
    }

    if (bMatch)
    {
#if 0 
        SPRINTF_S(dbg_str,"Audio data matched: ""\tat mic: %d""\tat ref: %d", idxMic, idxRef);
        DEBUG_INFO(dbg_str);
#endif//PRINT_AEC_INFO
    }
    else
    {
#if 0
        DEBUG_INFO("Audio data NO MATCH found!");
#endif//PRINT_AEC_INFO
    }

    m_cs.Unlock();
    return bMatch;
}
