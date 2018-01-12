#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>

#include "message.h"
#include "message_queue.h"
#include "main.h"

#define	MSG_KEY_PATH	"/tmp"



void QueueLock()
{
    pthread_mutex_lock(&lock);
}

void QueueUnlock()
{
    pthread_mutex_unlock(&lock);
}

/******************************************************************************/
/** message map queue mechanism initialize
 *
 * @return none
 *
 * @note
 *         this function is only called at main() before any task was created.
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

    *******************************************************************************/

int QueueInit(void)
{
    int msgIdx;
    
	pthread_mutex_init (&lock, NULL);

    for(msgIdx = 0; msgIdx < MSG_TYPE_END; msgIdx++)
    {
        Queuelist[msgIdx].queue = NULL;
    }

    int queueIdx;

    for(queueIdx = 0; queueIdx < QUEUE_TYPE_END; queueIdx++)
    {
        m_MsgKey[queueIdx] = -1;
        m_MsgQueueID[queueIdx] = -1;;
        QueueCreate(MSG_KEY_PATH,queueIdx);
    }
    return 1;
}

int QueueCreate(char* pathName,int queueIdx)
{
    if(queueIdx < 0 && queueIdx > QUEUE_TYPE_END)
    {
        return -1;
    }
    int nQueueID = -1;
    m_MsgKey[queueIdx] = ftok(pathName, queueIdx);
    long ret;
    if (m_MsgKey[queueIdx] < 0 )
    {
        return -1;
    }else{
        nQueueID = msgget(m_MsgKey[queueIdx], IPC_CREAT|IPC_EXCL|0666);
    }

    if(-1 == nQueueID)
    {
        nQueueID = msgget(m_MsgKey[queueIdx], IPC_CREAT|0666);
        if(-1 == msgctl(nQueueID, IPC_RMID, NULL) )
        {
            DEBUG("MessageQueue::QueueCreate msgctl IPC_RMID nQueueID = [%d]  failed:%s",nQueueID,strerror(errno));
        }

        nQueueID = 0;
        nQueueID = msgget(m_MsgKey[queueIdx], IPC_CREAT|0666);
    }

    m_MsgQueueID[queueIdx] = nQueueID;
    return 0;
}


QueueHandle_t QueueGetId(QueueHandle_t queue_id)
{
    if(queue_id>0 && queue_id <QUEUE_TYPE_END)
    {
        return m_MsgQueueID[queue_id];
    }else{

        return -1;
    }
}

/******************************************************************************/
/** malloc a new queue node and insert it to the QueueList_t
 *
 * @param[in] listx the list to insert , the listx : Queuelist[]->queue
 * @param[in] queue new queue node to insert to the QueueList_t
 *
 * @return
 *     - < 0 the queue inserted failed
 *     - > 0 the queue inserted success
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

    *******************************************************************************/
int LinkListInsert(struct QueueList_t * listx, QueueHandle_t queue_id)
{
    int exist = 0;

    struct QueueLinkNode * cur_node = NULL, * new_node = NULL;

    if(NULL == listx)
    {
        return -1;
    }

    cur_node = listx->queue;
    while(NULL != cur_node)
    {
        new_node = cur_node;
        if(queue_id == cur_node->queue_id)
        {
            exist = 1;
            break;
        }

        cur_node = cur_node->next_queue;
    }

    if(!exist)
    {
        cur_node = (struct QueueLinkNode *)malloc(sizeof(struct QueueLinkNode));

        if(NULL == cur_node)
        {
            return -1;
        }

        cur_node->queue_id  = queue_id;
        cur_node->next_queue = NULL;

        new_node->next_queue = cur_node;
    }

    return 1;
}

/******************************************************************************/
/** delete a specified queue node from the QueueList
 *
 * @param[in] listx the list to insert , the listx : Queuelist[]->queue
 * @param[in] queue new queue node to insert to the QueueList_t
 *
 * @return
 *     - < 0 the queue isn't present
 *     - > 0 the queue present and delete success
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

    *******************************************************************************/
