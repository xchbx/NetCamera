#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "message.h"
#include <pthread.h>

typedef unsigned int QueueHandle_t;
typedef void * QueueSetHandle_t;
typedef void * QueueSetMemberHandle_t;

struct QueueLinkNode
{
    QueueHandle_t queue_id;
    struct QueueLinkNode * next_queue;
};

struct QueueList_t
{
    struct QueueLinkNode * queue;
};

#define MESSAGE_IS_VALUE     (1 << 30)
#define MESSAGE_IS_POINTER   (0 << 30)
#define MESSAGE_MAX_LENGTH   (128    )

/****************************************************************************
 * Private define
 ****************************************************************************/
key_t m_MsgKey[QUEUE_TYPE_END];
int m_MsgQueueID[QUEUE_TYPE_END];
pthread_mutex_t lock;
struct QueueList_t Queuelist[MSG_TYPE_END];

/****************************************************************************
 * Private Functions
 ****************************************************************************/
int QueueInit(void);
int QueueDeInit(void);
int QueueCreate(char* pathName,int queueIdx);
int LinkListInsert(struct QueueList_t * listx, QueueHandle_t queue_id);
int LinkListDelete(struct QueueList_t * listx, QueueHandle_t queue_id);
void QueueLock();
void QueueUnlock();
/****************************************************************************
 * Public API
 ****************************************************************************/
int MessageSend(int send_msg, void * para_pointer, uint32_t para_length, uint32_t pointer_or_value);
int MessageRecv(QueueHandle_t queue_idx, int * const received_msg, void * received_msg_data);
int MessageQueueRegister(int msg, QueueHandle_t queue_idx);
int MessageQueueUnregister(int msg, QueueHandle_t queue_idx);

#ifdef __cplusplus
}
#endif

#endif
