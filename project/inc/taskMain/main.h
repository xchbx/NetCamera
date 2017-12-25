/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define DEBUG(format,...)  do{ printf("[FuncName:%s],[LineNum:%u],[happy]===>"format" \r\n", \
        __func__, __LINE__,##__VA_ARGS__);}while(0)

/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Mediatek *****END OF FILE****/
