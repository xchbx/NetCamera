#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


/* part_1: SDK tasks configure infomation, please don't modify */
#define TASK_STACK_SIZE_DFT				(32*1024)

/*****************************************************************************
 * API Functions
 *****************************************************************************/

void task_def_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_DEF_H__ */

