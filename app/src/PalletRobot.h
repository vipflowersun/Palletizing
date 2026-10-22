#pragma once
#include "Eigen/Dense"
#include <cmath>
#include <vector>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <iostream>

#define PI  3.14159265358979323846

template<typename T>
const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

class PalletRobot {

public:
    explicit PalletRobot();

    void initParameters(double l1, double l2);
    void setJointLimits(const std::pair<double, double> limits[4]);
    // -------------------- 正解 --------------------
    Eigen::Vector4d forward(const Eigen::Vector4d& q) const;
    std::vector<Eigen::Vector4d> inverse(const Eigen::Vector4d& pose) const;
    Eigen::Vector4d selectNearest(const std::vector<Eigen::Vector4d>& sols,const Eigen::Vector4d& current) const;
    bool isJointValid(const Eigen::Vector4d& q) const;
private:
    static double normalize(double a) {
        while (a > PI) a -= 2.0 * PI;
        while (a < -PI) a += 2.0 * PI;
        return a;
    }

    static double angleDiff(double a, double b) {
        double d = a - b;
        while (d > PI) d -= 2.0 * PI;
        while (d < -PI) d += 2.0 * PI;
        return d;
    }

private:
    double L1; // 大臂长度
    double L2; // 小臂长度

    std::pair<double, double> q_limits[4];
};


