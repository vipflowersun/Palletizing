#include "AutoPage.h"
#include "MainWindow.h"
#include "ScriptController.h"
#include "Mani4A.h"
#include "MotionInfoDisplayWidget.h"
#include "ui_AutoPageUi.h"

#include "ElaScrollArea.h"
#include "ElaPushButton.h"

#include <QFont>
#include <QVBoxLayout>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

using namespace Palletizing;
using namespace ModuleScript;

AutoPage::AutoPage(QWidget* parent) : QWidget(parent)
{
	_ui = new Ui::AutoPage();
	_ui->setupUi(this);

	QFont buttonFont = font();
	buttonFont.setPointSize(28);
	buttonFont.setWeight(QFont::Medium);
	_ui->btnStart->setFont(buttonFont);
	_ui->btnStop->setFont(buttonFont);

	const QString buttonStyle = QStringLiteral(
		"QPushButton {"
		" background-color: #ffffff;"
		" border: 1px solid #dcdfe6;"
		" border-radius: 4px;"
		" color: #1f1f1f;"
		" padding: 8px 16px;"
		"}"
		"QPushButton:hover {"
		" background-color: #f5f9ff;"
		" border-color: #409eff;"
		"}"
		"QPushButton:pressed {"
		" background-color: #ecf5ff;"
		" border-color: #337ecc;"
		"}"
		"QPushButton:disabled {"
		" background-color: #f5f5f5;"
		" border-color: #e4e7ed;"
		" color: #a8abb2;"
		"}");
	_ui->btnStart->setStyleSheet(buttonStyle);
	_ui->btnStop->setStyleSheet(buttonStyle);


	connect(_ui->btnStart, &QPushButton::clicked, this, &AutoPage::onBtnStart);
	connect(_ui->btnStop, &QPushButton::clicked, this, &AutoPage::onBtnStop);
	connect(MainWindow::getMainWindow()->getScriptController(), &ScriptController::scriptStarted, this, &AutoPage::onScriptStarted);
	connect(MainWindow::getMainWindow()->getScriptController(), &ScriptController::scriptFinished, this, &AutoPage::onScriptFinished);
	connect(MainWindow::getMainWindow()->getScriptController(), &ScriptController::motionExecuting, this, &AutoPage::onScriptMotionExecuting);
	connect(MainWindow::getMainWindow()->getScriptController(), &ScriptController::palletizingUpdated, this, &AutoPage::onPalletizingUpdated);
}

void AutoPage::onBtnStart()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	ScriptController* scriptController = mainWindow->getScriptController();

	QFile file(QCoreApplication::applicationDirPath() + "/scripts/process.lua");
	QString fileScriptContent;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// 读取文件内容
		QTextStream in(&file);
		fileScriptContent = in.readAll();
		file.close();
	}
	else
	{
		mainWindow->appendLogMessage(tr("未找到process.lua"), 1);
		return;
	}

	if (fileScriptContent.isEmpty())
	{
		mainWindow->appendLogMessage(tr("空的process.lua"), 1);
		return;
	}

	//mainWindow->getScriptController()->clearError();
	if (!mainWindow->getScriptController()->runScript(fileScriptContent.toStdString()))
		mainWindow->appendLogMessage(tr("执行失败"), 2);
}

void AutoPage::onBtnStop()
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;
	mainWindow->getMani4A()->terminate();
	mainWindow->getScriptController()->terminateScript();
}

void AutoPage::onPalletizingUpdated(const Palletizer::PalletizingOutput& output)
{
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;

	int palletLength = mainWindow->getSettings()->value("basic/plateLength", 1000).toInt();
	int palletWidth = mainWindow->getSettings()->value("basic/plateWidth", 1000).toInt();

	_ui->widPalletizingOutput->setPalletizingOutput(palletLength, palletWidth, output);
}

void AutoPage::onScriptStarted()
{
	_ui->btnStart->setEnabled(false);
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;
	mainWindow->appendLogMessage(tr("开始运行"), 0);
}

void AutoPage::onScriptFinished(const QString& errorMsg)
{
	_ui->btnStart->setEnabled(true);
	MainWindow* mainWindow = MainWindow::getMainWindow();
	if (!mainWindow)
		return;
	mainWindow->appendLogMessage(tr("停止运行"), 1);
}

void AutoPage::onScriptMotionExecuting(const QString& motionInfo)
{
	_ui->motionInfoDisplay->updateMotionInfo(motionInfo);
}
