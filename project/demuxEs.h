#ifndef _DEMUX_ES_H
#define _DEMUX_ES_H


#define	GetStartFlag(x)			((x[1] >> 6) & 0x01)
#define	GetAdapCtrl(x)			((x[3] >> 4) & 0x03)
#define	GetAdapLen(x)			(x[4])
#define	GetPid(x)				(((x[1]&0x1f)<<8) | x[2])



unsigned int GetEsPayload(unsigned char *pInData, unsigned int *uPts, unsigned char * pOutData, unsigned int nOutLen);

#endif
