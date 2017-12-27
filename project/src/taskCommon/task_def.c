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
int xTaskCreate(pthread_t *thread, TaskFunction_t start_routine, void *arg, const char* pthreadname, int stacksz)
{
    if (start_routine == NULL)
    {
        DEBUG("commcreatethread start_routine == NULL");
        return -1;
    }

    int retval;
    int stacksize = stacksz;
    pthread_attr_t attr;
    if (stacksize == 0)
        stacksize = 32*1024;
    retval = pthread_attr_init(&attr);
    if (retval != 0)
    {
        DEBUG("commcreatethread call pthread_attr_init retval = %d errno=%d", retval, errno);
        return -1;
    }
    retval = pthread_attr_setstacksize(&attr, stacksize);
    if (retval != 0)
    {
        DEBUG("commcreatethread call pthread_attr_setstacksize retval = %d errno=%d", retval, errno);
        return -1;
    }

    int cbparam = sizeof(thread_param_info);
    int cbthreadname = (pthreadname != NULL) ? (strlen(pthreadname) + 1) : 0;
    cbparam += cbthreadname;
    thread_param_info* param = (thread_param_info*)malloc(cbparam);
    if (param == NULL)
    {
        DEBUG("commcreatethread call malloc == NULL");
        return -1;
    }
    param->start_routine = start_routine;
    param->arg = arg;
    if (pthreadname != NULL && pthreadname[0] != '\0')
    {
        strcpy(param->pthreadname, pthreadname);
    }
    else
    {
        param->pthreadname[0] = '\0';
    }

    pthread_t thread_id;
    retval = pthread_create(&thread_id, &attr, (*start_routine), (void*)param);
    if (retval != 0)
    {
        DEBUG( "commcreatethread call pthread_create retval=%d errno=%d", retval, errno);
        return -1;
    }

    pthread_attr_destroy(&attr);
    if (thread != NULL)
    {
        *thread = thread_id;
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

        DEBUG("xCreate task %s, u32StackSize %d", t->ps8Name, (int)t->u32StackSize);

        x = xTaskCreate(&task_handles[i],
                        t->pTask,
                        t->pParameters,
                        t->ps8Name,
                        t->u32StackSize
                        );

        if (x != 0) {
            DEBUG(":task_def_init failed");
        } else {
            DEBUG(":task_def_init succeeded");
        }
    }
}
