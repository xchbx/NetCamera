#include <pthread.h>
#include <sys/time.h>
#include "rtmpSend.h"
#include "demuxEs.h"

#define MAX_ES_BUF  0x400000
#define PID_AUDIO		49
#define PID_VIDEO		48

static SPTS_RTMP_SEND_Ctrl_S SPTS_RTMP_Send_Ctrl;
static UdpRcvParam g_tParam[ MAX_PROCESS_THREAD];
static int gCh[8] = {0,1,2,3,4,5,6,7};

void Rtmp_handle_Callback(int iCh,char* pBuff,int iLen)
{
	int iPos = 0;
	int ret = 0;

	while(iPos < iLen)
	{
		char *pStart = pBuff + iPos;
		int iEsStart = GetStartFlag(pStart);
		int iPid = GetPid(pStart);
		if(iPid == PID_VIDEO)
		{
			if(iEsStart)
			{
				//printf("Get Video Len %d\n", g_tParam[iCh].iVPos);
				PutRtmpVData(g_tParam[iCh].pVEsData, g_tParam[iCh].iVPos, iCh,g_tParam[iCh].uVPts/90);
				g_tParam[iCh].iVPos = 0;
			}
			ret = GetEsPayload(pStart,&g_tParam[iCh].uVPts, g_tParam[iCh].pVEsData + g_tParam[iCh].iVPos, MAX_ES_BUF - g_tParam[iCh].iVPos);

			if(ret >0)
			{
				g_tParam[iCh].iVPos += ret;
			}
		}
		else if(iPid == PID_AUDIO)
		{
			if(iEsStart)
			{
				//printf("Get Audio Len %d\n", g_tParam[iCh].iAPos);
				PutRtmpAData(g_tParam[iCh].pAEsData , g_tParam[iCh].iAPos, iCh,g_tParam[iCh].uAPts/90);
				g_tParam[iCh].iAPos = 0;
			}
			ret = GetEsPayload(pStart, &g_tParam[iCh].uAPts, g_tParam[iCh].pAEsData + g_tParam[iCh].iAPos, MAX_ES_BUF - g_tParam[iCh].iAPos);
			if(ret >0)
			{
				g_tParam[iCh].iAPos += ret;
			}
		}
		iPos += 188;
	}
}

static int CreateSocket(int iCh)
{
	int SocketFd = -1;
	struct sockaddr_in  ServerAddr;
	in_addr_t           IpAddr;
	struct ip_mreq      Mreq;
	int              AddrLen = sizeof(ServerAddr);
	char *pMulAddr = g_tParam[iCh].arMulIp;
	unsigned short usPort = g_tParam[iCh].usPort;

	SocketFd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (SocketFd < 0)
	{
		printf("create socket error [%d].\n", errno);
		return -1;
	}

	IpAddr = inet_addr(pMulAddr);
	printf("ip = %s:%d\n",pMulAddr,usPort);

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = IpAddr;
	ServerAddr.sin_port = htons(usPort);
/*
	int nOptval = 1;
	if (setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR,(const void *)&nOptval , sizeof(int)) < 0)
	{
		printf("set sock reuse fail \n");
		close(SocketFd);
		return -1;
	}
*/
	if (bind(SocketFd,(struct sockaddr *)(&ServerAddr),sizeof(struct sockaddr_in)) < 0)
	{
		printf("socket bind error [%d].\n", errno);
		close(SocketFd);
		return -2;
	}

	if (IpAddr)
	{
		Mreq.imr_multiaddr.s_addr = IpAddr;
		Mreq.imr_interface.s_addr = inet_addr("192.168.15.101");
		if (setsockopt(SocketFd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &Mreq, sizeof(struct ip_mreq)))
		{
			printf("Socket setsockopt ADD_MEMBERSHIP error [%d].\n", errno);
			close(SocketFd);
			return -3;
		}
	}

	g_tParam[iCh].iRevSockt = SocketFd;
	return 0;
}

static int closeSocket(int iCh)
{
	if(0 < g_tParam[iCh].iRevSockt)
	{
		close(g_tParam[iCh].iRevSockt);
		g_tParam[iCh].iRevSockt = -1;
	}
	return 0;
}



void *udpRcv_process(void *arg)
{
	int iCh = *(int*)arg;
	printf("iCh = %d \n",iCh);
	int iRet;
	iRet = CreateSocket(iCh);
	if(iRet < 0)
	{
		printf("CreateSocket Create error\n");
	}
	while (g_tParam[iCh].iThreadActive)
	{
		char buf[1600];
		int iReadLen = recv(g_tParam[iCh].iRevSockt, buf, sizeof(buf), 0);
		if (iReadLen > 0)
		{
			//callback
			//printf("recv...... 1 (%d) len = %d \n",iCh,iReadLen);
			Rtmp_handle_Callback(iCh, buf, iReadLen);
		}
		//printf("recv...... 2 (%d) len = %d \n",iCh,iReadLen);
	}
	closeSocket(iCh);

	g_tParam[iCh].iThreadExitFlag = TRUE;
}



int Conponent_UdpRcv_Init()
{
	int status, i;

	for (i = 0; i <  MAX_PROCESS_THREAD; i++)
	{
		sprintf(g_tParam[i].arMulIp, "231.0.16.10%d", i);
		printf("[debug]===>ip = %s\n",g_tParam[i].arMulIp);
		g_tParam[i].usPort = 5001;
		g_tParam[i].iThreadActive = TRUE;
		g_tParam[i].iThreadExitFlag = FALSE;
		g_tParam[i].iRevSockt = -1;
		g_tParam[i].pAEsData = malloc(MAX_ES_BUF);
		g_tParam[i].pVEsData = malloc(MAX_ES_BUF);
		if((NULL == g_tParam[i].pVEsData) || (NULL == g_tParam[i].pAEsData))
		{
			printf("pEsData malloc error\n");
			return -1;
		}
		g_tParam[i].iAPos = 0;
		g_tParam[i].iVPos = 0;
		g_tParam[i].uAPts = 0;
		g_tParam[i].uVPts = 0;
		g_tParam[i].iCh   = i;

	}

	for (i =0;i <  MAX_PROCESS_THREAD; i++)
	{
		printf("i = %d\n",i);
		status = 0;
		if(i == 1)
		{
			status = pthread_create(&g_tParam[i].pthHandle, NULL, udpRcv_process,&g_tParam[i].iCh);
		}
		if (status != 0)
		{
			g_tParam[i].iThreadExitFlag = FALSE;
			printf ("\r\n udp REV thread create failed...");
			return -1;
		}
	}
	//pthread_delete(&g_tParam[i].pthHandle);
	return 0;

}


