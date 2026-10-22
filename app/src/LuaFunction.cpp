#include "LuaFunction.h"
#include "ScriptController.h"
#include "MainWindow.h"
#include "JMath.hpp"
#include "JUtils.hpp"
#include "MotionPlan.h"
#include "Mani4A.h"

#include "spdlog/spdlog.h"

#include <sstream>
#include <iomanip>
#include <thread>

#include <QDebug>

using namespace ModuleScript;
using namespace Palletizing;

#define TERMINATE_WITH_ERROR_TIPS(errorTips)                                           \
    do                                                                                 \
    {                                                                                  \
        luaManager->clearInputStack();                                                 \
        std::ostringstream oss;                                                        \
        oss << "(line " << luaManager->getCurrentLine() << ": " << (errorTips) << ")"; \
        scriptController->pushError(oss.str());                                        \
        SPDLOG_WARN("{}", oss.str());              \
        luaManager->terminateScript();                                                 \
        emit scriptController->scriptErrorOccurred(QString::fromStdString(oss.str())); \
        return 0;                                                                      \
    } while (0)


namespace LuaFunction
{
    int sleep(lua_State *L)
    {
        ScriptController* scriptController = std::any_cast<ScriptController*>(LuaManager::userData(L, "scriptController"));
        std::shared_ptr<LuaManager> luaManager = scriptController->luaManager().lock();

        if (luaManager->getInputTopCount() != 1)
            TERMINATE_WITH_ERROR_TIPS("sleep need 1 parametor: timeMs[number]");

        int ms = luaManager->getInputInt(-1);
        if (ms <= 0)
            TERMINATE_WITH_ERROR_TIPS("sleep time should be positive");
        
        SPDLOG_DEBUG("execute sleep {} ms", ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));

