#include "Mani4A.h"
#include <thread>
using namespace Palletizing;

#define ERROR_ZMC_OFFSET								(0x2000)
#define ERROR_ZMC_NOT_OPEN								(ERROR_ZMC_OFFSET + 1)
#define ERROR_ZMC_INVALID_INPUT							(ERROR_ZMC_OFFSET + 2)
#define ERROR_ZMC_ERROR_GCODE_TYPE_NOT_SUPORT			(ERROR_ZMC_OFFSET + 3)
#define ERROR_ZMC_INVALID_AXISID						(ERROR_ZMC_OFFSET + 4)

#define AXIS_COUNT (4)
#define LOAD_PORT (10)
#define UNLOAD_PORT (11)

#define CHECK_CMD_ZMC(func, rt)		do  \
									{	\
										if (rt)   \
										{		\
											printf("error func: %s, rtc: %d, line: %d\n", #func, rt, __LINE__); \
											return rt; \
										}   \
									} while (0)

Mani4A::~Mani4A()
{
	this->close();
}

int Mani4A::open()
{
	palletRobot_ = std::make_shared<PalletRobot>();
	palletRobot_->initParameters(685, 1100);

	axisMap.insert(std::make_pair(0, std::array<int, 4>{0, 1, 2, 3}));
	//axisMap.insert(std::make_pair(1, std::array<int, 4>{4, 5, 6, 7}));

	gripperOutMap.insert(std::make_pair(1, 4));//1号夹爪out
	gripperOutMap.insert(std::make_pair(2, 5));//2号夹爪out
	gripperOutMap.insert(std::make_pair(3, 6));//3号夹爪out

	gripperInMap.insert(std::make_pair(1, 7));//1号夹爪in
	gripperInMap.insert(std::make_pair(2, 8));//2号夹爪in
	gripperInMap.insert(std::make_pair(3, 9));//3号夹爪in

	//return 0;


	int rt = ZAux_OpenEth((char*)("192.168.1.11"), &_hZMC);
	CHECK_CMD_ZMC(ZAux_OpenEth, rt);
	rt = ZAux_SetTimeOut(_hZMC, 5000);
	CHECK_CMD_ZMC(ZAux_SetTimeOut, rt);
	//全轴使能
	for (int i = 0; i < AXIS_COUNT; i++)
	{
		rt = ZAux_Direct_SetAxisEnable(_hZMC, i, 1);
		CHECK_CMD_ZMC(ZAux_Direct_SetAxisEnable, rt);
	}

	ZAux_Direct_SetUnits(_hZMC, 0, 416.670f);
	ZAux_Direct_SetUnits(_hZMC, 1, 4250.0f);
	ZAux_Direct_SetUnits(_hZMC, 2, 3361.1f);
	ZAux_Direct_SetUnits(_hZMC, 3, 1388.89);

	float value;
	/*ZAux_Direct_GetMpos(_hZMC, 0, &value);
	ZAux_Direct_SetMpos(_hZMC, 0, value - 0);*/

	/*ZAux_Direct_GetMpos(_hZMC, 1, &value);
	ZAux_Direct_SetMpos(_hZMC, 1, value + 256.689);
	ZAux_Direct_SetDpos(_hZMC, 1, value + 256.689);

	ZAux_Direct_GetMpos(_hZMC, 2, &value);
	ZAux_Direct_SetMpos(_hZMC, 2, value - 1358.205);
	ZAux_Direct_SetDpos(_hZMC, 2, value - 1358.205);*/

	

	return rt;
}

int Mani4A::close()
{
	//全轴去使能
	for (int i = 0; i < AXIS_COUNT; i++)
	{
		ZAux_Direct_SetAxisEnable(_hZMC, i, 0);
	}
	int rt = ZAux_Close(_hZMC);
	CHECK_CMD_ZMC(ZAux_Close, rt);
	_hZMC = nullptr;
	return 0;
}

int Mani4A::terminate()
{
	int rt = ZAux_Direct_Rapidstop(_hZMC, 2);
	CHECK_CMD_ZMC(ZAux_Direct_Rapidstop, rt);
	return rt;
}

int Mani4A::queryState(int device, uint32_t& state)
{
	int axis = (device) ? 1 : 4;
	int idle = 0;
	int rt = ZAux_Direct_GetIfIdle(_hZMC, axis, &idle);
	state = 1;
	if (idle == -1)
		state = 0;
	return 0;
}

int Mani4A::queryJoints(int device, std::array<double, 4>& joints)
{
	if (device > 1 || device < 0)
		return -1;
	std::array<int, 4> aixs = axisMap.find(device)->second;
	for (int i = 0; i < 4; i++)
	{
		float pos = 0.0;
		ZAux_Direct_GetMpos(_hZMC, aixs[i], &pos);
		if (i == 0)
		{
			//pos += 1053;
			pos += 1370;
		}
			
		if (i == 1)
		{
			//pos += 256.689;
			pos -= 94.886;
			pos = pos * 3.14 / 180;
		}
			
		if (i == 2)
		{
			//pos += 1358.205;
			pos += 86.374;
			pos = pos * 3.14 / 180;
		}
			
		if (i == 3)
			pos = pos * 3.14 / 180;
		joints[i] = (double)pos;
	}
	return 0;
}

int Mani4A::queryPose(int device, std::array<double, 4>& xyzrz)
{
	if (device > 1 || device < 0)
		return -1;
	std::array<double, 4> joints;
	queryJoints(device, joints);
	Eigen::Vector4d angles;
	angles << joints[0], (joints[1]), (joints[2]), (joints[3]);
	
	Eigen::Vector4d ret = palletRobot_->forward(angles);
	xyzrz[0] = ret[0] + 322;
	xyzrz[1] = ret[1];
	//xyzrz[2] = ret[2] + 1053;
	//xyzrz[2] = ret[2] + 789;
	xyzrz[2] = ret[2];
	xyzrz[3] = ret[3];

	std::cout << ret[0] << "," << ret[1] << "," << ret[2] << "," << ret[3];
	return 0;
}

int Mani4A::jogJ(int device, int joint, double vel)
{
	if (joint < 0 || joint >= AXIS_COUNT)
		return ERROR_ZMC_INVALID_AXISID;

	int rt = ZAux_Direct_SetSpeed(_hZMC, joint, fabs(vel));
	CHECK_CMD_ZMC(ZAux_Direct_SetSpeed, rt);

	int dir = 1;
	if (vel < 0)
		dir = -1;
	rt = ZAux_Direct_Single_Vmove(_hZMC, joint, dir);
	CHECK_CMD_ZMC(ZAux_Direct_Single_Vmove, rt);
	return rt;
}

int Mani4A::moveJ(int device, const std::array<double, 4>& joints, double vel)
{
	if (device > 1 || device < 0)
		return -1;
	int rt = ZAux_Direct_SetSpeed(_hZMC, 0, fabs(vel));
	CHECK_CMD_ZMC(ZAux_Direct_SetSpeed, rt);
	std::array<float, 4> joints_;
	std::transform(joints.begin(), joints.end(), joints_.begin(),
		[](double d) { return static_cast<float>(d); });
	
	/*joints_.data()[0] -= 1053;
	joints_.data()[1] -= 256.689;
	joints_.data()[2] -= 1358.205;*/

	joints_.data()[0] -= 1370;
	joints_.data()[1] += 94.886;
	joints_.data()[2] -= 86.374;

	rt = ZAux_Direct_MoveAbs(_hZMC, 4, axisMap.find(device)->second.data(), joints_.data());

	int ret1 = 0, ret2 = 0, ret3 = 0, ret4 = 0;
	while (true)
	{
		ZAux_Direct_GetIfIdle(_hZMC, 0, &ret1);
		ZAux_Direct_GetIfIdle(_hZMC, 1, &ret2);
		ZAux_Direct_GetIfIdle(_hZMC, 2, &ret3);
		ZAux_Direct_GetIfIdle(_hZMC, 3, &ret4);

		if (ret1 == -1 && ret2 == -1 && ret3 == -1 && ret4 == -1)
			break;
		else
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	
	
	return rt;
}

int Mani4A::moveP(int device, const std::array<double, 4>& xyzrz, double vel)
{
	if (device > 1 || device < 0)
		return -1;

	Eigen::Vector4d pos;
	pos << xyzrz[0], xyzrz[1], xyzrz[2], xyzrz[3];
	std::array<double, 4> ret;
	queryJoints(device, ret);
	Eigen::Vector4d currentAngle;
	currentAngle << ret[0], ret[1] , ret[2], ret[3];

	Eigen::Vector4d angles = palletRobot_->selectNearest(palletRobot_->inverse(pos), currentAngle);
	
	std::array<double, 4> endAngles;
	endAngles[0] = angles[0];
	for (int i = 1; i < 4; i++)
	{
		endAngles[i] = R2D(angles[i]);
	}
	
	moveJ(device, endAngles, vel);

	return 0;
}

int Mani4A::gripper(int device, int gripper, bool on)
{
	if (device > 1 || device < 0)
		return -1;
	uint32 val = on ? 1 : 0;
	ZAux_Direct_SetOp(_hZMC, gripperOutMap.find(gripper)->second, val);

	return 0;
}

int Mani4A::isGripperOn(int device, int gripper, bool& on)
{
	if (device > 1 || device < 0)
		return -1;
	uint32 val = 0;
	ZAux_Direct_GetIn(_hZMC, gripperInMap.find(gripper)->second, &val);
	on = (val == 1) ? true : false;
	return 0;
}

int Mani4A::isLoadReady(int device, bool& ready)
{
	if (device > 1 || device < 0)
		return -1;
	uint32 val = 0;
	ZAux_Direct_GetIn(_hZMC, LOAD_PORT, &val);
	ready = (val == 1) ? true : false;
	return 0;
}

int Mani4A::isUnloadCleared(int device, bool& cleared)
{
	if (device > 1 || device < 0)
		return -1;
	uint32 val = 0;
	ZAux_Direct_GetIn(_hZMC, UNLOAD_PORT, &val);
	cleared = (val == 1) ? true : false;
	return 0;
}


double Mani4A::R2D(double r)
{
	return r * 180.0 / M_PI;
}

double Mani4A::D2R(double d)
{
	return d * M_PI / 180.0;
}