void Conponent_UdpRcv_DeInit()
{
	int i,quit = FALSE;
	for (i = 0; i <  MAX_PROCESS_THREAD; i++)
	{
		g_tParam[i].iThreadActive = FALSE;
	}

	while (!quit)
	{
		quit = TRUE;
		for (i = 0; i <  MAX_PROCESS_THREAD; i++)
		{
			if(!g_tParam[i].iThreadExitFlag) quit = FALSE;

			if(g_tParam[i].pAEsData != NULL)
			{
				free(g_tParam[i].pAEsData);
				g_tParam[i].pAEsData = NULL;
			}

			if(g_tParam[i].pVEsData != NULL)
			{
				free(g_tParam[i].pVEsData);
				g_tParam[i].pVEsData = NULL;
			}
		}
		usleep(300000);
	}
}


char rtmp_url_ip[MAX_RTMP_STREAM_CHANNEL][128] = {
	"rtmp://192.168.15.50:1935/live/100",
	"rtmp://192.168.15.40:1935/live/123",
	"rtmp://192.168.15.50:1935/live/102",
	"rtmp://192.168.15.50:1935/live/103",
	"rtmp://192.168.15.50:1935/live/104",
	"rtmp://192.168.15.50:1935/live/105",
	"rtmp://192.168.15.50:1935/live/106",
	"rtmp://192.168.15.50:1935/live/107"
};

Int32 Component_GetRtmp_Inf(UInt8 bych,char *pString, Bool *Enable)
{
	int len  = 0;
	*Enable = 1;
	pString[0] = '\0';

	len = strlen(rtmp_url_ip[bych]);
	if(128 > len)
	{
		strcpy(pString, rtmp_url_ip[bych]);
	}

	return 0;

}

Bool bIsLess(unsigned int nLeft, unsigned int nRight)
{
#define DELTA_TIME	10000 	//10s
	if((nLeft - nRight < DELTA_TIME) || (nRight - nLeft < DELTA_TIME))
	{
		return nLeft < nRight;
	}
	else
	{
		return nRight < nLeft;
	}
}


int RtmpGetBufNullSize(RTMP_BUF_S *pRtmpBuf)
{
	if(pRtmpBuf->nRdPos <= pRtmpBuf->nWrPos)
	{
		return pRtmpBuf->nBufSize - pRtmpBuf->nWrPos -1 + pRtmpBuf->nRdPos;
	}

	return pRtmpBuf->nRdPos - pRtmpBuf->nWrPos -1;
}

int RtmpGetDataSize(RTMP_BUF_S *pRtmpBuf)
{
	if(pRtmpBuf->nRdPos<=pRtmpBuf->nWrPos)
	{
		return pRtmpBuf->nWrPos - pRtmpBuf->nRdPos;
	}

	return pRtmpBuf->nBufSize-pRtmpBuf->nRdPos+pRtmpBuf->nWrPos;
}

void RtmpEmpty(RTMP_BUF_S *pRtmpBuf)
{
	if(pRtmpBuf->nRdPos == pRtmpBuf->nWrPos)
	{
		return ;
	}

	pRtmpBuf->nRdPos = pRtmpBuf->nWrPos;

	return ;

}

Bool RtmpWrite(RTMP_BUF_S *pRtmpBuf,char *pData,int nDataLen)
{
	int nNullBufSize = RtmpGetBufNullSize(pRtmpBuf);
	if(nNullBufSize<nDataLen)
	{
		//printf("<RtmpWrite>there is no enough buffer!!!nNullSize:%d,nDataLen:%d\n",nNullBufSize,nDataLen);
		return FALSE;
	}

	if(pRtmpBuf->nBufSize-pRtmpBuf->nWrPos>=nDataLen)
	{
		memcpy(pRtmpBuf->pRtmpBuf+pRtmpBuf->nWrPos,pData,nDataLen);
	}
	else
	{
		memcpy(pRtmpBuf->pRtmpBuf+pRtmpBuf->nWrPos,pData,pRtmpBuf->nBufSize-pRtmpBuf->nWrPos);
		memcpy(pRtmpBuf->pRtmpBuf,pData+(pRtmpBuf->nBufSize-pRtmpBuf->nWrPos),nDataLen-(pRtmpBuf->nBufSize-pRtmpBuf->nWrPos));
	}
	pRtmpBuf->nWrPos += nDataLen;
	pRtmpBuf->nWrPos %= pRtmpBuf->nBufSize;

	return TRUE;
}


Bool RtmpRead(RTMP_BUF_S *pRtmpBuf,char *pBuf,int nNeedLen)
{
	int nDataLen = RtmpGetDataSize(pRtmpBuf);
	if(nDataLen<nNeedLen)
	{
		return FALSE;
	}

	if(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos>=nNeedLen)
	{
		memcpy(pBuf,pRtmpBuf->pRtmpBuf+pRtmpBuf->nRdPos,nNeedLen);
	}
	else
	{
		memcpy(pBuf,pRtmpBuf->pRtmpBuf+pRtmpBuf->nRdPos,pRtmpBuf->nBufSize-pRtmpBuf->nRdPos);
		memcpy(pBuf+(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos),pRtmpBuf->pRtmpBuf,nNeedLen-(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos));
	}

	pRtmpBuf->nRdPos += nNeedLen;
	pRtmpBuf->nRdPos %= pRtmpBuf->nBufSize;

	return TRUE;
}

Bool RtmpGetTime(unsigned int
		*pTimeStamp,RTMP_BUF_S *pRtmpBuf)
{
	int nDataLen = RtmpGetDataSize(pRtmpBuf);
	if(nDataLen<=8)
	{
		return FALSE;
	}

	if(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos>=4)
	{
		memcpy((char*)pTimeStamp,pRtmpBuf->pRtmpBuf+pRtmpBuf->nRdPos,4);
	}
	else
	{
		memcpy((char*)pTimeStamp,pRtmpBuf->pRtmpBuf+pRtmpBuf->nRdPos,pRtmpBuf->nBufSize-pRtmpBuf->nRdPos);
		memcpy((char*)pTimeStamp+(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos),pRtmpBuf->pRtmpBuf,4-(pRtmpBuf->nBufSize-pRtmpBuf->nRdPos));
	}

	return TRUE;
}


Bool InitSock(int nCh)
{
	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock=RTMP_Alloc();
	}

	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl = (char*)malloc(MAX_URL_LEN);
		if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl==NULL)
		{
			printf("ch %d pStreamUrl alloc failed!\n",nCh);
			return FALSE;
		}
	}
	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate = FALSE;
	return TRUE;
}

Bool DeInitSock(int nCh)
{
	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl!=NULL)
	{

		free(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl);
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl = NULL;
	}
	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock!=NULL)
	{

		RTMP_Free(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock);
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock = NULL;
	}


	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate = FALSE;

	return TRUE;
}

