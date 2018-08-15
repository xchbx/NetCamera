#ifndef __IVS_INTERFACE_H__
#define __IVS_INTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#include <imp/imp_common.h>
#include <imp/imp_ivs.h>

/**
 * IVS功能枚举定义
 */
typedef enum {
	IVS_MOVE_DETECT		= 0,			/**< 移动侦测 */
	IVS_LINE_DETECT,					/**< 越线检测 */
	IVS_PERM_DETECT,					/**< 周界防范 */
	IVS_FIGURE_DETECT,					/**< 人形检测 */
	IVS_SKIPFIGURE_DETECT,                    /**<      */
	IVS_FACE_DETECT,                    /**< 人脸检测 */
	IVS_SKIPFACE_DETECT,                    /**<      */
	IVS_CAR_DETECT,						/**< 车辆检测 */
	IVS_SMOKE_DETECT,					/**< 烟检测 */
	IVS_SHADE_DETECT,					/**< 遮挡检测 */
	IVS_HEAD_DETECT,                    /**< 人数统计 */
	IVS_LPR_DETECT,                     /**< 车牌检测 */
	IVS_DYNAMIC_TRACKER_DETECT,         /**< 动态运动追踪检测 */
	IVS_MOTION_TRACKER_DETECT,          /**< 静态运动追踪检测 */
	IVS_FAKE_DETECT,					/**< 虚假检测 */
    	IVS_OPT_FLOW_DETECT,                /**<光流检测*/
    	IVS_CLOUD_TRACKER_DETECT,           /**<云端多目标跟踪检测*/
	IVS_PLATEREC_DETECT,                /**<车牌识别*/
	IVS_FALL_DETECT_DETECT,           /**<跌倒检测*/
	IVS_INGENIC_END,					/**< 君正内部最大功能号 */
	IVS_FUNC_MAX	= 1024,				/**< 最大功能枚举值 */
} IMPIVSFuncEnum;

/**
 * @fn IMPIVSInterface *FaceInterfaceInit(void *param)
 *
 * 人脸检测接口初始化结构体
 *
 * @param[in] 人脸检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回人脸检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *FaceInterfaceInit(void *param);

/**
 * @fn void FaceInterfaceExit(IMPIVSInterface *inf)
 *
 * 人脸检测接口去初始化结构体
 *
 * @inf[in] 人脸检测接口结构体指针句柄
 *
 * @attention 无。
 */
void FaceInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *FigureInterfaceInit(void *param)
 *
 * 人形检测接口初始化结构体
 *
 * @param[in] 人形检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回人形检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *FigureInterfaceInit(void *param);

/**
 * @fn void FigureInterfaceExit(IMPIVSInterface *inf)
 *
 * 人形检测接口去初始化结构体
 *
 * @inf[in] 人形检测接口结构体指针句柄
 *
 * @attention 无。
 */
void FigureInterfaceExit(IMPIVSInterface *inf);

IMPIVSInterface *OptFlowInterfaceInit(void* param);

void OptFlowInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *MoveInterfaceInit(void *param)
 *
 * 移动侦测接口初始化结构体
 *
 * @param[in] 移动侦测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回移动侦测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *MoveInterfaceInit(void *param);

/**
 * @fn void MoveInterfaceExit(IMPIVSInterface *inf)
 *
 * 移动侦测接口去初始化结构体
 *
 * @inf[in] 移动侦测接口结构体指针句柄
 *
 * @attention 无。
 */
void MoveInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *LineInterfaceInit(void *param)
 *
 * 越线检测接口初始化结构体
 *
 * @param[in] 越线检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回越线检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *LineInterfaceInit(void *param);

/**
 * @fn void LineInterfaceExit(IMPIVSInterface *inf)
 *
 * 越线检测接口去初始化结构体
 *
 * @inf[in] 越线检测接口结构体指针句柄
 *
 * @attention 无。
 */
void LineInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *PermInterfaceInit(void *param)
 *
 * 周界防范接口初始化结构体
 *
 * @param[in] 周界防范初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回周界防范接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *PermInterfaceInit(void *param);

