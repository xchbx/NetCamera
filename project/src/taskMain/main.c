/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */

/* hal includes */
#include "task_def.h"
#include "message.h"
#include "message_queue.h"
#include "elog.h"

#define LOG_TAG    "TASKMAINFUNCTION"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static int ElogInit(void) {
    /* close printf buffer */
    setbuf(stdout, NULL);
    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* start EasyLogger */
    elog_start();

    return 0;
}
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
    ElogInit();

	/** creat task */
	task_def_init();

    /* Start the scheduler. */
    while(1)
    {
        usleep(3*1000*1000);
	log_a("Hello EasyLogger!");
	log_e("Hello EasyLogger!");
	log_w("Hello EasyLogger!");
	log_i("Hello EasyLogger!");
	log_d("Hello EasyLogger!");
	log_v("Hello EasyLogger!");   
    }
}