Bool InitVideoInfo(int nCh)
{
	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps = FALSE;


	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen = 0;
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps = (char*)malloc(MAX_PPS_OR_SPS_LEN);
		if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps==NULL)
		{
			printf("ch %d video sps alloc failed !\n",nCh);
			return FALSE;
		}
	}


	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen = 0;
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps = (char*)malloc(MAX_PPS_OR_SPS_LEN);
		if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps==NULL)
		{
			printf("ch %d video pps alloc failed!\n",nCh);
			return FALSE;
		}
	}
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf = (char*)malloc(RTMP_VIDEO_REV);

		if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf==NULL)
		{
			printf("ch %d,video receive buf alloc failed!\n",nCh);
		}
	}
	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nBufSize = RTMP_VIDEO_REV;
	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nRdPos = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nWrPos = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nVideoConfLen = 0;
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf == NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf = (char*)malloc(MAX_VIDEO_AUDIO_CONF_LEN + RTMP_MAX_HEADER_SIZE);
		if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf == NULL)
		{
			printf("ch %d video conf alloc failed!\n",nCh);
			return FALSE;
		}
	}
	return TRUE;
}

Bool DeInitVideoInfo(int nCh)
{


	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf!=NULL)
	{

		free(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf);
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf = NULL;
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nVideoConfLen = 0;
	}
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf!=NULL)
	{
		free(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf);
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.pRtmpBuf=NULL;
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nBufSize = 0;
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nRdPos = 0;

		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo.nWrPos = 0;
	}

	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps = FALSE;

	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps!=NULL)
	{

		free(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps);

		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps = NULL;
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen = 0;
	}
	if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps!=NULL)
	{

		free(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps);

		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps = NULL;
		SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen = 0;
	}


	return TRUE;
}

Bool InitAudioInfo(int nCh)
{
	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr = FALSE;
	if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf == NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf =
			(char*)malloc(MAX_VIDEO_AUDIO_CONF_LEN + RTMP_MAX_HEADER_SIZE);
		if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf == NULL)
		{
			printf("ch %d audio conf alloc failed!\n",nCh);
			return FALSE;
		}
	}
	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].nAudioConfLen = 0;

	if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf == NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf = (char*)malloc(RTMP_AUDIO_REV);
		if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf == NULL)
		{
			printf("ch %d,Audio receive buf alloc failed!\n",nCh);
			return FALSE;
		}
	}
	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nBufSize = RTMP_AUDIO_REV;

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nRdPos = 0;

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nWrPos = 0;
	return TRUE;
}

Bool DeInitAudioInfo(int nCh)
{

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr = FALSE;

	if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf != NULL)
	{
		free(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf);
		SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.pRtmpBuf = NULL;
	}

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nBufSize = 0;

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nRdPos = 0;

	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio.nWrPos = 0;

	if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf != NULL)
	{


		free(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf);
		SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf = NULL;
		SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].nAudioConfLen = 0;
	}


	return TRUE;
}

Bool InitErrStatic(int nCh)
{
	SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut = 0;
	return TRUE;
}

Bool InitDataCtrl(int nCh)
{
	if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData==NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData = (char*)malloc(RTMP_SEND_LEN);
		if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData==NULL)
		{
			printf("ch %d RtmpSendData alloc failed!\n",nCh);
			return FALSE;
		}
	}
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendChunkInfo = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateV = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateVRingbuf = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nAudioSendTime = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nVideoSendTime = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = FALSE;
	return TRUE;
}

Bool DeInitDataCtrl(int nCh)
{

	if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData!=NULL)
	{

		free(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData);
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData = NULL;
	}


	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled = FALSE;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt =0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateV = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutCnt = 0;
	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutRate = 0;

	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = FALSE;
	return TRUE;

}

Bool RtmpInit()
{
	int nCh=0;
	Bool bRet = FALSE;
	memset(&SPTS_RTMP_Send_Ctrl,0x00,sizeof(SPTS_RTMP_Send_Ctrl));

	for(nCh=0;nCh<MAX_RTMP_STREAM_CHANNEL;nCh++)
	{

		/*data ctrl*/
		bRet = InitDataCtrl(nCh);
		if(!bRet)
		{
			printf("<RtmpInit> ch %d Init data ctrl failed!\n", nCh);
			return FALSE;
		}

		/*err static*/
		bRet = InitErrStatic(nCh);
		if(!bRet)
		{
			printf("<RtmpInit>ch %d Init err static failed!\n", nCh);
			return FALSE;
		}

		/*audio init*/
		bRet = InitAudioInfo(nCh);
		if(!bRet)
		{
			printf("<RtmpInit>ch %d Init audio failed!\n", nCh);
			return FALSE;
		}

		/*video init*/
		bRet = InitVideoInfo(nCh);
		if(!bRet)
		{
			printf("<RtmpInit>ch %d Init video failed!\n", nCh);
			return FALSE;
		}

		/*sock init*/
		bRet = InitSock(nCh);
		if(!bRet)
		{
			printf("<RtmpInit>ch %d Init sock failed!\n", nCh);
			return FALSE;
		}

		//@haiping del
		Component_GetRtmp_Inf(nCh,
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl,
		&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled);
	}

	return TRUE;

}

Bool RtmpDeInit()
{
	int nCh=0;

	for(nCh=0;nCh<MAX_RTMP_STREAM_CHANNEL;nCh++)
	{
		/*data ctrl*/
		DeInitDataCtrl(nCh);


		/*audio deinit*/
		DeInitAudioInfo(nCh);


		/*video deinit*/
		DeInitVideoInfo(nCh);

		/*sock deinit*/
		DeInitSock(nCh);

	}
	return TRUE;

}


Bool RtmpSetURL(char * pURL,int nCh)
{
	Bool bRet = FALSE;
	if(strlen(pURL) >= MAX_URL_LEN)
	{
		printf("ch %d the url longer than the max length:%d\n",nCh, MAX_URL_LEN);
		return FALSE;
	}

	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl!=NULL)
	{
		strcpy(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl, pURL);
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate = TRUE;
		return TRUE;
	}

	return bRet;
}

Bool RtmpSetEnable(int nCh)
{
	if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled == FALSE)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled = TRUE;
	}
	return TRUE;
}

Bool RtmpConnect(int nCh)
{
	if(strlen(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl) == 0)
	{
		printf("<RtmpConnect>ch %d there is no rtmpUrl!\n", nCh);
		return FALSE;
	}
	printf("O_o!*** [RtmpConnect] connect url:%s,ch:%d\n",SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl,
			nCh);
	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
	{
		printf("<RtmpConnect>ch %d the rtmp has been connected!\n", nCh);
		return FALSE;
	}

	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock == NULL)
	{
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock = RTMP_Alloc();
		if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock == NULL)
		{
			return FALSE;
		}
	}

	RTMP_Init(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock);	//default 30s Timeout

	int nErr = RTMP_SetupURL(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock,
			SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl);
	if(nErr <= 0)
	{
		//printf("ch %d RTMP_SetupURL Error<%s>\n", nCh, SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl);
		return FALSE;
	}

	RTMP_EnableWrite(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock);

	nErr = RTMP_Connect(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock, NULL);
	if(nErr <= 0)
	{
		printf("ch %d RTMP_Connect Error\n", nCh);
		return FALSE;
	}

	nErr = RTMP_ConnectStream(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock, 0);
	if(nErr <= 0)
	{
		//printf("ch %d RTMP_ConnectStream Error\n", nCh);
		return FALSE;
	}

	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect = TRUE;

	return TRUE;
}



