/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */

/* hal includes */
#include "task_def.h"
#include "message.h"
#include "message_queue.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
//static void vTestTask( void *pvParameters );

/* Create the log control block for freertos module.
 * The initialization of the log is in the sys_init.c.
 * Please refer to the log dev guide under /doc folder for more details.
 */

int main(void)
{
	/** hardware init */

	/** message map queue mechanism init */
    QueueInit();

	/** creat task */
	task_def_init();

    /* Start the scheduler. */
    while(1)
    {
        usleep(3*1000*1000);   
    }
}