        luaManager->clearInputStack();
        return 0;
    }

    int print(lua_State* L)
    {
        ScriptController* scriptController = std::any_cast<ScriptController*>(LuaManager::userData(L, "scriptController"));
        std::shared_ptr<LuaManager> luaManager = scriptController->luaManager().lock();
        if (luaManager->getInputTopCount() != 1)
            TERMINATE_WITH_ERROR_TIPS("print need 1 parametor: msg[string]");
        std::string msg;
        if (luaManager->isStackString(-1))
            msg = luaManager->getInputString(-1);
        else if (luaManager->isStackNumber(-1))
        {
            double num = luaManager->getInputDouble(-1);
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(4) << num;
            msg = oss.str();
        }
        else
			TERMINATE_WITH_ERROR_TIPS("print only support string or number parametor");
        
        SPDLOG_INFO("{}", msg);
        luaManager->clearInputStack();
		return 0;
    }

    int plan(lua_State* L)
    {
        ScriptController* scriptController = std::any_cast<ScriptController*>(LuaManager::userData(L, "scriptController"));
        std::shared_ptr<LuaManager> luaManager = scriptController->luaManager().lock();

		MainWindow* mainWindow = MainWindow::getMainWindow();
        if (!mainWindow)
            TERMINATE_WITH_ERROR_TIPS("");
        
        auto settings = mainWindow->getSettings();
        int plateLength = settings->value("basic/plateLength", 1000).toInt();
        int plateWidth = settings->value("basic/plateWidth", 1000).toInt();
        int boxLength = settings->value("basic/boxLength", 100).toInt();
        int boxWidth = settings->value("basic/boxWidth", 100).toInt();
        int gripperCount = settings->value("basic/gripperCount", 3).toInt();
		int outlineTolerance = settings->value("basic/outlineTolerance", 100).toInt();
        
        Palletizer::PalletizingInput input;
		input.palletLength = plateLength;
		input.palletWidth = plateWidth;
		input.boxLength = boxLength;
		input.boxWidth = boxWidth;
        input.availableBoxes = 1000;
        input.outlineTolerance = outlineTolerance;
        const Palletizer palletizer;
        const Palletizer::PalletizingOutput output = palletizer.Calculate(input);
        if (!output.success)
            TERMINATE_WITH_ERROR_TIPS("垛型计算失败, 过大的形状误差");

        MotionPlan::MotionPlanInput motionInput;
        motionInput.palletizingOutput = output;
		motionInput.gripperCount = gripperCount;
        const MotionPlan motionPlan;
        const MotionPlan::MotionPlanOutput motionOutput = motionPlan.Calculate(motionInput);
        if (!motionOutput.success)
            TERMINATE_WITH_ERROR_TIPS("运动规划失败");

        luaManager->setUserData("palletizingOutput", std::make_any<Palletizer::PalletizingOutput>(output));
		luaManager->setUserData("motionPlanOutput", std::make_any<MotionPlan::MotionPlanOutput>(motionOutput));

        return 0;
    }

    int palletLevel(lua_State* L)
    {
        
        ScriptController* scriptController = std::any_cast<ScriptController*>(LuaManager::userData(L, "scriptController"));
        std::shared_ptr<LuaManager> luaManager = scriptController->luaManager().lock();

        if (luaManager->getInputTopCount() != 2)
            TERMINATE_WITH_ERROR_TIPS("print need 2 parametor: device[int], level[int]");

        MainWindow* mainWindow = MainWindow::getMainWindow();
        if (!mainWindow)
            TERMINATE_WITH_ERROR_TIPS("");

        int device = luaManager->getInputInt(-2);
		int level = luaManager->getInputInt(-1);
		const MotionPlan::MotionPlanOutput motionOutput = std::any_cast<MotionPlan::MotionPlanOutput>(luaManager->userData(L, "motionPlanOutput"));
		Palletizer::PalletizingOutput palletizingOutput = std::any_cast<Palletizer::PalletizingOutput>(luaManager->userData(L, "palletizingOutput"));
        if (level % 2 == 1)
            palletizingOutput = Palletizer().MirrorOutput(palletizingOutput);
        emit scriptController->palletizingUpdated(palletizingOutput);


        if (device != 0 && device != 1)
			TERMINATE_WITH_ERROR_TIPS("device should be 0 or 1");

        /*if (level == 0)
        {
            bool cleared = true;
            if (int err = mainWindow->getMani4A()->isUnloadCleared(device, cleared))
                TERMINATE_WITH_ERROR_TIPS("isUnloadCleared failed");
            if (!cleared)
                TERMINATE_WITH_ERROR_TIPS("下料位未清空");
        }*/

        /******************运动执行******************/

        auto waitMotionIdle = [luaManager, mainWindow, device]() {
            while (true)
            {
                if (luaManager->getTerminateFlag())
                    return 0;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
                uint32_t state = 0;
                int err = mainWindow->getMani4A()->queryState(device, state);
                if (err || !JUtils::BitOperation::bit(state, 0))
                    break;
            }
            return 0;
        };

        //数据
        //自动速度
        double autoRate = mainWindow->getSettings()->value("basic/autoRate", 10).toDouble();
        //上料点的Z高度
        double loadZHeight = mainWindow->getSettings()->value("basic/loadZHeight", 100).toDouble();
        //上料点的关节坐标
        QString loadJointStr = mainWindow->getSettings()->value((device == 0 ? "calibration/pickPosJ_1" : "calibration/pickPosJ_2"), "0,0,0,0").toString();
        QStringList loadJointList = loadJointStr.split(",");
        if (loadJointList.size() != 4)
            TERMINATE_WITH_ERROR_TIPS("invalid load joint coordinates");
        std::array<double, 4> loadJoint;
        for (int i = 0; i < 4; ++i)
            loadJoint[i] = loadJointList[i].toDouble();
        //等待夹紧时间
        int gripWaitTime = mainWindow->getSettings()->value("basic/gripWaitTime", 2000).toInt();
        //下料点Z高度
        double unloadZHeight = mainWindow->getSettings()->value("basic/unloadZHeight", 100).toDouble();
        //托盘笛卡尔坐标
		QString palletPoseStr = mainWindow->getSettings()->value(device == 0 ? "calibration/palletPos_1" : "calibration/palletPos_2", "0,0,0,0").toString();
        QStringList palletPoseList = palletPoseStr.split(",");
        if (palletPoseList.size() != 4)
            TERMINATE_WITH_ERROR_TIPS("invalid palletPos coordinates");
        std::array<double, 4> palletPose;
        for (int i = 0; i < 4; ++i)
            palletPose[i] = palletPoseList[i].toDouble();
		int plateLength = mainWindow->getSettings()->value("basic/plateLength", 1000).toInt();
		int plateWidth = mainWindow->getSettings()->value("basic/plateWidth", 1000).toInt();
		palletPose[0] = palletPose[0] + (plateLength - palletizingOutput.occupiedLength) / 2.0;
		palletPose[1] = palletPose[1] + (plateWidth - palletizingOutput.occupiedWidth) / 2.0;
        //箱子高度
		int boxHeight = mainWindow->getSettings()->value("basic/boxHeight", 100).toInt();

        auto mapToBase = [palletPose](const std::array<double, 4>& poseInPallet)->std::array<double, 4> {
			Eigen::Matrix4d matBase2Pallet = JMath::makeTranslate(Eigen::Vector3d(palletPose[0], palletPose[1], palletPose[2]));
            matBase2Pallet = matBase2Pallet * JMath::makeRotate<double>(Eigen::Vector3d::UnitZ(), palletPose[3]);
			std::array<double, 6> poseInPalletTmp = {poseInPallet[0], poseInPallet[1], poseInPallet[2], 0, 0, poseInPallet[3] };
            std::array<double, 6> poseInBase = JMath::transformMatrixToXyzrpy<double>(matBase2Pallet * JMath::xyzrpyToTransformMatrix(poseInPalletTmp));
            return {poseInBase[0], poseInBase[1], poseInBase[2], poseInBase[5]};
        };

		for (const auto& act : motionOutput.actions)
		{
            if (act.type == MotionPlan::ActionType::Load)
            {
                //等待上料点ready
                emit scriptController->motionExecuting(QObject::tr("等待上料点就绪"));
                /*while (true)
                {
                    if (luaManager->getTerminateFlag())
                        return 0;

                    bool ready = true;
                    if (int err = mainWindow->getMani4A()->isLoadReady(device, ready))
                        TERMINATE_WITH_ERROR_TIPS("isLoadReady failed");
                    if (ready)
                        break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }*/

                Sleep(1000);

                //运动至上料点
                emit scriptController->motionExecuting(QObject::tr("运动至上料点"));
                std::array<double, 4> loadJointZUp = loadJoint;
                loadJointZUp[0] += loadZHeight;
                if (int err = mainWindow->getMani4A()->moveJ(device, loadJointZUp, autoRate))
                    TERMINATE_WITH_ERROR_TIPS("moveJ to load joint failed");
                waitMotionIdle();
                if (int err = mainWindow->getMani4A()->moveJ(device, loadJoint, autoRate))
                    TERMINATE_WITH_ERROR_TIPS("moveJ to load joint failed");
                waitMotionIdle();

                //抓取
                emit scriptController->motionExecuting(QObject::tr("抓取物料"));
                for (const auto& gripperIndex : act.gripperIndices)
                    if (int err = mainWindow->getMani4A()->gripper(device, gripperIndex, true))
                        TERMINATE_WITH_ERROR_TIPS("grip failed");
                /*while (true)
                {
                    if (luaManager->getTerminateFlag())
                        return 0;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    bool isOn = true;
                    for (const auto& gripperIndex : act.gripperIndices)
                    {
                        bool isOnTmp = false;
                        if (int err = mainWindow->getMani4A()->isGripperOn(device, act.gripperIndices[gripperIndex], isOnTmp))
                            TERMINATE_WITH_ERROR_TIPS("isGripperOn failed");
                        if (!isOnTmp)
                            isOn = false;
                    }
                    if (isOn)
                        break;
                }*/
                std::this_thread::sleep_for(std::chrono::milliseconds(gripWaitTime));

                //抬高
                if (int err = mainWindow->getMani4A()->moveJ(device, loadJointZUp, autoRate))
                    TERMINATE_WITH_ERROR_TIPS("moveJ to load joint failed");
                waitMotionIdle();
            }
            else
            {
                std::array<double, 4> unloadPoseInPallet = { 0 };
                unloadPoseInPallet[0] = act.centerX;
                unloadPoseInPallet[1] = act.centerY;
                unloadPoseInPallet[2] = boxHeight * level;
				unloadPoseInPallet[3] = JMath::D2R(act.angleDegrees);
				std::array<double, 4> unloadPoseInBase = mapToBase(unloadPoseInPallet);
				std::array<double, 4> unloadPoseInBaseZUp = unloadPoseInBase;
                unloadPoseInBaseZUp[2] += unloadZHeight;

                std::cout << "loadPose" << unloadPoseInBaseZUp[0]<<"," << unloadPoseInBaseZUp[1] << "," << unloadPoseInBaseZUp[2] << "," << unloadPoseInBaseZUp[3] << std::endl;

                //运动至下料点
				emit scriptController->motionExecuting(QObject::tr("运动至下料点"));
				if (int err = mainWindow->getMani4A()->moveP(device, unloadPoseInBaseZUp, autoRate))
					TERMINATE_WITH_ERROR_TIPS("moveP to unload pose failed");
				waitMotionIdle();
                if (int err = mainWindow->getMani4A()->moveP(device, unloadPoseInBase, autoRate))
                    TERMINATE_WITH_ERROR_TIPS("moveP to unload pose failed");
                waitMotionIdle();

				//释放
                emit scriptController->motionExecuting(QObject::tr("放置物料"));
                for (const auto& gripperIndex : act.gripperIndices)
                    if (int err = mainWindow->getMani4A()->gripper(device, gripperIndex, false))
                        TERMINATE_WITH_ERROR_TIPS("grip failed");
                /*while (true)
                {
                    if (luaManager->getTerminateFlag())
                        return 0;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    bool isOn = false;
                    for (const auto& gripperIndex : act.gripperIndices)
                    {
                        bool isOnTmp = true;
                        if (int err = mainWindow->getMani4A()->isGripperOn(device, act.gripperIndices[gripperIndex], isOnTmp))
                            TERMINATE_WITH_ERROR_TIPS("isGripperOn failed");
                        if (isOnTmp)
                            isOn = true;
                    }
                    if (!isOn)
                        break;
                }*/
                std::this_thread::sleep_for(std::chrono::milliseconds(gripWaitTime));

                //抬高
                if (int err = mainWindow->getMani4A()->moveP(device, unloadPoseInBaseZUp, autoRate))
                    TERMINATE_WITH_ERROR_TIPS("moveP to unload pose failed");
                waitMotionIdle();
            }
		}

        /******************************************/

        return 0;
    }

    int getTotalLevel(lua_State* L)
    {
        ScriptController* scriptController = std::any_cast<ScriptController*>(LuaManager::userData(L, "scriptController"));
        std::shared_ptr<LuaManager> luaManager = scriptController->luaManager().lock();

        MainWindow* mainWindow = MainWindow::getMainWindow();
        if (!mainWindow)
            TERMINATE_WITH_ERROR_TIPS("");

		int totalLevel = mainWindow->getSettings()->value("basic/totalLevel", 5).toInt();

        luaManager->pushOutputInt(totalLevel);

        return 1;
    }
}