int LinkListDelete(struct QueueList_t * listx, QueueHandle_t queue_id)
{
    int exist = 0;

    struct QueueLinkNode * cur_node = NULL, * prv_node = NULL;

    if((NULL == listx)||(0 == queue_id))
    {
        return -1;
    }

    cur_node = listx->queue;

    while( NULL != cur_node )
    {
        if(queue_id == cur_node->queue_id)
        {
            exist = 1;

            break;
        }

        prv_node = cur_node;

        cur_node = cur_node->next_queue;
    }

    if(exist)
    {
        if(NULL == cur_node->next_queue)
        {
            cur_node->next_queue = NULL;
        }

        if(cur_node == listx->queue)
        {
            listx->queue = cur_node->next_queue;
        }
        else
        {
            prv_node->next_queue = cur_node->next_queue;
        }

        if(cur_node != NULL)
        {
            free(cur_node);
        }

        return 1;
    }

    return -1;
}

/******************************************************************************/
/** to construct the map relation between message and queue.
 *
 * @param[in] msg   the message to map to the queue < queue, msg >
 * @param[in] queue the queue to map the msg < queue, msg >
 *
 * @return
 *     - < 0 the queue inserted failled , and the map is canceled.
 *     - > 0 the queue is inserted success, and the map relation is ok
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

    *******************************************************************************/
int MessageQueueRegister(int msg, QueueHandle_t queue_idx)
{
    DEBUG("START");
    struct QueueLinkNode * new_node;

    if(msg < MSG_TYPE_END)
    {
        QueueLock();

        if(NULL == Queuelist[msg].queue)
        {

            new_node =(struct QueueLinkNode *)malloc(sizeof(struct QueueLinkNode));

            if(NULL == new_node)
            {
                QueueUnlock();
                return -1;
            }

            new_node->queue_id = m_MsgQueueID[queue_idx];
            new_node->next_queue = NULL;

            Queuelist[msg].queue = new_node;
        }
        else
        {
            LinkListInsert(&Queuelist[msg], m_MsgQueueID[queue_idx]);
        }

        QueueUnlock();
        ////////////////////////////////////////////////////////////////////////////////
#if 0
        printf("===============================================================================\n");
        int idx;
        for(idx = 0;idx < MSG_TYPE_END;++idx)
        {
            if(NULL != Queuelist[idx].queue)
            {
                struct  QueueLinkNode* root = Queuelist[idx].queue;
                printf("[MSG] = %d MessageQueueRegister QUEUE AS :\n", idx);
                int cnt = 0;
                while(( NULL != root )&&(0 != root->queue_id))
                {
                    printf("cnt =%d,queue_id =%d\r\n",cnt,root->queue_id );
                    root = root->next_queue; /* next queue */
                    cnt++;
                }
                printf("\r\n");

            }
        }
        printf("===============================================================================\n");
#endif
        ////////////////////////////////////////////////////////////////////////////////
        DEBUG("END");
        return 1;
    }
    return -1;
}

/******************************************************************************/
/** when dellete a task , then delete the message and queue map relation
 *
 * @param[in] msg   the message to unmap with the spec-queue
 * @param[in] queue the queue to unmap with the msg
 *
 * @return
 *     - < 0 the <queue, msg> unmap failed, and the unmap operation is failed
 *     - > 0 the <queue, msg> unmap success, and the unmap operation is OK
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

    *******************************************************************************/
int MessageQueueUnregister(int msg, QueueHandle_t queue_idx)
{
    /* you can delete msg-queue-map directly, but make sure the queue be empty before delete task !!! */

    DEBUG("START");
    int v;

    QueueLock();

    /* must examine if the queue is empty,code is andded later */

    v = LinkListDelete(&Queuelist[msg], m_MsgQueueID[queue_idx]);

    QueueUnlock();

    ////////////////////////////////////////////////////////////////////////////////
#if 0
    printf("===============================================================================\n");
    int idx;
    for(idx = 0;idx < MSG_TYPE_END;++idx)
    {
        if(NULL != Queuelist[idx].queue)
        {
            struct  QueueLinkNode* root = Queuelist[idx].queue;
            printf("[MSG] = %d MessageQueueRegister QUEUE AS :\n", idx);
            int cnt = 0;
            while(( NULL != root )&&(0 != root->queue_id))
            {
                printf("cnt =%d,queue_id =%d\r\n",cnt,root->queue_id );
                root = root->next_queue; /* next queue */
                cnt++;
            }
            printf("\r\n");

        }
    }
    printf("===============================================================================\n");
#endif
    ////////////////////////////////////////////////////////////////////////////////
    DEBUG("END");
    return v;
}