Bool RtmpDisConnect(int nCh)
{
	if(!SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
	{
		//printf("<RtmpDisconnect>this Rtmp has been disconnect already! ch:%d\n",nCh);
		return FALSE;
	}

	SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect = FALSE;
	if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock!=NULL)
	{

		RTMP_Close(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock);
		RTMP_Free(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock);
		SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock = NULL;
	}

	return TRUE;
}


Bool FindNal(unsigned char * pData, unsigned int nEsLen, unsigned char *pNalType,
		unsigned int *pStartPos, unsigned int *pEndPos)
{
	unsigned int nPos = 0;
	unsigned char *pEnd = pData + nEsLen;
	unsigned char byFlag = 0;

	while(pData + nPos + 6 < pEnd)
	{
		if((pData[nPos + 0] == 0x00) && (pData[nPos + 1] == 0x00) &&
				(pData[nPos + 2] == 0x00) && (pData[nPos + 3] == 0x01))
		{
			byFlag ++;

			if(1 == byFlag)
			{
				*pNalType = pData[nPos + 4] & 0x1f;
				*pStartPos = nPos;
			}
			else if(2 == byFlag)
			{
				*pEndPos = nPos;
				return TRUE;
			}
		}
		nPos++;
	}

	if(1 == byFlag)
	{
		*pEndPos = nEsLen;
		return TRUE;
	}

	return FALSE;
}

Bool RtmpGenVideoConf(int nCh)
{
	char *pBuf = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf + RTMP_MAX_HEADER_SIZE;

	//flv video tag
	pBuf[0] = 0x17;		//Frame Type[4 bit][1-keyframe]  CodecID[4bit][7-AVC]
	pBuf[1] = 0x00;		//AVC Packet Type[8bit][0-AVC sequence header]
	pBuf[2] = pBuf[3] = pBuf[4] = 0x00;//CompositionTime[24bit][0]

	//AVC sequence header referent ISO-14496-15 AVC file format
	pBuf[5] = 1; 			//configurationVersion
	pBuf[6] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps[1]; 	// AVCProfileIndication
	pBuf[7] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps[2]; 	// profile_compatibility
	pBuf[8] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps[3]; 	// AVCLevelIndication
	pBuf[9] = 0x03 ; 		//& 0xFC; // lengthSizeMinusOne

	int offset = 10;

	if(offset+6+SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen
			+ SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen >= MAX_VIDEO_AUDIO_CONF_LEN)
	{
		printf("<RtmpGenVideoConf>ch %d data is too long!\n", nCh);
		return FALSE;
	}

	pBuf[offset] = 0x01; // & 0xE0; // numOfSequenceParameterSets
	offset += 1;
	pBuf[offset] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen>> 8;
	pBuf[offset+1] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen;
	offset += 2;

	memcpy(pBuf+ offset, SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps,
			SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen);
	offset += SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen;

	pBuf[offset] = 0x01;	//numOfPictureParameterSets
	offset += 1;
	pBuf[offset] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen>> 8;
	pBuf[offset+1] = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen;
	offset += 2;

	memcpy(pBuf + offset, SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps, SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen);
	offset += SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen;

	SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nVideoConfLen = offset;
	return TRUE;
}

Bool RtmpGenVDataPacket(unsigned char * pMove, unsigned int nBufLen, unsigned char byNalType,
		unsigned int nStartPos, unsigned int nEndPos,unsigned int uTimeStamp,int nCh)
{
	unsigned char byTmp[9];
	unsigned char byHeader[128];
	unsigned int nSize = nEndPos - nStartPos - 4;
	unsigned int nAllSize = nSize + 9;

	RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;

	if(RtmpGetBufNullSize(pRtmpBuf) < nAllSize + 8)
	{
		return FALSE;
	}

	//flv video tag
	byTmp[0] = (0x05 == byNalType) ? 0x17 : 0x27;//Frame Type[4 bit][1-keyframe 2-inter frame]	CodecID[4bit][7-AVC]
	byTmp[1] = 0x01;	//AVC Packet Type[8bit][1 - AVC NALU]
	byTmp[2] = 0x00;	//CompositionTime[24bit][0]
	byTmp[3] = 0x00;
	byTmp[4] = 0x00;

	//NALU Len
	byTmp[5] = (nSize >> 24) & 0xff;
	byTmp[6] = (nSize >> 16) & 0xff;
	byTmp[7] = (nSize >> 8) & 0xff;
	byTmp[8] = nSize & 0xff;

	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt  += nBufLen;
	memcpy(byHeader,(char *)&uTimeStamp, 4);
	memcpy(byHeader+4,(char *)&nAllSize, 4);
	memcpy(byHeader+8,(char *)byTmp, 9);

	//write data to the videobuf Ringbuf
	pthread_mutex_lock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);
	RtmpWrite(pRtmpBuf,(char *)byHeader,17);
	RtmpWrite(pRtmpBuf,(char *)pMove + nStartPos + 4,nSize);
	pthread_mutex_unlock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);

	return TRUE;
}

