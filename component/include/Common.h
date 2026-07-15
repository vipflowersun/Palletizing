#pragma once
#include "Eigen/Dense"

namespace Common
{
    struct MatchPoint
    {
        Eigen::Vector3d source;
        Eigen::Vector3d match;
    };

    class WeldSeamKeyPoint : public Eigen::Matrix4d
    {
    public:
        enum class Type
        {
            Face3IntersectionPoint = 0,
            Face2IntersectionLineVertexPoint = 1,
            ProjectionPoint = 2,
        };
    public:
        WeldSeamKeyPoint() : Eigen::Matrix4d(Eigen::Matrix4d::Identity()) {}
        WeldSeamKeyPoint(double x, double y, double z) : Eigen::Matrix4d(Eigen::Matrix4d::Identity()) {
            (*this)(0, 3) = x;
            (*this)(1, 3) = y;
			(*this)(2, 3) = z;
        }
    public:
        Type type; //焊缝点类型
        std::vector<Eigen::Vector3d> relativeFaceNormals; //相对于焊缝点的面法向量列表
        int weld_type = 0;        // 0直线点,1圆弧线点
        int weld_dir = -1;   // 焊缝方向,0平焊缝,1立焊缝,-1(既不是立焊缝也不是平焊缝,判断默认容差30°)
    };

    class Vertex : public Eigen::Vector3d
    {
    public:
        Vertex() : Eigen::Vector3d(Eigen::Vector3d::Zero()) {}
        Vertex(double x, double y, double z) : Eigen::Vector3d(x, y, z) {}
    public:
		std::vector<Eigen::Vector3d> directions; //顶点的方向向量列表
    };

	//焊接模式
	enum WeldMode
	{
		P_MIG, //脉冲
		FP_MIG, //快速脉冲
		DP_MIG, //大熔深
		MAG,	//恒压
		LST		//软飞溅
	};

	//焊接参数
	struct WeldingParameters {
		//焊接参数
		WeldMode weldMode = P_MIG; //焊接模式
		bool separateMode = true; //是否分别模式，true 表示分别模式，false 表示一元化模式
		double weldCurrent = 200.0; //焊接电流 A
		double weldVoltage = 20.0; //焊接电压%
		double currentStart = 200.0; //起弧电流 A
		double voltageStart = 20.0; //起弧电压%
		double currentEnd = 200.0;   //收弧电流 A
		double voltageEnd = 20.0;   //收弧电压%

		//电弧跟踪系统参数
		bool enabledArcTracking = false; // 是否启用电弧跟踪
		double coeff_K = 2.0;    // 循环队列系数（number, 单位：次，≥2）数据采集长度系数, K 个摆动周期内不进行跟踪
		int lag_time = 460; // 滞后时间（number, 单位：ms，≥0）：默认值为 165
		int start_time = 500; // 跟踪开始时间（number, 单位：ms，≥0）
		std::tuple<double, double> sensitivity_coeff = { 0.5, 0.5 }; // Y 方 向 和 Z 方 向 的 敏 感 系 数 （10≥X≥0.1）,调节相应的纠偏能力的参数。推荐敏感系数设置 : 0.3 - 1.0
		double diff_constant = 12.0; // 电流差常量（number, 单位：无， > 0） 用于跟踪算法的常数，焊丝直径 1.2mm 时默认为 12
		double diff_value = 2.0; // 电流差设定值（number, 单位：A，≥0）  用于电流差在设定值范围内不进行纠偏
		bool voltage_track = false; // 弧压跟踪, 当为 true 时，使用弧压跟踪，默认 false
		bool calc_lag_time = false; // 计算滞后时间，bool 类型，默认值 false
		bool arc_dir = false; // 圆弧焊接方向，bool 类型  false 表示与圆平面垂直，true 表示与圆平面平行
		int track_ref_cycle_times = 1; // 基准参考周期

		//电弧跟踪参数
		double freq = 10.0; // 数据更新参考频率
		std::tuple<bool, bool> active = { false, false }; // {active_y, active_z}, 补偿方向开关
		std::tuple<double, double> kp = { 200.0, 200.0 }; //  {kp_y, kp_z}, 补偿方向上的控制 kp 系数, 一般不用调整，给 {200,200}
		std::tuple<double, double> kd = { 25.0, 25.0 }; // {kd_y, kd_z}, 补偿方向上的控制 kd 系数, 一般不用调整，给 {25,25}
		std::tuple<double, double> limit = { 0.1, 0.1 }; //{limit_y, limit_z}, 补偿方向上最大补偿偏移量，单位 m
		std::tuple<double, double> limit_vel = { 0.1, 0.1 }; // { limit_vel_y, limit_vel_z }, 补偿方向上最大补偿速度，单位 m / s
		double base_current = 20.0; // 基准电流
		int k = 4; // 系数，默认 4

		//摆焊参数
		bool enabledFlippingWelding = false; // 是否启用摆焊
		int type = 1; // 1：平面三角形轨迹，2：平面正弦轨迹，3：平面圆形轨迹， 4：平面梯形轨迹，5：平面 8 字形轨迹
		int ref_plane = 0; //参考平面，0：工具 XOY 平面，1：工具 XOZ 平面，2：工具 YOZ 平面，3：工件 XOY 平面，4：工件 XOZ 平面，5：工件 YOZ 平面。
		double fq = 2.0; // 频率，单位（Hz）。
		double amp = 0.001; // 振幅，单位（m）。
		double leftMs = 1.0; // 左右停留时间，仅针对平面梯形轨迹以及平面正弦轨迹有效，单位 ms。
		double rightMs = 1.0; // 左右停留时间，仅针对平面梯形轨迹以及平面正弦轨迹有效，单位 ms。
	};
}