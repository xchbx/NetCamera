#ifndef SPTS_RTMP_SEND_H
#define SPTS_RTMP_SEND_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "rtmp.h"
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define OSA_SOK      0  ///< Status : OK
#define OSA_EFAIL   -1  ///< Status : Generic error


/* unsigned quantities */
typedef unsigned long long UInt64;      ///< Unsigned 64-bit integer
typedef unsigned int UInt32;            ///< Unsigned 32-bit integer
typedef unsigned short UInt16;          ///< Unsigned 16-bit integer
typedef unsigned char UInt8;            ///< Unsigned  8-bit integer

/* signed quantities */
typedef long long Int64;               ///< Signed 64-bit integer
typedef int Int32;
typedef void Void;
typedef bool Bool;

#define MAX_RTMP_STREAM_CHANNEL  8

#define MAX_PROCESS_THREAD		MAX_RTMP_STREAM_CHANNEL

#define MONITOR_IN_BW	100000		//100Kbps

//#pragma once
#define MAX_RTMP_SEND_LEN 1048576	//1M Byte
#define MAX_URL_LEN			128
#define MAX_PPS_OR_SPS_LEN  128
#define MAX_VIDEO_AUDIO_CONF_LEN 1024
#define RTMP_CHUNK_SIZE 1280

#define RTMP_SEND_LEN  409600  //0.4MB, 3.2Mbps
#define RTMP_VIDEO_REV 409600   //0.4MB, 3.2Mbps
#define RTMP_AUDIO_REV 16384    //16KB,128kbps


typedef struct
{
	unsigned int nBufSize;
	unsigned int nRdPos;
	unsigned int nWrPos;

	//char byRtmpBuf[MAX_RTMP_SEND_LEN];
	char   *pRtmpBuf;
}RTMP_BUF_S;



struct adts_fixed_header
{
	unsigned char syncword_H;
	unsigned char protection_absent			: 1;
	unsigned char layer						: 2;
	unsigned char ID						: 1;
	unsigned char syncword_L				: 4;

	unsigned char channel_configuration_1	: 1;
	unsigned char private_bit				: 1;
	unsigned char sampling_frequency_index	: 4;
	unsigned char profile					: 2;

};

//aac sequence header
struct aac_conf_header
{
	unsigned char frequencyH	: 3;	//
	unsigned char profile		: 5;	//audioObjectType[5bit] 1-AAC_LC

	unsigned char reserved		: 3;
	unsigned char bit2			: 4; // 2-2channel
	unsigned char frequencyL	: 1;	//348K
};


typedef struct
{
	char *pBody;
	unsigned int nBodySize;
	unsigned int nTimeStamp;
	int nPacketType;
	int nCtrlChId;
}RTMP_GEN_PACKET_PARAMS_S;

typedef struct{
	RTMP *RtmpSock;
	char *pStreamUrl;
	Bool bConnect;
	Bool bUrlUpdate;
}SPTS_RTMP_SEND_SOCK_INFO_S;

typedef struct{
	Bool bFindSpsPps;
	char byReserved[2];
	char *pSps;
	unsigned int nSpsLen;
	char *pPps;
	unsigned int nPpsLen;
	char *pVideoConf;
	unsigned int nVideoConfLen;

	RTMP_BUF_S struVideo;

}SPTS_RTMP_SEND_VIDEO_INFO_S;

typedef struct{
	Bool bFindAacHdr;
	char byReserved[2];

	char *pAudioConf;
	unsigned int nAudioConfLen;
	RTMP_BUF_S struAudio;

}SPTS_RTMP_SEND_AUDIO_INFO_S;

typedef struct{
	unsigned int nErrTimeOut;
	unsigned int nErrOut;
	unsigned int nErrIn;
}SPTS_RTMP_SEND_ERROR_INFO_S;

typedef struct{
	char *pRtmpSendData;
	Bool bEnabled;
	Bool bSendVideoConf;
	Bool bSendAudioConf;
	Bool bSendChunkInfo;
	unsigned int nInCnt;
	unsigned int nInRingbufCnt;
	unsigned int nInRateV;
	unsigned int nInRateVRingbuf;
	unsigned int nOutCnt;
	unsigned int nOutRate;
	unsigned int nAudioSendTime;
	unsigned int nVideoSendTime;
	pthread_mutex_t mutexDataLock;
	Bool bPutDataMux;
	unsigned int nTryCnt;

}SPTS_RTMP_SEND_DATA_CTRL_S;


typedef struct
{
	pthread_t pthHandle;
	int iCh;
	int iRevSockt;
	int iThreadActive;
	int iThreadExitFlag;
	char arMulIp[20];
	unsigned short usPort;
	unsigned short usReserved;
	char *pAEsData;
	char *pVEsData;
	int iAPos;
	int iVPos;
	unsigned  int uAPts;
	unsigned  int uVPts;
}UdpRcvParam;

typedef struct{


	SPTS_RTMP_SEND_SOCK_INFO_S sRtmpSendSock[MAX_RTMP_STREAM_CHANNEL];
	SPTS_RTMP_SEND_VIDEO_INFO_S sRtmpVideoInfo[MAX_RTMP_STREAM_CHANNEL];
	SPTS_RTMP_SEND_AUDIO_INFO_S sRtmpAudioInfo[MAX_RTMP_STREAM_CHANNEL];
	SPTS_RTMP_SEND_ERROR_INFO_S sRtmpErrInfo[MAX_RTMP_STREAM_CHANNEL];
	SPTS_RTMP_SEND_DATA_CTRL_S  sRtmpDataCtrl[MAX_RTMP_STREAM_CHANNEL];

	pthread_t 	MonitorThrHndl;
    Bool 		MonitorExitThr;
    Bool 		MonitorIsThrStopDone;

	pthread_t   sendThrHndl[MAX_PROCESS_THREAD];
    Bool 		sendExitThr[MAX_PROCESS_THREAD];
    Bool 		sendIsThrStopDone[MAX_PROCESS_THREAD];


}SPTS_RTMP_SEND_Ctrl_S;

//
int  RtmpComponentInit();
int  RtmpComponentDeInit();
int  RtmpComponentStart();
int  RtmpComponentStop();

Bool Component_updateRtmp();

Void    Rtmp_printStaticInfo(char *print_buf);

//data
unsigned int PutRtmpVData(char *pBuf, unsigned int nBufLen, int nCh,unsigned int nTimeStamp);
unsigned int PutRtmpAData(char *pBuf, unsigned int nBufLen, int nCh,unsigned int nTimeStamp);

#endif

