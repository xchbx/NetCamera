#ifndef __IVS_INF_FACE_H__
#define __IVS_INF_FACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include "ivs_common.h"

/*
 * 人脸检测算法的输入结构体
 */
typedef struct {
  /* face cfg*/                  	
  int reserved;	                      /**< 预留信息 */
  int minFrameFaceGap;                /**< 人脸检测最小时间间隔 */
  int minFrameMoveNum;	              /**< 同一目标连续移动时间 */
  float OverlapThreshold;             /**< 物体移动重叠度阈值，用于评估物体运动激烈程度*/

  /*move cfg*/
  int sense; 	                      /**< 高级移动侦测的灵敏度，范围为0-4 */
  int minHeight;                      /**< 高级移动侦测物体的最小高度 */
  int minWidth;                       /**< 高级移动侦测物体的最小宽度 */ 
  IVSRect* rois;                      /**< 高级移动侦测待检测的区域信息 */
  int cntRoi;            	      /**< 高级移动侦测待检测区域的数量 */

  IVSFrameInfo frameInfo;             /**< 帧信息 */
} face_param_input_t;


/*
 * 人脸检测算法的输出结构体
 */
typedef struct {
  int count;		/**< 检测到的矩形框目标个数 */
  IVSRect* rects;	/**< 检测到的目标矩形框 */
  int* isFace;        /**< 检测到的目标score */
  int64_t timeStamp; /**< 时间戳 */
} face_param_output_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __IVS_INF_FACE_H__ */
