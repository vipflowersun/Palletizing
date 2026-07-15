#include "MonitorWidget.h"
#include "ComponentWidget.h"
#include "MainWindow.h"
#include "Mani4A.h"
#include "JUtils.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QTimer>
using namespace Palletizing;

MonitorWidget::MonitorWidget(QWidget* parent) : QWidget(parent)
{
	const QSize ledSize(24, 24);

	QStringList titles = { tr("机械手1运动:"), tr("机械手2运动:"), tr("机械手1报警"), tr("机械手2报警"),
	tr("夹爪1_1夹紧:"),tr("夹爪1_2夹紧:"),tr("夹爪1_3夹紧:"),tr("夹爪2_1夹紧:"),tr("夹爪2_2夹紧:"),tr("夹爪2_3夹紧:"),
	tr("上料信号1"), tr("下料位清空信号1"), tr("上料信号2"), tr("下料位清空信号2") };
	QGridLayout* layout = new QGridLayout(this);
	layout->setContentsMargins(12, 10, 12, 10);
	layout->setHorizontalSpacing(24);
	layout->setVerticalSpacing(6);

	for (int index = 0; index < titles.size(); ++index)
	{
		const int row = index % 6;
		const int column = (index / 6) * 2;

		QLabel* titleLabel = new QLabel(titles.at(index), this);
		titleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		layout->addWidget(titleLabel, row, column);

		CircleLedWidget* led = new CircleLedWidget(this);
		led->setFixedSize(ledSize);
		layout->addWidget(led, row, column + 1, Qt::AlignLeft | Qt::AlignVCenter);
		_leds.push_back(led);
	}

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &MonitorWidget::updateStatus);
	timer->setInterval(200);
	timer->start();
}

void MonitorWidget::updateStatus()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;
	
	uint32_t state = 0;
	if (int err = mainWindow->getMani4A()->queryState(0, state))
		return;
	_leds[0]->setOn(JUtils::BitOperation::bit(state, 0));
	_leds[2]->setOn(JUtils::BitOperation::bit(state, 1));
	if (int err = mainWindow->getMani4A()->queryState(1, state))
		return;
	_leds[1]->setOn(JUtils::BitOperation::bit(state, 0));
	_leds[3]->setOn(JUtils::BitOperation::bit(state, 1));
	bool on = false;
	if (int err = mainWindow->getMani4A()->isGripperOn(0, 0, on))
		return;
	_leds[4]->setOn(on);
	if (int err = mainWindow->getMani4A()->isGripperOn(0, 1, on))
		return;
	_leds[5]->setOn(on);
	if (int err = mainWindow->getMani4A()->isGripperOn(0, 2, on))
		return;
	_leds[6]->setOn(on);
	if (int err = mainWindow->getMani4A()->isGripperOn(1, 0, on))
		return;
	_leds[7]->setOn(on);
	if (int err = mainWindow->getMani4A()->isGripperOn(1, 1, on))
		return;
	_leds[8]->setOn(on);
	if (int err = mainWindow->getMani4A()->isGripperOn(1, 2, on))
		return;
	_leds[9]->setOn(on);
	if (int err = mainWindow->getMani4A()->isLoadReady(0, on))
		return;
	_leds[10]->setOn(on);
	if (int err = mainWindow->getMani4A()->isUnloadCleared(0, on))
		return;
	_leds[11]->setOn(on);
	if (int err = mainWindow->getMani4A()->isLoadReady(1, on))
		return;
	_leds[12]->setOn(on);
	if (int err = mainWindow->getMani4A()->isUnloadCleared(1, on))
		return;
	_leds[13]->setOn(on);
}