/**
 * @fn void PermInterfaceExit(IMPIVSInterface *inf)
 *
 * 周界防范接口去初始化结构体
 *
 * @inf[in] 周界防范接口结构体指针句柄
 *
 * @attention 无。
 */
void PermInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *SmokeInterfaceInit(void *param)
 *
 * 烟雾检测接口初始化结构体
 *
 * @param[in] 烟雾检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回烟雾检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *SmokeInterfaceInit(void* param);

/**
 * @fn void SmokeInterfaceExit(IMPIVSInterface *inf)
 *
 * 烟雾检测接口去初始化结构体
 *
 * @inf[in] 烟雾检测接口结构体指针句柄
 *
 * @attention 无。
 */
void SmokeInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *HeadInterfaceInit(void *param)
 *
 * 人头检测接口初始化结构体
 *
 * @param[in] 人头检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回人头检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *HeadInterfaceInit(void *param);

/**
 * @fn void HeadInterfaceExit(IMPIVSInterface *inf)
 *
 * 人头检测接口去初始化结构体
 *
 * @inf[in] 人头检测接口结构体指针句柄
 *
 * @attention 无。
 */
void HeadInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *ShadeInterfaceInit(void *param)
 *
 * 遮挡检测接口初始化结构体
 *
 * @param[in] 遮挡检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回遮挡检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *ShadeInterfaceInit(void *param);

/**
 * @fn void ShadeInterfaceExit(IMPIVSInterface *inf)
 *
 * 遮挡检测接口去初始化结构体
 *
 * @inf[in] 遮挡检测接口结构体指针句柄
 *
 * @attention 无。
 */
void ShadeInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *LprInterfaceInit(void *param)
 *
 * 车牌检测接口初始化结构体
 *
 * @param[in] 车牌检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回车牌检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *LprInterfaceInit(void *param);

/**
 * @fn void LprInterfaceExit(IMPIVSInterface *inf)
 *
 * 车牌检测接口去初始化结构体
 *
 * @inf[in] 车牌检测接口结构体指针句柄
 *
 * @attention 无。
 */
void LprInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *TrackerInterfaceInit(void* param)
 *
 * 人形追踪检测接口初始化结构体
 *
 * @param[in] 人形追踪检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回人形追踪检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *TrackerInterfaceInit(void* param);

/**
 * @fn void TrackerInterfaceExit(IMPIVSInterface *inf)
 *
 * 人形追踪检测接口去初始化结构体
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 *
 * @attention 无。
 */
void TrackerInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *Motion_TrackerInterfaceInit(void* param)
 *
 * 静态运动追踪检测接口初始化结构体
 *
 * @param[in] 静态运动追踪检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回静态运动追踪检测接口结构体指针句柄
 *
 * @attention 适用于全景摄像机。
 */
IMPIVSInterface *Motion_TrackerInterfaceInit(void* param);

/**
 * @fn void Motion_TrackerInterfaceExit(IMPIVSInterface *inf)
 *
 * 静态运动追踪检测接口去初始化结构体
 *
 * @inf[in] 静态运动追踪检测接口结构体指针句柄
 *
 * @attention 适用于全景摄像机。
 */
void Motion_TrackerInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *Dynamic_TrackerInterfaceInit(void* param)
 *
 * 动态运动追踪检测接口初始化结构体
 *
 * @param[in] 动态运动追踪检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回动态运动追踪检测接口结构体指针句柄
 *
 * @attention 适用于适用于摇头机。
 */
IMPIVSInterface *Dynamic_TrackerInterfaceInit(void* param);

/**
 * @fn void Dynamic_TrackerInterfaceExit(IMPIVSInterface *inf)
 *
 * 动态运动追踪检测接口去初始化结构体
 *
 * @inf[in] 动态运动追踪检测接口结构体指针句柄
 *
 * @attention 适用于适用于摇头机。
 */
void Dynamic_TrackerInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *Cloud_TrackerInterfaceInit(void* param)
 *
 * 人形追踪检测接口初始化结构体
 *
 * @param[in] 运动追踪检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回运动追踪检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *Cloud_TrackerInterfaceInit(void* param);

/**
 * @fn void Cloud_TrackerInterfaceExit(IMPIVSInterface *inf)
 *
 * 人形追踪检测接口去初始化结构体
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 *
 * @attention 无。
 */
