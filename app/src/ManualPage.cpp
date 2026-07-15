#include "ManualPage.h"
#include "MainWindow.h"
#include "Mani4A.h"
#include "JMath.hpp"

#include "ElaPushButton.h"
#include "ElaScrollArea.h"
#include "ElaToggleSwitch.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QVBoxLayout>
#include <QDebug>

using namespace Palletizing;

namespace
{
	QLabel* addModeLabel(QGridLayout* layout, int row, const QString& title, ElaToggleSwitch* toggle,
		const QString& enabledText, const QString& disabledText)
	{
		layout->addWidget(new QLabel(title), row, 0);
		layout->addWidget(toggle, row, 1);
		QLabel* value = new QLabel(disabledText);
		layout->addWidget(value, row, 2);
		QObject::connect(toggle, &ElaToggleSwitch::toggled, value,
			[=](bool checked) { value->setText(checked ? enabledText : disabledText); });
		return value;
	}

	QWidget* createJogButtons(ManualPage* page, int robot, const QStringList& axes,
		ElaToggleSwitch* modeToggle, ElaToggleSwitch* speedToggle)
	{
		QWidget* widget = new QWidget;
		QGridLayout* layout = new QGridLayout(widget);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(new QLabel(QObject::tr("轴")), 0, 0);
		layout->addWidget(new QLabel(QObject::tr("负方向")), 0, 1);
		layout->addWidget(new QLabel(QObject::tr("正方向")), 0, 2);

		for (int row = 0; row < axes.size(); ++row)
		{
			const QString axis = axes.at(row);
			layout->addWidget(new QLabel(axis), row + 1, 0);
			for (int column = 1; column <= 2; ++column)
			{
				const bool positive = column == 2;
				ElaPushButton* button = new ElaPushButton(
					QObject::tr("%1 %2").arg(axis, positive ? QObject::tr("+") : QObject::tr("-")), widget);
				button->setMinimumSize(92, 34);
				layout->addWidget(button, row + 1, column);
				QObject::connect(button, &QPushButton::pressed, page,
					[=]() { emit page->jogStarted(robot, axis, positive, modeToggle->getIsToggled(), speedToggle->getIsToggled()); });
				QObject::connect(button, &QPushButton::released, page,
					[=]() { emit page->jogStopped(robot, axis, positive, modeToggle->getIsToggled(), speedToggle->getIsToggled()); });
			}
		}
		return widget;
	}

	QGroupBox* createRobotGroup(ManualPage* page, int robot, int gripperCount)
	{
		QGroupBox* group = new QGroupBox(QObject::tr("机械手%1控制区").arg(robot));
		group->setFixedWidth(543);
		QVBoxLayout* groupLayout = new QVBoxLayout(group);

		QGridLayout* optionsLayout = new QGridLayout;
		ElaToggleSwitch* modeToggle = new ElaToggleSwitch(group);
		modeToggle->setEnabled(false);
		ElaToggleSwitch* speedToggle = new ElaToggleSwitch(group);
		addModeLabel(optionsLayout, 0, QObject::tr("点动模式"), modeToggle,
			QObject::tr("笛卡尔点动"), QObject::tr("关节点动"));
		addModeLabel(optionsLayout, 1, QObject::tr("点动速度"), speedToggle,
			QObject::tr("点动高速"), QObject::tr("点动低速"));
		groupLayout->addLayout(optionsLayout);

		QStackedWidget* jogStack = new QStackedWidget(group);
		jogStack->addWidget(createJogButtons(page, robot, {QObject::tr("J1"), QObject::tr("J2"),
			QObject::tr("J3"), QObject::tr("J4")}, modeToggle, speedToggle));
		jogStack->addWidget(createJogButtons(page, robot, {QObject::tr("X"), QObject::tr("Y"),
			QObject::tr("Z"), QObject::tr("RZ")}, modeToggle, speedToggle));
		QObject::connect(modeToggle, &ElaToggleSwitch::toggled, jogStack,
			[=](bool cartesian) { jogStack->setCurrentIndex(cartesian ? 1 : 0); });
		groupLayout->addWidget(jogStack);

		QGroupBox* gripperGroup = new QGroupBox(QObject::tr("夹爪控制"), group);
		QGridLayout* gripperLayout = new QGridLayout(gripperGroup);
		for (int index = 0; index < gripperCount; ++index)
		{
			ElaToggleSwitch* gripperToggle = new ElaToggleSwitch(gripperGroup);
			QLabel* state = new QLabel(QObject::tr("关闭"), gripperGroup);
			gripperLayout->addWidget(new QLabel(QObject::tr("夹爪%1").arg(index + 1)), index, 0);
			gripperLayout->addWidget(gripperToggle, index, 1);
			gripperLayout->addWidget(state, index, 2);
			QObject::connect(gripperToggle, &ElaToggleSwitch::toggled, page,
				[=](bool opened)
				{
					state->setText(opened ? QObject::tr("打开") : QObject::tr("关闭"));
					emit page->gripperToggled(robot, index + 1, opened);
				});
		}
		groupLayout->addWidget(gripperGroup);
		return group;
	}

