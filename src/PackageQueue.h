#pragma once

#ifndef PACKAGE_QUEUE_H
#define PACKAGE_QUEUE_H

#include <memory>
#include <deque>
#include <stdint.h>//TODO:
#include <math.h>
#include <map>
#include <iostream>


class BaseBuffer
{
public:
    BaseBuffer(char* pBuf, int size);
    BaseBuffer(const BaseBuffer& other);
    virtual ~BaseBuffer();
    char* getBuffer() { return m_pBuf; }
    int getSize() const { return m_nSize; }

protected:
    char* m_pBuf;
    int m_nSize;
};


class AECInfo
{
public:
    AECInfo(double sysTimeRecordedOrPlayed)
        : m_sysTimeRecordedOrPlayed(sysTimeRecordedOrPlayed) {}
    virtual ~AECInfo() {}

    long long GetRecordedOrPlayedTime() const { return m_sysTimeRecordedOrPlayed; }

protected:
    long long m_sysTimeRecordedOrPlayed; // time by which audio buffer was recorded or played
};

class AAudioBuffer : public BaseBuffer, public AECInfo
{

public:
    AAudioBuffer(char* pBuf, int size, long long sysTimeRecordedOrPlayed);
    AAudioBuffer(const AAudioBuffer& other);
    virtual ~AAudioBuffer();
};


class RTPPackageInfo
{
public:
    RTPPackageInfo(uint32_t seqNumber,
                   uint32_t extSeqNumber,
                   double sysTimeReceived)
                   : m_seqNumber(seqNumber),
                     m_extSeqNumber(extSeqNumber),
                     m_sysTimeReceived(sysTimeReceived) {}
    virtual ~RTPPackageInfo() {}

    uint32_t GetSequenceNumber() const { return m_seqNumber; }
    uint32_t GetExtendedSequenceNumber() const { return m_extSeqNumber; }
    double GetReceiveTime() const { return m_sysTimeReceived; }

protected:
    uint32_t m_seqNumber;
    uint32_t m_extSeqNumber;
    double m_sysTimeReceived;
};


class PlayBuffer : public BaseBuffer, public RTPPackageInfo
{

public:
    PlayBuffer(char* pBuf, int size, uint32_t seqNumber, uint32_t extSeqNumber, double sysTimeReceived);
    PlayBuffer(const PlayBuffer& other);
    virtual ~PlayBuffer();
};


class spinlock_mutex
{
    private:
    std::atomic_flag flag;
    public:
        spinlock_mutex():flag(ATOMIC_FLAG_INIT) {}
        void Lock()
        {
            while(flag.test_and_set(std::memory_order_acquire));
        }
        void Unlock()
        {
            flag.clear(std::memory_order_release);
        }
};

template<class ELEMENT_TYPE>
class PackageQueue
{
public:
    typedef ELEMENT_TYPE element_type;
    typedef const element_type const_element_type;
    typedef std::deque<element_type> queue_type;
    typedef typename queue_type::const_iterator const_iterator;
    typedef typename queue_type::iterator iterator;
    typedef typename queue_type::size_type size_type;
    typedef typename queue_type::const_reference const_reference;
    typedef typename queue_type::reference reference;

public:
    PackageQueue() {}
    virtual ~PackageQueue() {
        clear();
    }

    void clear()
    {
        m_cs.Lock();
        m_list.clear();
        m_cs.Unlock();
    }

    iterator begin()
    {
        m_cs.Lock();
        iterator it = m_list.begin();
        m_cs.Unlock();
        return it;
    }

    /*
    iterator end()
    {
        m_cs.Lock();
        iterator it = m_list.end();
        m_cs.Unlock();
        return it;
    }
    */

    const_reference at(size_type _Pos) const
    {
        m_cs.Lock();
        const_reference cRef = m_list.at(_Pos);
        m_cs.Unlock();
        return cRef;
    }
    reference at(size_type _Pos)
    {
        m_cs.Lock();
        reference ref = m_list.at(_Pos);
        m_cs.Unlock();
        return ref;
    }

    reference front()
    {
        m_cs.Lock();
        reference ref = m_list.front();
        m_cs.Unlock();
        return ref;
    }
    const_reference front() const
    {
        m_cs.Lock();
        const_reference ref = m_list.front();
        m_cs.Unlock();
        return ref;
    }

    size_type size() const
    {
        m_cs.Lock();
        size_type sz = m_list.size();
        m_cs.Unlock();
        return sz;
    }

    void push_back(const element_type pBuf)
    {
        m_cs.Lock();
        m_list.push_back(pBuf);
        m_cs.Unlock();
    }
    void pop_front()
    {
        m_cs.Lock();
        m_list.pop_front();
        m_cs.Unlock();
    }

private:
    mutable spinlock_mutex m_cs;
    queue_type m_list;
};

typedef std::shared_ptr<AAudioBuffer>    RecordPackage;
typedef std::shared_ptr<PlayBuffer>     PlayPackage;

typedef class PackageQueue<RecordPackage>   RecPkgQueue;
typedef class PackageQueue<PlayPackage>     PlayPkgQueue;

class AECQueue
{
public:
    AECQueue(RecPkgQueue* pQueueMic, RecPkgQueue* pQueueRef)
        : m_pQueueMic(pQueueMic)
        , m_pQueueRef(pQueueRef)
    {
    }
    virtual ~AECQueue() {}

    void AddMicPackage(RecPkgQueue::element_type& mic)
    {
        m_cs.Lock();
        m_pQueueMic->push_back(mic);
        m_cs.Unlock();
    }
    void AddRefPackage(RecPkgQueue::element_type& ref)
    {
        m_cs.Lock();
        m_pQueueRef->push_back(ref);
        m_cs.Unlock();
    }

    bool FindMatch(double offset, double maxTimeInterval, int& idxMic, int& idxRef);

protected:
    RecPkgQueue* m_pQueueMic;
    RecPkgQueue* m_pQueueRef;

     mutable spinlock_mutex m_cs;
};

#endif//PACKAGE_QUEUE_H
