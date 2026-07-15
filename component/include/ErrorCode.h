#pragma once

/*****************************设备错误*****************************/
#define ERROR_DEVICE_OFFSET				(10000)

/*仿真器连接失败*/
#define ERROR_SIMULATOR_CONNECT_FAILED	(ERROR_DEVICE_OFFSET + 1)

/*机器人连接失败*/
#define ERROR_ROBOT_CONNECT_FAILED		(ERROR_DEVICE_OFFSET + 2)

/*机器人命令失败*/
#define ERROR_ROBOT_CMD_FAILED			(ERROR_DEVICE_OFFSET + 3)

/*相机连接失败*/
#define ERROR_CAMERA_CONNECT_FAILED		(ERROR_DEVICE_OFFSET + 4)

/*****************************************************************/

/*****************************规划错误*****************************/
#define ERROR_PLAN_OFFSET												(20000)

/*拍照区域未能包含所有的目标点位*/
#define ERROR_NOT_ALL_TARGET_PTS_INCLUDE								(ERROR_PLAN_OFFSET + 1)

/*拍照区域未包含任何的目标点位*/
#define ERROR_NO_TARGET_PTS_INCLUDE										(ERROR_PLAN_OFFSET + 2)

/*用户初定位拍照点不存在*/
#define ERROR_USER_INIT_LOCATE_CAPTURE_POSE_NOT_FOUND					(ERROR_PLAN_OFFSET + 3)

/*存在未匹配的点*/
#define ERROR_UNMATCHED_POINT_EXIST										(ERROR_PLAN_OFFSET + 4)

/*焊缝未图层通用属性*/
#define ERROR_LAYER_GENERAL_UNSET										(ERROR_PLAN_OFFSET + 5)

/*焊缝未配置工艺图层*/
#define ERROR_SEAM_LAYER_UNSET											(ERROR_PLAN_OFFSET + 6)

/*未定义拍照模板点*/
#define ERROR_SCANNER_TEMPLATE_NOT_DEFINED								(ERROR_PLAN_OFFSET + 7)

/*未定义示教模板点*/
#define ERROR_TEACH_TEMPLATE_NOT_DEFINED								(ERROR_PLAN_OFFSET + 8)

/*拍照模板未包含在拍照区域中*/
#define ERROR_SCANNER_TEMPLATE_NOT_FOUND_IN_CUSTOMER_SCANNER_REGION		(ERROR_PLAN_OFFSET + 9)

/*未定义的焊接点位置计算策略*/
#define ERROR_UNDEFINED_WELD_POINT_CALCULAITON_STRATEGY					(ERROR_PLAN_OFFSET + 10)
/*****************************************************************/