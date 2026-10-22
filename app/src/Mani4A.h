#pragma once
#include <array>
#include "zauxdll2.h"
#include <iostream>
#include "PalletRobot.h"
#include <map>


namespace Palletizing
{
	class Mani4A
	{
	public:
		explicit Mani4A() = default;
		~Mani4A();
	public:
		/// @brief 打开控制器
		/// @return 错误码
		int open();
		/// @brief 关闭控制器
		/// @return 错误码
		int close();
		/// @brief 终止所有运动
		/// @return 错误码
		int terminate();
		/// @brief 查询机械手状态
		/// @param device 设备ID
		/// @param state 状态位(bit0: moving bit1: error bit2: warning)
		/// @return 错误码
		int queryState(int device, uint32_t& state);
		/// @brief 查询机械手关节坐标
		/// @param device 设备ID
		/// @param joints 关节坐标(rad/mm)
		/// @return 错误码
		int queryJoints(int device, std::array<double, 4>& joints);
		/// @brief 查询机械手笛卡尔坐标
		/// @param device 设备ID
		/// @param xyzrz 笛卡尔坐标(rad/mm)
		/// @return 错误码
		int queryPose(int device, std::array<double, 4>& xyzrz);
		/// @brief 机械手Jog运动
		/// @param device 设备ID
		/// @param joint 关节索引
		/// @param vel 速度[0:100]
		/// @return 错误码
		int jogJ(int device, int joint, double vel);
		/// @brief 机械手moveJ运动
		/// @param device 设备ID
		/// @param joints 关节坐标
		/// @param vel 速度[0:100]
		/// @return 错误码
		int moveJ(int device, const std::array<double, 4>& joints, double vel);
		/// @brief 机械手moveP运动
		/// @param device 设备ID
		/// @param xyzrz 笛卡尔坐标
		/// @param vel 速度[0:100]
		/// @return 错误码
		int moveP(int device, const std::array<double, 4>& xyzrz, double vel);
		/// @brief 机械手夹爪控制
		/// @param device 设备ID
		/// @param gripper 夹爪索引
		/// @param on 目标状态
		/// @return 错误码
		int gripper(int device, int gripper, bool on);
		/// @brief 机械手夹爪状态查询
		/// @param device 设备ID
		/// @param gripper 夹爪索引
		/// @param on 夹爪状态
		/// @return 错误码
		int isGripperOn(int device, int gripper, bool& on);
		/// @brief 查询上料准备状态
		/// @param device 设备ID
		/// @param ready 上料准备状态
		/// @return 错误码
		int isLoadReady(int device, bool& ready);
		/// @brief 查询下料清空状态
		/// @param device 设备ID
		/// @param cleared 下料清空状态
		/// @return 错误码
		int isUnloadCleared(int device, bool& cleared);

	private:

		double R2D(double r);
		double D2R(double d);

		ZMC_HANDLE _hZMC = nullptr;

		std::shared_ptr<PalletRobot> palletRobot_;

		std::map<int, std::array<int, 4>> axisMap;

		std::map<int, int> gripperOutMap;
		std::map<int, int> gripperInMap;
	};
}