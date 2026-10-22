#include "PalletRobot.h"


PalletRobot::PalletRobot() {
    for (int i = 0; i < 4; ++i) 
    {
        q_limits[i] = { -std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity() };
    }
}

void PalletRobot::initParameters(double l1, double l2)
{
    L1 = l1;
    L2 = l2;
}

void PalletRobot::setJointLimits(const std::pair<double, double> limits[4]) 
{
    for (int i = 0; i < 4; ++i) 
    {
        if (limits[i].first > limits[i].second)
            throw std::invalid_argument("Joint limit min > max");
        q_limits[i] = limits[i];
    }
}


Eigen::Vector4d PalletRobot::forward(const Eigen::Vector4d& q) const 
{
    const double c2 = std::cos(q[1]);
    const double s2 = std::sin(q[1]);
    const double c23 = std::cos(q[1] + q[2]);
    const double s23 = std::sin(q[1] + q[2]);

    Eigen::Vector4d p;
    p[0] = L1 * c2 + L2 * c23;
    p[1] = L1 * s2 + L2 * s23;
    p[2] = q[0];
    p[3] = q[1] + q[2] + q[3];    // theta  
    return p;
}

std::vector<Eigen::Vector4d> PalletRobot::inverse(const Eigen::Vector4d& pose) const
{
    std::vector<Eigen::Vector4d> sols;
    const double x = pose[0]-322, y = pose[1], z = pose[2], theta = pose[3];

    const double r2 = x * x + y * y;
    const double r = std::sqrt(r2);

    const double maxReach = L1 + L2;
    const double minReach = std::abs(L1 - L2);
    constexpr double EPS = 1e-9;
    if (r > maxReach + EPS || r < minReach - EPS)
    {
        return sols; //  不可达
    }

    double c3 = (r2 - L1 * L1 - L2 * L2) / (2.0 * L1 * L2);
    c3 = clamp(c3, -1.0, 1.0);

    const double s3_mag = std::sqrt(std::max(0.0, 1.0 - c3 * c3));
    const double s3_candidates[2] = { s3_mag, -s3_mag };

    for (double s3 : s3_candidates) 
    {
        const double q3 = std::atan2(s3, c3);

        // 大臂角 q2
        const double k1 = L1 + L2 * c3;
        const double k2 = L2 * s3;
        const double q2 = std::atan2(y, x) - std::atan2(k2, k1);

        // 末端角 q4
        const double q4 = theta - q2 - q3;

        Eigen::Vector4d q;
        q << z, normalize(q2), normalize(q3), normalize(q4);

        // 限位检查 
        if(true)// (isJointValid(q)) 
        {
            sols.push_back(q);
        }
    }

    // 去重 
    if (sols.size() == 2)
    {
        if (std::abs(sols[0][1] - sols[1][1]) < 1e-6 &&
            std::abs(sols[0][2] - sols[1][2]) < 1e-6 &&
            std::abs(sols[0][3] - sols[1][3]) < 1e-6)
        {
            sols.resize(1);
        }
    }
    return sols;
}


Eigen::Vector4d PalletRobot::selectNearest(const std::vector<Eigen::Vector4d>& sols,const Eigen::Vector4d& current) const
{
    if (sols.empty())
        throw std::runtime_error("IK: no valid solution");

    if (sols.size() == 1) return sols[0];

    double bestCost = std::numeric_limits<double>::max();
    Eigen::Vector4d best = sols[0];

    for (const auto& q : sols)
    {
        double cost = std::abs(q[0] - current[0]);
        for (int i = 1; i < 4; ++i)
        {
            cost += std::abs(angleDiff(q[i], current[i]));
        }
        if (cost < bestCost) 
        {
            bestCost = cost;
            best = q;
        }
    }
    return best;
}


bool PalletRobot::isJointValid(const Eigen::Vector4d& q) const 
{
    constexpr double EPS = 1e-9;
    for (int i = 0; i < 4; ++i) 
    {
        if (q[i] < q_limits[i].first - EPS || q[i] > q_limits[i].second + EPS)
            return false;
    }
    return true;
}


/*
int main() {
    // L1: 800 mm，L2: 600 mm
    PalletRobot robot(800.0, 600.0);

    // 设置关节限位
    std::pair<double, double> limits[4] = {
        {0.0, 2000.0},
        {-PI * 0.75, PI * 0.75},
        {-PI * 0.75, PI * 0.75},
        {-PI, PI}
    };
    robot.setJointLimits(limits);

    // test
    Eigen::Vector4d q;
    q << 1200.0,
        PI / 6.0,
        PI / 3.0,
        PI / 12.0;

    // 正解
    Eigen::Vector4d pose = robot.forward(q);
    std::cout << "x = " << pose[0] << std::endl;
    std::cout << "y = " << pose[1] << std::endl;
    std::cout << "z = " << pose[2] << std::endl;
    std::cout << "theta = " << pose[3] << std::endl;

    // 逆解
    Eigen::Vector4d pose1;
    pose1 << pose[0] + 30.0, pose[1] + 50.0, pose[2] + 300.0, pose[3] + PI / 3.0;

    std::vector<Eigen::Vector4d> sols = robot.inverse(pose1);

    for (size_t i = 0; i < sols.size(); ++i) {
        std::cout << "逆解 " << i + 1 << ": [ " << sols[i].transpose() << " ]" << std::endl;
    }

    // 选择与原始关节角最接近的解
    if (!sols.empty()) {
        Eigen::Vector4d q_nearest = robot.selectNearest(sols, q);
        std::cout << "q_nearest =  " << q_nearest.transpose() << std::endl;
    }

    return 0;
}
*/