unsigned int PutRtmpVData(char *pBuf, unsigned int nBufLen, int nCh,unsigned int uTimeStamp)
{
	unsigned char *pMove = (unsigned char *)pBuf;
	unsigned int nStartPos = 0;
	unsigned int nEndPos = 0;
	unsigned char byNalType = 0;
	unsigned int nLeftLen = nBufLen;
	RTMP_BUF_S *pRtmpBuf;

	pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;

	SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt += nBufLen;


	if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
	{
		RtmpEmpty(pRtmpBuf);
		return nBufLen;
	}

	if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux)
	{
		RtmpEmpty(pRtmpBuf);
		return 0;
	}

	while(1)
	{
		//NAL
		if(FALSE == FindNal(pMove, nLeftLen, &byNalType, &nStartPos, &nEndPos))
		{
			break;
		}
		//printf("FindNal NalType %d Start %d End %d LeftLen %d\n", byNalType, nStartPos, nEndPos, nLeftLen);
		if(NULL == SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps || NULL == SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps)
		{
			break;
		}
		//sps
		if((0x07 == byNalType) && (FALSE == SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps))
		{
			memcpy(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pSps, pMove + nStartPos + 4, nEndPos - nStartPos - 4);
			SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen =  nEndPos - nStartPos -4;
			printf("ch %d Find Sps, Length %d\n", nCh, SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen);
		}

		//pps
		if((0x08 == byNalType) && (FALSE == SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps))
		{
			memcpy(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pPps, pMove + nStartPos + 4, nEndPos - nStartPos - 4);
			SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen = nEndPos - nStartPos -4;
			printf("ch %d Find Pps, Length %d\n", nCh, SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen);
		}

		//found sps pps
		if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps)
		{
			if((SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nSpsLen > 0) && (SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nPpsLen > 0))
			{
				RtmpGenVideoConf(nCh);
				SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps = TRUE;
			}
		}

		//get valid data
		if(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps)
		{
			if((byNalType != 0x07) && (byNalType != 0x08) && (byNalType != 0x09))
			{
				Bool bRet = RtmpGenVDataPacket(pMove,nBufLen,byNalType,nStartPos,nEndPos,uTimeStamp,nCh);
				if(FALSE==bRet)
				{
					return 0;
				}
			}
		}

		pMove += nEndPos;
		nLeftLen -= nEndPos;
	}

	return nBufLen;
}


int RtmpFindAudioConf(unsigned char * pData,unsigned int nEsLen,int nCh)
{
	struct adts_fixed_header fixed_hdr;
	struct aac_conf_header aac_conf;
	char *pBuf = NULL;

	aac_conf.bit2 = 2;// 2 channel

	memcpy(&fixed_hdr, pData, sizeof(fixed_hdr));
	aac_conf.profile = fixed_hdr.profile + 1;
	aac_conf.frequencyL = fixed_hdr.sampling_frequency_index;
	aac_conf.frequencyH = (fixed_hdr.sampling_frequency_index >> 1);
	aac_conf.reserved = 0;

	if(NULL == SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf)
		return -1;
	pBuf = SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf + RTMP_MAX_HEADER_SIZE;
	//flv video tag
	pBuf[0] = 0xAF;//SoundFormat[4bit][A-AAC] SoundRate[2bit][3-44k]
	//SoundSize[1bit][1-snd16Bit]SoundType[1bit][1-sndStereo]
	pBuf[1] = 0x00;//AACPacketType[8bit][0-AAC sequence header]

	memcpy(pBuf + 2, &aac_conf, sizeof(aac_conf));
	SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].nAudioConfLen = sizeof(aac_conf) + 2;

	return 0;
}

Bool RtmpGenADataPacket(char *pBuf, unsigned int nBufLen, int nCh,unsigned int uTimeStamp)
{
	unsigned char byTmp[2];
	unsigned char byHeader[128];
	unsigned int nOffSet = 0;
	struct adts_fixed_header afh;
	unsigned int nSize;
	RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;

	memcpy(&afh, pBuf, sizeof(afh));
	nOffSet = 7 + (afh.protection_absent ? 0 : 2);
	if(nBufLen <= nOffSet)
	{
		return FALSE;
	}
	nSize = nBufLen - nOffSet + 2;

	if(RtmpGetBufNullSize(pRtmpBuf) <= nSize + 8)
	{
		return FALSE;
	}
	//flv video tag
	byTmp[0] = 0xaf;//SoundFormat[4bit][A-AAC] SoundRate[2bit][3-44k]
	//SoundSize[1bit][1-snd16Bit]SoundType[1bit][1-sndStereo]
	byTmp[1] = 0x01;//AACPacketType[8bit][1-AAC raw]


	//SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt += nBufLen;
	memcpy(byHeader,(char *)&uTimeStamp, 4);
	memcpy(byHeader+4,(char *)&nSize, 4);
	memcpy(byHeader+8,(char *)byTmp, 2);

	pthread_mutex_lock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);
	RtmpWrite(pRtmpBuf,(char*)byHeader,10);
	RtmpWrite(pRtmpBuf,(char *)pBuf + nOffSet,nBufLen - nOffSet);
	pthread_mutex_unlock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);

	return TRUE;
}


unsigned int PutRtmpAData(char *pBuf, unsigned int nBufLen, int nCh,unsigned int uTimeStamp)
{
	RTMP_BUF_S *pRtmpBuf;
	pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;

	if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
	{
		RtmpEmpty(pRtmpBuf);
		return nBufLen;
	}

	//drop data
	if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux)
	{
		RtmpEmpty(pRtmpBuf);
		return 0;
	}

	//creat audio Config
	if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr)
	{
		int iRet = -1;
		iRet = RtmpFindAudioConf((unsigned char *)pBuf, nBufLen,nCh);
		if(iRet >= 0)
		{
			SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr = TRUE;
			printf("Find Audio Config Ok ch %d\n" , nCh);
		}
	}

	//get valid data,drop 7B Header
	if(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr)
	{
		Bool bRet = RtmpGenADataPacket(pBuf,nBufLen,nCh,uTimeStamp);
		if(FALSE==bRet)
		{
			return 0;
		}
	}

	return nBufLen;
}

int RtmpGenAndSendPacket(RTMP_GEN_PACKET_PARAMS_S *pRtmpParms,int nCh)
{
	RTMPPacket rtmp_pkt;
	RTMPPacket_Reset(&rtmp_pkt);
	RTMP *pRtmpHdl = SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock;

	rtmp_pkt.m_packetType = pRtmpParms->nPacketType;
	rtmp_pkt.m_nBodySize = pRtmpParms->nBodySize;
	rtmp_pkt.m_nTimeStamp = pRtmpParms->nTimeStamp;
	rtmp_pkt.m_nChannel = pRtmpParms->nCtrlChId;
	rtmp_pkt.m_headerType = RTMP_PACKET_SIZE_MEDIUM;//RTMP_PACKET_SIZE_LARGE;
	rtmp_pkt.m_nInfoField2 = pRtmpHdl->m_stream_id;//m_hRtmp->m_stream_id;
	rtmp_pkt.m_body = (char *)pRtmpParms->pBody;

	if(RTMP_IsConnected(pRtmpHdl))
	{
		return RTMP_SendPacket(pRtmpHdl, &rtmp_pkt, 0);
	}
	else
	{
		return 0;
	}
}

Bool ChangeRtmpChunkSize(int nCh)
{
	RTMP_GEN_PACKET_PARAMS_S struRtmpParams;
	char byBuf[RTMP_MAX_HEADER_SIZE + 4];
	AMF_EncodeInt32(byBuf+RTMP_MAX_HEADER_SIZE,byBuf+RTMP_MAX_HEADER_SIZE+4,RTMP_CHUNK_SIZE);
	struRtmpParams.pBody = byBuf+RTMP_MAX_HEADER_SIZE;
	struRtmpParams.nBodySize = 4;
	struRtmpParams.nPacketType = 0x01;
	struRtmpParams.nTimeStamp = 0;
	struRtmpParams.nCtrlChId = 2;
	int nRet = RtmpGenAndSendPacket(&struRtmpParams,nCh);
	if(!nRet)
	{
		return FALSE;
	}
	return TRUE;
}

