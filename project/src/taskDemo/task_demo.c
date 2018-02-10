
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "message_queue.h"
#include "main.h"
#include "elog.h"

#define LOG_TAG    "TASKDEMO"
#if 0
void* TaskDemo(void * arg)
{
    int u32Cnt = 0;
    while(1)
    {
        DEBUG("--------------------------TaskDemo u32Cnt = %d",u32Cnt++);
        usleep(10*1000*1000);
    }
}
#endif

void* TaskSender(void * arg)
{    
    int msg;
    int dat;
    
    int msg_data[3] = {0};
        
    while(1)
    {   
        /* transmit value example */
        dat = 0x11;
        msg = MSG_TEST_0;
        MessageSend(msg, &dat, 4, MESSAGE_IS_VALUE);
        
        dat = 0x22;
        msg = MSG_TEST_1;
        MessageSend(msg, &dat, 4, MESSAGE_IS_VALUE);
        
        dat = 0x44;
        msg = MSG_TEST_3;
        MessageSend(msg, &dat, 4, MESSAGE_IS_VALUE);
        
        /* transmit poniter example */
        /* do not fill the message length field, because the MessageSend function fill the length field auto */
        msg = MSG_TEST_2;           /* message code */        
        msg_data[0] = 0xFEDCBA98;   /* message data */
        msg_data[1] = 0x89ABCDEF;   /* message data */
        msg_data[2] = 0x12345678;   /* message data */
        
        MessageSend(msg, msg_data, sizeof(msg_data), MESSAGE_IS_POINTER);
        
        usleep(1*1000*1000);
    }
}

void* TaskRecver(void * arg)
{
    int msg;
    int msg_data[32];
    
   MessageQueueRegister(MSG_TEST_0,QUEUE_TYPE_WAKE0);
   MessageQueueRegister(MSG_TEST_1,QUEUE_TYPE_WAKE0);
   MessageQueueRegister(MSG_TEST_2,QUEUE_TYPE_WAKE0);
   MessageQueueRegister(MSG_TEST_3,QUEUE_TYPE_WAKE0);
   MessageQueueRegister(MSG_TEST_4,QUEUE_TYPE_WAKE0);
   MessageQueueRegister(MSG_TEST_5,QUEUE_TYPE_WAKE0);
    
    while(1)
    {
        if(-1 != MessageRecv(QUEUE_TYPE_WAKE0, &msg, &msg_data) )
        {
            switch(msg)
            {
                case MSG_TEST_0:
                case MSG_TEST_1:
                case MSG_TEST_2:
                case MSG_TEST_3:
                case MSG_TEST_4:
                case MSG_TEST_5:
                case MSG_TEST_6:
                {
                	int * p;
            		p = (int *)msg_data;
            		log_d("-----------------recv msg = %d \r\n", msg);
            		log_e("TaskRecver<==============recv message=[%p] %x,%x,%x,%x\r\n",p, p[0],p[1],p[2],p[3]);
                    break;
                }

            }
            
            memset((uint8_t *)&msg_data, '\0', sizeof(msg_data));
        }
    }
}