/******************************************************************************/
/** message send function
 *
 * @param[in] send_msg: the message to send
 * @param[in] para_pointer: the local data buffer address
 * @param[in] para_length: the length of local data to send
 * @param[in] pointer_or_value: MESSAGE_IS_POINTER  or MESSAGE_IS_VALUE
 *
 * @return
 *     - < 0 the message send failed
 *     - > 0 the message send success
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

*******************************************************************************/
int MessageSend(int send_msg, void * para_pointer, uint32_t para_length, uint32_t pointer_or_value)
{
    MSG_T   message;
    BaseType_t   send_result;
    struct QueueLinkNode * queue_root = NULL;

    assert(para_pointer);

    if(send_msg > MSG_TYPE_END)
    {
        return -1;
    }

    queue_root = Queuelist[send_msg].queue;/* get the message's root queue node */

    while(( NULL != queue_root )&&(0 != queue_root->queue_id))
    {
        if(para_length > MESSAGE_MAX_LENGTH)
        {
            return -1;
        }

        /* package message data */
        if( MESSAGE_IS_POINTER == pointer_or_value )/* deliver pointer */
        {
            message.parameter.pointer =(char * )malloc(para_length * sizeof(char));

            if(NULL == message.parameter.pointer)
            {
                return -1;
            }

            memcpy((uint8_t *)message.parameter.pointer, (uint8_t *)para_pointer, para_length);/* make sure 'para_length >= 4' */

            message.msg_id = send_msg;
			message.msg_type = MESSAGE_IS_POINTER|para_length;

        }
        else/* deliver value */
        {
            message.parameter.value = *(uint32_t *)para_pointer;

            message.msg_id = send_msg;
			message.msg_type = MESSAGE_IS_VALUE|para_length;
        }

        /* dispacth message data */
        send_result = msgsnd(queue_root->queue_id, &message, para_length+sizeof(long), 0);
        if( -1 == send_result )
        {
            DEBUG("msgsnd ERROR %s", strerror(errno));
            return -1;
        }
        queue_root = queue_root->next_queue; /* next queue */
    }

    return 1;
}


/******************************************************************************/
/** message recv function
 *
 * @param[in] queue: the message to send
 * @param[in] received_msg: the receive message ID will be copied to received_msg
 * @param[in] received_msg_data: the receive message data will be copied to
 *                               received_msg_data
 *
 * @return
 *     - < 0 the message send failed
 *     - > 0 the message send success
 * @note
 *
 *//*
 * History        :
 * 1.Date         : 2017/11/28
 *   Modification : Created function

 *******************************************************************************/
int MessageRecv(QueueHandle_t queue_idx, int * const received_msg, void * received_msg_data)
{
    MSG_T  message;
    int msg;
    uint8_t buf[MESSAGE_MAX_LENGTH] = {0};
    struct QueueLinkNode * queue_root = NULL;

    assert(received_msg);
    assert(received_msg_data);

    memset(&message, 0x00, sizeof(MSG_T));
    int nResult = msgrcv(m_MsgQueueID[queue_idx],&buf,MESSAGE_MAX_LENGTH,0,0);
    if( -1 == nResult)
    {
        DEBUG("msgrcv ERROR %s", strerror(errno));
    }

    uint8_t * p;
    memcpy(&message,(uint8_t*)buf,sizeof(MSG_T));

    /* get the received message structure */
    if( MESSAGE_IS_VALUE == (message.msg_type & 0xFFFF0000) )/* delivered value */
    {
        *received_msg = message.msg_id; /* msg id */
        *(uint32_t *)received_msg_data = message.parameter.value /* msg data */;
    }
    else /* delivered pointer */
    {
        *received_msg = message.msg_id;

        if( NULL != message.parameter.pointer )/* need to check the pointer's validation */
        {
            if( (message.msg_type & 0x0000FFFF) <= MESSAGE_MAX_LENGTH)
            {
                memcpy((uint8_t *)received_msg_data, (uint8_t *)message.parameter.pointer, (message.msg_type & 0x0000FFFF) );

                free( message.parameter.pointer );/* release the pointer */
            }
            else
            {
                free( message.parameter.pointer );/* release the pointer */
                return -1;
            }
        }
    }

    return 1;
}

/*========================================================================================*/