void Cloud_TrackerInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn int Cloud_TrackerEncodeInterfaceInit(IMPIVSInterface *inf)
 *
 * 人形追踪检测目标数据编码接口初始化结构体
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 *
 * @retval 非0 失败
 * @retval 0 成功
 *
 * @attention 无。
 */
int Cloud_TrackerEncodeInterfaceInit(IMPIVSInterface *inf);

/**
 * @fn int Cloud_TrackerEncodeInterfaceProcess(IMPIVSInterface *inf, void* param, void* output)
 *
 * 人形追踪检测目标数据编码接口处理目标数据
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 * @param[in] 人形追踪检测目标数据编码初始化参数
 * @output[out] 人形追踪检测目标数据编码结果
 *
 * @retval 非0 失败
 * @retval 0 成功
 *
 * @attention 无。
 */
int Cloud_TrackerEncodeInterfaceProcess(IMPIVSInterface *inf, void* param, void* output);

/**
 * @fn void Cloud_TrackerInterfaceExit(IMPIVSInterface *inf)
 *
 * 人形追踪检测目标数据编码接口释放中间资源
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 *
 * @retval 非0 失败
 * @retval 0 成功
 *
 * @attention 无。
 */
int Cloud_TrackerEncodeInterfaceRelease(IMPIVSInterface *inf);

/**
 * @fn int Cloud_TrackerEncodeInterfaceExit(IMPIVSInterface *inf)
 *
 * 人形追踪检测目标数据编码接口去初始化结构体
 *
 * @inf[in] 人形追踪检测接口结构体指针句柄
 *
 * @retval 非0 失败
 * @retval 0 成功
 *
 * @attention 无。
 */
int Cloud_TrackerEncodeInterfaceExit(IMPIVSInterface *inf);

IMPIVSInterface *SkipFaceInterfaceInit(void *param);

/**
 * @fn void SkipFaceInterfaceExit(IMPIVSInterface *inf)
 *
 *
 *
 * @inf[in]
 *
 * @attention
 */
void SkipFaceInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *PlateRecInterfaceInit(void *param)
 *
 * 车牌识别接口初始化结构体
 *
 * @inf[in] 车牌识别接口初始化参数
 *
 * @retval NULL 失败
 *
 * @retval 非NULL 初始化成功，返回车牌识别接口结构体句柄
 *
 * @attention 无
 */

IMPIVSInterface *PlateRecInterfaceInit(void *param);

/**
 * @fn void PlateRecInterfaceExit(IMPIVSInterface *inf)
 *
 * 车牌识别去初始化结构体句柄
 *
 * @inf[in] 车牌识别接口结构体句柄
 *
 * @attention 无
 */
void PlateRecInterfaceExit(IMPIVSInterface *inf);
/**
 * @fn IMPIVSInterface *PlateRecInterfaceInit(void *param)
 *
 *             
 *
 * @inf[in]            
 *
 * @retval NULL   
 *
 * @retval  NULL                    
 *
 * @attention  
 */

IMPIVSInterface *SkipFigureInterfaceInit(void *param);

/**
 * @fn void SkipFigureInterfaceExit(IMPIVSInterface *inf)
 *
 *
 *
 * @inf[in]
 *
 * @attention
 */
void SkipFigureInterfaceExit(IMPIVSInterface *inf);

/**
 * @fn IMPIVSInterface *Fall_DetectInterfaceInit
 *
 * 跌倒检测接口初始化结构体
 *
 * @param[in] 跌倒检测初始化参数
 *
 * @retval NULL 失败
 * @retval 非NULL 初始化成功，返回跌倒检测接口结构体指针句柄
 *
 * @attention 无。
 */
IMPIVSInterface *Fall_DetectInterfaceInit(void* param);

/**
 * @fn Fall_DetectInterfaceExit(IMPIVSInterface *inf)
 *
 * 跌倒检测接口去初始化结构体
 *
 * @inf[in] 跌倒检测接口结构体指针句柄
 *
 * @attention 无。
 */
void Fall_DetectInterfaceExit(IMPIVSInterface *inf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __IVS_INTERFACE_H__ */
