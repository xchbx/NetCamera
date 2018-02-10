#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <errno.h>
#include "main.h"
#include "task_def.h"
#include "elog.h"

#define LOG_TAG    "TASKDEF"

/****************************************************************************
 * Types.
 ****************************************************************************/
typedef void* (*TaskFunction_t)( void * );
typedef pthread_t TaskHandle_t;

typedef struct tasks_list_ {
    TaskFunction_t      pTask;
    const char*         ps8Name;
    void*               pParameters;
    uint32_t            u32StackSize;
} tasks_list_t;


typedef struct _thread_param_info_
{
    TaskFunction_t start_routine;
    void *arg;
    char pthreadname[32];
}thread_param_info;

/****************************************************************************
 * Forward Declarations.
 ****************************************************************************/
extern void* TaskSender(void * arg);
extern void* TaskRecver(void * arg);


static const tasks_list_t tasks_list[] = {
	{ TaskSender,    "TASK_SENDER",     NULL,   128*1024},
    { TaskRecver,    "TASK_RECVER",     NULL,   128*1024},
};

#define tasks_list_count  (sizeof(tasks_list) / sizeof(tasks_list_t))

static TaskHandle_t     task_handles[tasks_list_count];

/****************************************************************************
 * Private Functions
 ****************************************************************************/
int xTaskCreate(pthread_t *pThreadId, TaskFunction_t fTaskFunc, void *pParameters, const char* pThreadName, int s32StackSize)
{
    if (fTaskFunc == NULL)
    {
        return -1;
    }

    int s32Ret;
    int s32LocalSackSize = s32StackSize;
    pthread_attr_t attr;
    if (s32LocalSackSize == 0)
        s32LocalSackSize = 32*1024;
    s32Ret = pthread_attr_init(&attr);
    if (s32Ret != 0)
    {
        log_e("pthread_attr_init s32Ret = %d errno=%d", s32Ret, errno);
        return -1;
    }

    s32Ret = pthread_attr_setstacksize(&attr, s32LocalSackSize);
    if (s32Ret != 0)
    {
        log_e("pthread_attr_setstacksize s32Ret = %d errno=%d", s32Ret, errno);
        return -1;
    }

    int s32ParamLen = sizeof(thread_param_info);
    int s32ThreadNameLen = (pThreadName != NULL) ? (strlen(pThreadName) + 1) : 0;
    s32ParamLen += s32ThreadNameLen;
    thread_param_info* pThreadParamInfo = (thread_param_info*)malloc(s32ParamLen);
    if (pThreadParamInfo == NULL)
    {
        log_e("malloc == NULL");
        return -1;
    }
    pThreadParamInfo->start_routine = fTaskFunc;
    pThreadParamInfo->arg = pParameters;
    if (pThreadName != NULL && pThreadName[0] != '\0')
    {
        strcpy(pThreadParamInfo->pthreadname, pThreadName);
    }
    else
    {
        pThreadParamInfo->pthreadname[0] = '\0';
    }

    pthread_t thread_id;
    s32Ret = pthread_create(&thread_id, &attr, (*fTaskFunc), (void*)pThreadParamInfo);
    if (s32Ret != 0)
    {
        log_e( "pthread_create s32Ret=%d errno=%d", s32Ret, errno);
        return -1;
    }

    pthread_attr_destroy(&attr);
    if (pThreadId != NULL)
    {
        *pThreadId = thread_id;
    }
    return 0;
}

/****************************************************************************
 * Public API
 ****************************************************************************/

void task_def_init(void)
{
    uint16_t            i;
    int32_t             x;
    const tasks_list_t  *t;

    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];

        log_i("xCreate task %s, u32StackSize %d", t->ps8Name, (int)t->u32StackSize);

        x = xTaskCreate(&task_handles[i],
                        t->pTask,
                        t->pParameters,
                        t->ps8Name,
                        t->u32StackSize
                        );

        if (x != 0) {
            log_e(":task_def_init failed");
        } else {
            log_i(":task_def_init succeeded");
        }
    }
}
