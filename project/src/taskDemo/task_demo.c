
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "message_queue.h"
#include "main.h"

void* TaskDemo(void * arg)
{
    int u32Cnt = 0;
    while(1)
    {
        DEBUG("--------------------------TaskDemo u32Cnt = %d",u32Cnt++);
        usleep(10*1000*1000);
    }
}