int RTMP_Send_ChunkSize(int nCh)
{
	//change trans chunk size
	if(!SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendChunkInfo)
	{
		if(ChangeRtmpChunkSize(nCh))
		{
			SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock->m_outChunkSize = RTMP_CHUNK_SIZE;
		}
		else
		{
			return -1;
		}
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendChunkInfo = TRUE;
	}
	return 0;
}

int RTMP_Send_AVConf(int nCh)
{
	RTMP_GEN_PACKET_PARAMS_S struRtmpParams;

	if((TRUE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf)&&
			(TRUE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf))
	{
		return 0;
	}
	//send video conf
	if((FALSE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf)&&
			(SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nVideoConfLen>0))
	{
		struRtmpParams.nBodySize = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].nVideoConfLen;
		struRtmpParams.nCtrlChId = 0x04;
		struRtmpParams.nPacketType = 0x09;
		struRtmpParams.nTimeStamp = 0;
		struRtmpParams.pBody = SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].pVideoConf + RTMP_MAX_HEADER_SIZE;
		int nRet = RtmpGenAndSendPacket(&struRtmpParams,nCh);
		if(!nRet)
		{
			return -1;
		}

		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf = TRUE;
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nVideoSendTime = 0;
	}

	//send audio conf
	if((FALSE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf)&&
			(SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].nAudioConfLen>0))
	{
		struRtmpParams.nBodySize = SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].nAudioConfLen;
		struRtmpParams.nCtrlChId = 0x04;
		struRtmpParams.nPacketType = 0x08;
		struRtmpParams.nTimeStamp = 0;
		struRtmpParams.pBody = SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].pAudioConf + RTMP_MAX_HEADER_SIZE;
		int nRet = RtmpGenAndSendPacket(&struRtmpParams,nCh);
		if(!nRet)
		{
			return -1;
		}

		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf = TRUE;
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nAudioSendTime = 0;
	}

	if((FALSE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf)||
			(FALSE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf))
	{
		return -2;
	}

	return 0;
}

int RTMP_Send_AudioData(int nCh)
{
	RTMP_GEN_PACKET_PARAMS_S struRtmpParams;
	RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;
	unsigned int *pAudioTime = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nAudioSendTime;
	unsigned int nSendLen = 0;

	pthread_mutex_lock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);
	RtmpRead(pRtmpBuf,(char*)pAudioTime,4);
	RtmpRead(pRtmpBuf,(char*)&nSendLen,4);
	Bool nRdRet = RtmpRead(pRtmpBuf,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData+RTMP_MAX_HEADER_SIZE
			,nSendLen);
	pthread_mutex_unlock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);

	if(nRdRet)
	{
		struRtmpParams.nBodySize = nSendLen;
		struRtmpParams.nCtrlChId = 0x04;
		struRtmpParams.nPacketType = 0x08;
		struRtmpParams.nTimeStamp = *pAudioTime;
		struRtmpParams.pBody = SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData
			+RTMP_MAX_HEADER_SIZE;
		int nRet = RtmpGenAndSendPacket(&struRtmpParams,nCh);
		if(!nRet)
		{
			//printf("<>ch %d send audio data failed!\n", nCh);
			return -1;
		}
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutCnt += nSendLen;
	}
	else
	{
		RtmpEmpty(pRtmpBuf);
	}
	return 0;
}

int RTMP_Send_VideoData(int nCh)
{

	RTMP_GEN_PACKET_PARAMS_S struRtmpParams;
	RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;
	unsigned int *pVideoTime = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nVideoSendTime;
	unsigned int nSendLen = 0;


	pthread_mutex_lock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);
	RtmpRead(pRtmpBuf,(char*)pVideoTime,4);
	RtmpRead(pRtmpBuf,(char*)&nSendLen,4);

	Bool nRdRet = RtmpRead(pRtmpBuf,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData+RTMP_MAX_HEADER_SIZE
			,nSendLen);
	pthread_mutex_unlock(&SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].mutexDataLock);

	if(nRdRet)
	{
		struRtmpParams.nBodySize = nSendLen;
		struRtmpParams.nCtrlChId = 0x04;
		struRtmpParams.nPacketType = 0x09;
		struRtmpParams.nTimeStamp = *pVideoTime;
		struRtmpParams.pBody = SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].pRtmpSendData
			+ RTMP_MAX_HEADER_SIZE;
		int nRet = RtmpGenAndSendPacket(&struRtmpParams,nCh);
		if(!nRet)
		{
			printf("<>ch %d send video data failed!\n", nCh);
			return -1;
		}
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutCnt += nSendLen;
	}
	else
	{
		RtmpEmpty(pRtmpBuf);
	}
	return 0;
}


Bool RtmpCheckAndGetTime(int nCh)
{
	unsigned int *pVTimeStamp = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nVideoSendTime;
	unsigned int *pATimeStamp = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nAudioSendTime;

	RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;

	if(FALSE==RtmpGetTime(pATimeStamp,pRtmpBuf))
	{
		return FALSE;
	}

	pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;
	if(FALSE==RtmpGetTime(pVTimeStamp,pRtmpBuf))
	{
		return FALSE;
	}
	return TRUE;
}


int ConnectStatusCheck(int nCh)
{
	RTMP *pRtmpHdl = SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].RtmpSock;
	if(!(RTMP_IsConnected(pRtmpHdl)))
	{
		return -1;
	}
	return 0;

}

void reportErr(int nCh)
{
	SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut++;
	SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrTimeOut = RTMP_GetTime();
}

