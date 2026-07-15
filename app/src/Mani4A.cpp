#include "Mani4A.h"

using namespace Palletizing;

#define ERROR_ZMC_OFFSET								(0x2000)
#define ERROR_ZMC_NOT_OPEN								(ERROR_ZMC_OFFSET + 1)
#define ERROR_ZMC_INVALID_INPUT							(ERROR_ZMC_OFFSET + 2)
#define ERROR_ZMC_ERROR_GCODE_TYPE_NOT_SUPORT			(ERROR_ZMC_OFFSET + 3)
#define ERROR_ZMC_INVALID_AXISID						(ERROR_ZMC_OFFSET + 4)

#define AXIS_COUNT (7)

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
	int rt = ZAux_OpenEth((char*)("192.168.100.11"), &_hZMC);
	CHECK_CMD_ZMC(ZAux_OpenEth, rt);
	rt = ZAux_SetTimeOut(_hZMC, 5000);
	CHECK_CMD_ZMC(ZAux_SetTimeOut, rt);
	//全轴使能
	for (int i = 0; i < AXIS_COUNT; i++)
	{
		rt = ZAux_Direct_SetAxisEnable(_hZMC, i, 1);
		CHECK_CMD_ZMC(ZAux_Direct_SetAxisEnable, rt);
	}
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
	return 0;
}

int Mani4A::queryJoints(int device, std::array<double, 4>& joints)
{
	return 0;
}

int Mani4A::queryPose(int device, std::array<double, 4>& xyzrz)
{
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
	return 0;
}

int Mani4A::moveP(int device, const std::array<double, 4>& xyzrz, double vel)
{
	return 0;
}

int Mani4A::gripper(int device, int gripper, bool on)
{
	return 0;
}

int Mani4A::isGripperOn(int device, int gripper, bool& on)
{
	return 0;
}

int Mani4A::isLoadReady(int device, bool& ready)
{
	return 0;
}

int Mani4A::isUnloadCleared(int device, bool& cleared)
{
	return 0;
}