	QGroupBox* createCalibrationGroup(ManualPage* page)
	{
		QGroupBox* group = new QGroupBox(QObject::tr("位置标定区"));
		group->setFixedWidth(543);
		QGridLayout* layout = new QGridLayout(group);
		ElaPushButton* loadingButton1 = new ElaPushButton(QObject::tr("记录机械手1上料点位置"), group);
		ElaPushButton* unloadingButton1 = new ElaPushButton(QObject::tr("记录机械手1垛盘位置"), group);
		layout->addWidget(loadingButton1, 0, 0);
		layout->addWidget(unloadingButton1, 0, 1);
		QObject::connect(loadingButton1, &QPushButton::clicked, page, &ManualPage::onLoadingPointRecordRequested1);
		QObject::connect(unloadingButton1, &QPushButton::clicked, page, &ManualPage::onUnloadingPointRecordRequested1);
		ElaPushButton* loadingButton2 = new ElaPushButton(QObject::tr("记录机械手2上料点位置"), group);
		ElaPushButton* unloadingButton2 = new ElaPushButton(QObject::tr("记录机械手2垛盘位置"), group);
		layout->addWidget(loadingButton2, 1, 0);
		layout->addWidget(unloadingButton2, 1, 1);
		QObject::connect(loadingButton2, &QPushButton::clicked, page, &ManualPage::onLoadingPointRecordRequested2);
		QObject::connect(unloadingButton2, &QPushButton::clicked, page, &ManualPage::onUnloadingPointRecordRequested2);
		return group;
	}
}

ManualPage::ManualPage(QWidget* parent) : ElaScrollPage(parent)
{
	this->setTitleVisible(false);

	int gripperCount = MainWindow::getMainWindow()->getSettings()->value("basic/gripperCount", 3).toInt();

	ElaScrollArea* contentWidget = new ElaScrollArea(this);
	contentWidget->setWidgetResizable(true);

	QWidget* scrollContent = new QWidget;
	QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
	contentLayout->setSpacing(32);

	contentLayout->addWidget(createRobotGroup(this, 1, qMax(0, gripperCount)));
	contentLayout->addWidget(createRobotGroup(this, 2, qMax(0, gripperCount)));
	contentLayout->addWidget(createCalibrationGroup(this));
	contentWidget->setWidget(scrollContent);
	this->addCentralWidget(contentWidget, true, true, 0);

	connect(this, &ManualPage::jogStarted, this, &ManualPage::onJogStarted);
	connect(this, &ManualPage::jogStopped, this, &ManualPage::onJogStopped);
	connect(this, &ManualPage::gripperToggled, this, &ManualPage::onGripperToggled);
}

void ManualPage::onLoadingPointRecordRequested1()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	std::array<double, 4> joints = { 0 };
	if (int err = mainWindow->getMani4A()->queryJoints(0, joints))
	{
		mainWindow->appendLogMessage(QObject::tr("获取机械手1关节坐标失败，错误码：%1").arg(err), 2);
		return;
	}
	QString strJoints = QString("%1,%2,%3,%4").arg(joints[0]).arg(joints[1]).arg(joints[2]).arg(joints[3]);
	mainWindow->getSettings()->setValue("calibration/pickPosJ_1", strJoints);

	mainWindow->appendLogMessage(QObject::tr("机械手1上料点标定成功。"), 0);
}