void *SPTS_RTMP_Process(void *pData)
{
	int iTreadId = *((int*)pData);
	int iRet = -1;
	int iMaxThreadCh = (MAX_RTMP_STREAM_CHANNEL+MAX_PROCESS_THREAD - 1)/MAX_PROCESS_THREAD;
	int nCh = 0;
	sleep(3);
	while(!SPTS_RTMP_Send_Ctrl.sendExitThr[iTreadId])
	{
		Bool bSleep = TRUE;
		for (nCh = iTreadId*iMaxThreadCh; nCh < MAX_RTMP_STREAM_CHANNEL && nCh < (iTreadId+1) * iMaxThreadCh; nCh++)
		{
			unsigned int *pVideoTime = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nVideoSendTime;
			unsigned int *pAudioTime = &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nAudioSendTime;
			if(!SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled)
			{
				RtmpEmpty(&SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio);
				RtmpEmpty(&SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo);
				continue;
			}


			if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate)
			{
				reportErr(nCh);
				SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate = FALSE;
			}

			if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateV < MONITOR_IN_BW)
			{
				//SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn++;
				SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps = FALSE;
				//SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr = FALSE;
				reportErr(nCh);
				continue;
			}

			if ((SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut > 0) || (SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn > 0))
			{
				if(SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut > 0)
				{
					unsigned int uTime = RTMP_GetTime();
					if(uTime - SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrTimeOut < 8000)
					{
						continue;
					}
					SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrTimeOut = uTime;
				}

				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nTryCnt++;

				RtmpDisConnect(nCh);

				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendChunkInfo = FALSE;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = FALSE;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendVideoConf = FALSE;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendAudioConf = FALSE;

				Bool bRet = FALSE;
				bRet = RtmpConnect(nCh);
				if(!bRet)
				{
					printf("[debug] rtmp retry ch[%d]start failed!\n", nCh);
					continue;
				}

				SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut = 0;
				SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn = 0;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nTryCnt = 0;

			}

			if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
			{
				iRet =  ConnectStatusCheck(nCh);
				if(0 > iRet)
				{
					reportErr(nCh);
					continue;
				}

				//send Chunk Size
				iRet = RTMP_Send_ChunkSize(nCh);
				if(0 > iRet)
				{
					reportErr(nCh);
					continue;
				}

				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = TRUE;
				//send video conf
				iRet = RTMP_Send_AVConf(nCh);
				if(-1 == iRet)
				{
					reportErr(nCh);
					continue;
				}
				else if(-2 == iRet)
				{
					continue;
				}
#if 0
				RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;
				if(RtmpGetDataSize(pRtmpBuf) > 8)
				{
					iRet = RTMP_Send_AudioData(nCh);
					if(0 > iRet)
					{
						reportErr(nCh);
						continue;
					}
					printf("ch %d RTMP_Send_AudioData\n", nCh);
				}

				pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;
				if(RtmpGetDataSize(pRtmpBuf) > 8)
				{
					iRet = RTMP_Send_VideoData(nCh);
					if(0 > iRet)
					{
						reportErr(nCh);
						continue;
					}
					printf("ch %d RTMP_Send_VideoData\n", nCh);
				}
#endif
#if 0
				RTMP_BUF_S *pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio;
				if(FALSE==RtmpGetTime(pAudioTime,pRtmpBuf))
				{
					pAudioTime = NULL;
				}
				pRtmpBuf = &SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo;
				if(FALSE==RtmpGetTime(pVideoTime,pRtmpBuf))
				{
					pVideoTime = NULL;
				}

				if((NULL == pVideoTime)&&(NULL == pAudioTime)) continue;


				if((NULL == pVideoTime) ||(!bIsLess(*pVideoTime,*pAudioTime)))
				{
					iRet = RTMP_Send_AudioData(nCh);
					if(0 > iRet)
					{
						reportErr(nCh);
						continue;
					}
				}
				else
				{
					iRet = RTMP_Send_VideoData(nCh);
					if(0 > iRet)
					{
						reportErr(nCh);
						continue;
					}
				}
#endif
				if(FALSE==RtmpCheckAndGetTime(nCh))
				{
					continue;
				}
#if 1
				if(bIsLess(*pVideoTime,*pAudioTime))
				{
					iRet = RTMP_Send_VideoData(nCh);
					//printf("send video data %u\n",*pVideoTime);
					if(0 > iRet)
					{
						reportErr(nCh);
						printf("send video data error!!!!!!!!!!!\n");
						continue;
					}
				}
				else
				{
					//printf("send audio data %u\n",*pAudioTime);
					iRet = RTMP_Send_AudioData(nCh);
					if(0 > iRet)
					{
						reportErr(nCh);
						printf("send audio data error!!!!!!!\n");
						continue;
					}
				}

#endif
			}
			else
			{
				Bool bRet = FALSE;


				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bSendChunkInfo = FALSE;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = FALSE;
				bRet = RtmpConnect(nCh);
				if(!bRet)
				{
					printf("<RtmpStart 0>rtmp start failed!\n");
					reportErr(nCh);
					continue;
				}
			}
			bSleep = FALSE;
		}

		if (bSleep)
		{
			usleep(200000);
		}
	}

	for (nCh = iTreadId*iMaxThreadCh; nCh < MAX_RTMP_STREAM_CHANNEL && nCh < (iTreadId+1) * iMaxThreadCh; nCh++)
	{
		RtmpDisConnect(nCh);
		RtmpEmpty(&SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].struAudio);
		RtmpEmpty(&SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].struVideo);
		SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux = FALSE;
	}

	SPTS_RTMP_Send_Ctrl.sendExitThr[iTreadId] = TRUE;
	SPTS_RTMP_Send_Ctrl.sendIsThrStopDone[iTreadId] = TRUE;

	return NULL;
}

Bool RtmpStart()
{

	int i = 0;
	pthread_t thr[8];

	for (i=0; i<MAX_PROCESS_THREAD; i++)
	{
		SPTS_RTMP_Send_Ctrl.sendExitThr[i] = FALSE;
		SPTS_RTMP_Send_Ctrl.sendIsThrStopDone[i] = FALSE;

		int nErr = pthread_create(&SPTS_RTMP_Send_Ctrl.sendThrHndl[i],NULL,SPTS_RTMP_Process,&gCh[i]);
		if (nErr != 0)
		{
			SPTS_RTMP_Send_Ctrl.sendExitThr[i] = TRUE;
			SPTS_RTMP_Send_Ctrl.sendIsThrStopDone[i] = TRUE;

			printf ("\r\n Thread id %d TS Send thread create failed...\n", i);
			return FALSE;
		}
		printf("O_o!*** [SPTS_RTMP_Process] Thread id %d create done!\n", i);
	}

	return TRUE;
}

Bool RtmpStop()
{
	int i = 0;

	SPTS_RTMP_Send_Ctrl.MonitorExitThr = TRUE;
	while(!SPTS_RTMP_Send_Ctrl.MonitorIsThrStopDone)
	{
		usleep(500000);
	}

	for (i=0; i<MAX_PROCESS_THREAD; i++)
	{

		printf("<RtmpStop>OSA_thrDelete: id %d\n",i);
		SPTS_RTMP_Send_Ctrl.sendExitThr[i] = TRUE;

		//if thread is still running,wait until it stop!!!
		while(!SPTS_RTMP_Send_Ctrl.sendIsThrStopDone[i])
		{
			usleep(500000);
		}
	}
	return TRUE;
}

static unsigned int  getMSec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (tv.tv_sec*1000 + tv.tv_usec/1000);
}

