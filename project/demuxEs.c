#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "demuxEs.h"

#define STREAM_ID_MAP			0xBC
#define STREAM_ID_1			0xBD
#define STREAM_ID_PADDING		0xBE
#define STREAM_ID_2			0xBF
#define STREAM_ID_ECM			0xF0
#define STREAM_ID_EMM			0xF1
#define STREAM_ID_DSMCC			0xF2
#define STREAM_ID_ISO13522		0xF3
#define STREAM_ID_DIR			0xFF




int ParsePes(char *pInData, unsigned int nDataLen, unsigned int *pPts, unsigned short *usPesHLen)
{
    unsigned short usPesHeadLen = 9;

    if(NULL == pInData || NULL == pPts || NULL == usPesHLen)
        return -1;

    *usPesHLen = 0;

    if(nDataLen < 10)
    {
        printf("Pes len error Pes Len %d", nDataLen);
        return -1;
    }

    if((pInData[0] != 0x00) || (pInData[1] != 0x00) ||
            (pInData[2] != 0x01))
    {
        printf("Pes Head Error, 0x%02x%02x%02x\n", pInData[0], pInData[1], pInData[2]);
        return -2;
    }

    if((pInData[3] == STREAM_ID_MAP) ||
            (pInData[3] == STREAM_ID_1) ||
            (pInData[3] == STREAM_ID_PADDING) ||
            (pInData[3] == STREAM_ID_2) ||
            (pInData[3] == STREAM_ID_ECM) ||
            (pInData[3] == STREAM_ID_EMM) ||
            (pInData[3] == STREAM_ID_DSMCC) ||
            (pInData[3] == STREAM_ID_ISO13522) ||
            (pInData[3] == STREAM_ID_DIR))
    {
        printf("Pes Stream ID error. stream_id 0x%02x", pInData[3]);
        return -3;
    }

    if(0x00 != ((pInData[6] >> 4) & 0x03))
    {
        return -1;
    }

    if(!(pInData[6] & 0x80))
    {
        printf("pInData[6] & 0x80 = %d\n", pInData[6] & 0x80);
        return -4;
    }

    usPesHeadLen += pInData[8];

    *usPesHLen = usPesHeadLen;

    char byPTSFlag = pInData[7] >> 6;
    if(!(byPTSFlag & 0x02))
    {
        //printf("No DTS exist %x[%x]", byPTSFlag, pInData[7]);
        return 1;	//-5;
    }

    unsigned int nPtsPos = 9;

    if(nDataLen < 14)
    {
        printf("Pes Len 2 error，Pes Len %d\n", nDataLen);
        return -1;
    }

    if((byPTSFlag & 0x03) == 0x03) //把DTS值当PTS值使用
    {
        nPtsPos += 5;
        if(nDataLen < 19)
        {
            printf("Pes len 3 error，Pes Len %d\n", nDataLen);
            return -1;
        }
    }

    *pPts = ((pInData[nPtsPos + 4] & 0xfe) >> 1) + (pInData[nPtsPos + 3] << 7) +
        ((pInData[nPtsPos + 2] & 0xfe) << 14) + (pInData[nPtsPos + 1] << 22) +
        ((pInData[nPtsPos] & 0x06) << 29);

    return 0;
}

unsigned int GetEsPayload(unsigned char *pInData, unsigned int *pPts, unsigned char * pOutData, unsigned int nOutLen)
{
    unsigned int nPayloadLen = 0;
    int iRet = 0;

    char byAdapCtrl = GetAdapCtrl(pInData);
    unsigned int nPos = 4;
    unsigned short usPesHLen = 0;

    unsigned int nAdapLen = 0;
    if(byAdapCtrl & 0x02)
    {
        nAdapLen = GetAdapLen(pInData);
        nPos += (nAdapLen + 1);
    }

    if(nPos < 188)
    {
        nPayloadLen = 188 - nPos;
    }

    if(nOutLen < nPayloadLen)
    {
        return 0;
    }

    if(GetStartFlag(pInData))
    {
        unsigned int uPts = 0;
        iRet = ParsePes(pInData + nPos,nPayloadLen, pPts, &usPesHLen);
        if(iRet < 0)
        {
            printf("ParsePes Error\n");
            return iRet;
        }
    }

    //printf("GetEsPayload : pts %u,nPos %d,pesHLen %d,nPayloadLen %d ,nAdapLen %u \n",*pPts,nPos,usPesHLen,nPayloadLen,nAdapLen);
    if(nPayloadLen - usPesHLen > 0)
    {
        memcpy(pOutData, pInData + nPos + usPesHLen, nPayloadLen - usPesHLen);
    }
    return nPayloadLen - usPesHLen;
}