void ManualPage::onUnloadingPointRecordRequested1()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	std::array<double, 4> pose = { 0 };
	if (int err = mainWindow->getMani4A()->queryPose(0, pose))
	{
		mainWindow->appendLogMessage(QObject::tr("获取机械手1笛卡尔坐标失败，错误码：%1").arg(err), 2);
		return;
	}

	int boxLength = mainWindow->getSettings()->value("basic/boxLength", 400).toInt();
	int boxWidth = mainWindow->getSettings()->value("basic/boxWidth", 300).toInt();
	int boxHeight = mainWindow->getSettings()->value("basic/boxHeight", 200).toInt();
	int gripperCount = mainWindow->getSettings()->value("basic/gripperCount", 3).toInt();
	Eigen::Vector3d tmp(-boxLength / 2.0, -boxWidth * gripperCount / 2.0, 0);
	tmp = JMath::makeRotate<double>(Eigen::Vector3d::UnitZ(), pose[3]).block<3, 3>(0, 0) * tmp;
	pose[0] += tmp(0);
	pose[1] += tmp(1);

	QString strPose = QString("%1,%2,%3,%4").arg(pose[0]).arg(pose[1]).arg(pose[2]).arg(pose[3]);
	mainWindow->getSettings()->setValue("calibration/palletPos_1", strPose);

	mainWindow->appendLogMessage(QObject::tr("机械手1垛盘位置标定成功。"), 0);
}

void ManualPage::onLoadingPointRecordRequested2()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	std::array<double, 4> joints = { 0 };
	if (int err = mainWindow->getMani4A()->queryJoints(1, joints))
	{
		mainWindow->appendLogMessage(QObject::tr("获取机械手2关节坐标失败，错误码：%1").arg(err), 2);
		return;
	}
	QString strJoints = QString("%1,%2,%3,%4").arg(joints[0]).arg(joints[1]).arg(joints[2]).arg(joints[3]);
	mainWindow->getSettings()->setValue("calibration/pickPosJ_2", strJoints);

	mainWindow->appendLogMessage(QObject::tr("机械手2上料点标定成功。"), 0);
}

void ManualPage::onUnloadingPointRecordRequested2()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	std::array<double, 4> pose = { 0 };
	if (int err = mainWindow->getMani4A()->queryPose(1, pose))
	{
		mainWindow->appendLogMessage(QObject::tr("获取机械手2笛卡尔坐标失败，错误码：%1").arg(err), 2);
		return;
	}

	int boxLength = mainWindow->getSettings()->value("basic/boxLength", 400).toInt();
	int boxWidth = mainWindow->getSettings()->value("basic/boxWidth", 300).toInt();
	int boxHeight = mainWindow->getSettings()->value("basic/boxHeight", 200).toInt();
	int gripperCount = mainWindow->getSettings()->value("basic/gripperCount", 3).toInt();
	Eigen::Vector3d tmp(-boxLength / 2.0, -boxWidth * gripperCount / 2.0, 0);
	tmp = JMath::makeRotate<double>(Eigen::Vector3d::UnitZ(), pose[3]).block<3, 3>(0, 0) * tmp;
	pose[0] += tmp(0);
	pose[1] += tmp(1);

	QString strPose = QString("%1,%2,%3,%4").arg(pose[0]).arg(pose[1]).arg(pose[2]).arg(pose[3]);
	mainWindow->getSettings()->setValue("calibration/palletPos_2", strPose);

	mainWindow->appendLogMessage(QObject::tr("机械手2垛盘位置标定成功。"), 0);
}

void ManualPage::onJogStarted(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed)
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	int manaulRate = mainWindow->getSettings()->value("basic/manualRate", 20).toInt();

	int err = mainWindow->getMani4A()->jogJ(robot - 1, axis.right(1).toInt() - 1, positive ? (highSpeed ? manaulRate : manaulRate / 2.0) : (highSpeed ? -manaulRate : -manaulRate / 2.0));
	if (err)
		mainWindow->appendLogMessage(QObject::tr("机械手%1轴%2点动失败，错误码：%3").arg(robot).arg(axis).arg(err), 2);
}

void ManualPage::onJogStopped(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed)
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	mainWindow->getMani4A()->terminate();
}

void ManualPage::onGripperToggled(int robot, int gripper, bool opened)
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	int err = mainWindow->getMani4A()->gripper(robot - 1, gripper - 1, opened);
	if (err)
		mainWindow->appendLogMessage(QObject::tr("机械手%1夹爪%2操作失败，错误码：%3").arg(robot).arg(gripper).arg(err), 2);
}