void *SPTS_RTMP_Monitor()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned int uLastTime = getMSec();

	int nCh = 0;
	sleep(3);
	while(!SPTS_RTMP_Send_Ctrl.MonitorExitThr)
	{
		unsigned int uCurTime = getMSec();
		int nDuration = uCurTime - uLastTime;
		float fSecond = nDuration/1000.0;
		uLastTime = uCurTime;
		for(nCh=0;nCh<MAX_RTMP_STREAM_CHANNEL;nCh++)
		{
			if(FALSE==SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled)
			{
				SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn = 0;
				SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut = 0;
				continue;
			}
			/*
			   if(FALSE == SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bPutDataMux)
			   {
			   SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn = 0;
			   SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut = 0;
			   continue;
			   }*/
			if(0 < nDuration)
			{
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateVRingbuf =  SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt*8/fSecond;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateV = SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt*8/fSecond;
				SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutRate = SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutCnt*8/fSecond;
			}

			SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInCnt = 0;
			SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRingbufCnt = 0;
			SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nOutCnt = 0;
			/*
			   if(SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].nInRateV < MONITOR_IN_BW)
			   {
			   SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn++;
			   SPTS_RTMP_Send_Ctrl.sRtmpVideoInfo[nCh].bFindSpsPps = FALSE;
			//SPTS_RTMP_Send_Ctrl.sRtmpAudioInfo[nCh].bFindAacHdr = FALSE;
			}*/
		}
		sleep(4); //sleep 2s
	}

	SPTS_RTMP_Send_Ctrl.MonitorExitThr = TRUE;
	SPTS_RTMP_Send_Ctrl.MonitorIsThrStopDone = TRUE;

	return NULL;
}

void signal_handle(int sig)
{
	printf("******************SIGPIPE <%d>!", sig);
}

int  RtmpComponentInit()
{
	/*Init the memory and params*/
	int nErr = 0;


	if(RTMP_BHLibVersion() != 20140306)
	{
		printf("[debug]=>the older librtmp  date %d\n", RTMP_BHLibVersion());
		return OSA_EFAIL;
	}
	printf("[debug]=>the newer  librtmp date %d\n", RTMP_BHLibVersion());

	if(RtmpInit()==OSA_EFAIL)
	{
		printf("Rtmp init failsed!!!!!\n");
		return OSA_EFAIL;
	}
	signal(SIGPIPE, signal_handle);

	SPTS_RTMP_Send_Ctrl.MonitorExitThr = FALSE;
	SPTS_RTMP_Send_Ctrl.MonitorIsThrStopDone = FALSE;


	//创建处理线程
	printf ("\r\n SPTS_RTMP_Monitor thread create start...\n");
	//nErr = OSA_thrCreate(&SPTS_RTMP_Send_Ctrl.MonitorThrHndl, SPTS_RTMP_Monitor, OSA_THR_PRI_DEFAULT, (10*1024), NULL);
	nErr = pthread_create(&SPTS_RTMP_Send_Ctrl.MonitorThrHndl,NULL,SPTS_RTMP_Monitor, NULL);
	if (nErr != 0)
	{
		SPTS_RTMP_Send_Ctrl.MonitorExitThr = TRUE;
		SPTS_RTMP_Send_Ctrl.MonitorIsThrStopDone = TRUE;

		printf ("\r\n SPTS_RTMP_Monitor thread create failed...\n");
		return OSA_EFAIL;
	}
	printf ("\r\n SPTS_RTMP_Monitor thread create sussess...\n");
	return OSA_SOK;
}

int  RtmpComponentDeInit()
{
	//add by_xh 2014.2.19

	RtmpComponentStop();
	//add end
	sleep(1);

	RtmpDeInit();
	return OSA_SOK;
}


//Start RTMP
int  RtmpComponentStart()
{
	int nCh = 0;
	for(nCh=0;nCh<MAX_RTMP_STREAM_CHANNEL;nCh++)
	{
		if(FALSE==SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bConnect)
		{
			SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrIn = 0;
			SPTS_RTMP_Send_Ctrl.sRtmpErrInfo[nCh].nErrOut = 0;

		}
	}
	RtmpStart();
	return 0;
}

//stop RTMP
int  RtmpComponentStop()
{
	RtmpStop();
	return 0;
}


Bool Component_updateRtmp()
{
	int nCh = 0;
	char byStrUrl[128];

	for(nCh=0; nCh<MAX_RTMP_STREAM_CHANNEL; nCh++)
	{
		memset(byStrUrl,0, sizeof(byStrUrl));
		Component_GetRtmp_Inf(nCh, byStrUrl, &SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[nCh].bEnabled);

		//printf("%d ch byStrUrl:%s\n",nCh,byStrUrl);
		if(strcmp(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl,byStrUrl))
		{
			//memcpy(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl,byStrUrl,strlen(byStrUrl));
			strcpy(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl,byStrUrl);
			SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].bUrlUpdate = TRUE;

			printf("ch %d update url:%s\n",nCh,SPTS_RTMP_Send_Ctrl.sRtmpSendSock[nCh].pStreamUrl);
		}
	}
	return TRUE;
}


Void    Rtmp_printStaticInfo(char *print_buf)
{
	Int32 i;

	sprintf(print_buf+strlen(print_buf),  " \r\n *** Rtmp Out Statistics *** \r\n");

	for(i=0; i<MAX_RTMP_STREAM_CHANNEL; i++)
	{
		if(SPTS_RTMP_Send_Ctrl.sRtmpSendSock[i].pStreamUrl!=NULL)
		{
			sprintf(print_buf+strlen(print_buf),
					"\r\n "
					"ch %02d "
					"En %d connect %d Retry %d "
					"inRateV %dkbps "
					"inRingbuf %dkbps "
					"outRate %dkbps "
					"\r\nurl=  %s"
					,i
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].bEnabled
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].bPutDataMux
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nTryCnt
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nInRateV/1000
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nInRateVRingbuf/1000
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nOutRate/1000
					,SPTS_RTMP_Send_Ctrl.sRtmpSendSock[i].pStreamUrl);
		}
		else
		{
			sprintf(print_buf+strlen(print_buf),
					"\r\n "
					"ch %02d "
					"En %d connect %d Retry %d "
					"inRateV %dkbps "
					"inRingbuf %dkbps "
					"outRate %dkbps "
					,i
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].bEnabled
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].bPutDataMux
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nTryCnt
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nInRateV/1000
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nInRateVRingbuf/1000
					,SPTS_RTMP_Send_Ctrl.sRtmpDataCtrl[i].nOutRate	/1000);
		}
	}

	sprintf(print_buf+strlen(print_buf), "\r\n\r\n");
}


#if 1
int main()
{
	char test[1024];
	RtmpComponentInit();
	RtmpComponentStart();
	Conponent_UdpRcv_Init();
	while(1)
	{
		memset(test,0,1024);
		Rtmp_printStaticInfo(test);
		printf("[debug]===>-------------------------------------------------%s\n",test);
		sleep(5);
	}
	Conponent_UdpRcv_DeInit();
	RtmpComponentDeInit();
	return 0;
}

#endif

