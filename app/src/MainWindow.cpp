#include "MainWindow.h"
#include "ui_MainWindowUi.h"
#include "JMath.hpp"
#include "JUtils.hpp"
#include "AutoPage.h"
#include "ManualPage.h"
#include "SettingsPage.h"
#include "Mani4A.h"
#include "MotionPlan.h"
#include "ScriptController.h"

#include "ElaMessageBar.h"
#include "ElaContentDialog.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QSignalBlocker> 
#include <QDebug>

using namespace Palletizing;
using namespace ModuleScript;

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
{
    initSPDLogger();

    initModules();

    initUi();

    initSettings();

    initWindow();

    // 中心窗口
    initContent();

    _messageDisplayDuration = _settings->value("basic/message_display_ms", 2000).toInt();

    this->onFunctionTabBarCurrentIndexChanged(0, -1);

    bool fullScreen = this->getSettings()->value("display/fullScreen", false).toBool();
    if (fullScreen)
        this->showFullScreen();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::isTabletMode()
{
    // 1) 能力检测: 设备是否存在触摸屏. 普通台式机/笔记本无触摸屏, 直接判定为 PC.
    bool hasTouchScreen = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const auto devices = QInputDevice::devices();
    for (const QInputDevice* dev : devices)
    {
        if (dev && dev->type() == QInputDevice::DeviceType::TouchScreen)
        {
            hasTouchScreen = true;
            break;
        }
    }
#else
    const auto devices = QTouchDevice::devices();
    for (const QTouchDevice* dev : devices)
    {
        if (dev && dev->type() == QTouchDevice::TouchScreen)
        {
            hasTouchScreen = true;
            break;
        }
    }
#endif
    if (!hasTouchScreen)
        return false;

#ifdef _WIN32
    // 2) 状态检测(Windows): 二合一设备翻转/拆下键盘进入 slate 平板模式时,
    //    SM_CONVERTIBLESLATEMODE 返回 0; 处于笔记本模式时返回非 0.
    //    纯平板设备无翻转能力时其值可能恒为 0, 故再用 SM_TABLETPC 兜底.
    const bool isSlateMode = (GetSystemMetrics(SM_CONVERTIBLESLATEMODE) == 0);
    const bool isTabletPc = (GetSystemMetrics(SM_TABLETPC) != 0);
    return isSlateMode || isTabletPc;
#else
    // 非 Windows 平台: 有触摸屏即按平板模式处理.
    return true;
#endif
}

std::shared_ptr<spdlog::logger> MainWindow::logger() const
{
    return _logger;
}

ModuleScript::ScriptController* MainWindow::getScriptController() const
{
    return _scriptController;
}

QSettings* MainWindow::getSettings() const
{
    return _settings;
}

std::shared_ptr<Mani4A> MainWindow::getMani4A() const
{
    return _mani4A;
}

MainWindow* MainWindow::getMainWindow()
{
    foreach(QWidget * widget, QApplication::topLevelWidgets())
    {
        MainWindow* mainWindow = qobject_cast<MainWindow*>(widget);
        if (mainWindow)
        {
            return mainWindow;
        }
    }
    return nullptr;
}

void MainWindow::initSPDLogger()
{
	std::string logDir = "logs";
	if (!std::filesystem::exists(logDir)) {
		std::filesystem::create_directory(logDir);
	}
    std::string logFileName = logDir + "/" + JUtils::Time::getCurrentTimeString() + ".txt";
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>(spdlog::color_mode::always);
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_st>(logFileName, 1024 * 1024 * 5, 3);
    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
    auto logger = std::make_shared<spdlog::logger>("multi_logger", sinks.begin(), sinks.end());
    std::string fomat = std::string("%^[%H:%M:%S]") + std::string("[%l][thread %t][%s:%#] %v%$");
    logger->set_pattern(fomat);
    logger->set_level(spdlog::level::debug);
    _logger = logger;
}

void MainWindow::initUi()
{
    _ui = new Ui::MainWindow();
    _ui->setupUi(this);

    _ui->tbFunctionArea->setOrientation(Qt::Vertical);
    _ui->tbFunctionArea->setItemSize(72);
	QFont f = _ui->tbFunctionArea->font();
    f.setPointSize(12);
    f.setBold(true);
    _ui->tbFunctionArea->setFont(f);
	_ui->tbFunctionArea->setIconSize(QSize(24, 24));
    _ui->tbFunctionArea->addTab(tr("自动"), QIcon(":/Palletizing/scannerCali.png"));
    _ui->tbFunctionArea->addTab(tr("手动"), QIcon(":/Palletizing/drag.png"));
    _ui->tbFunctionArea->addTab(tr("配置"), QIcon(":/Palletizing/Admin.png"));
	_ui->tbFunctionArea->setCurrentIndex(0);

    connect(_ui->tbFunctionArea, &TabBar::currentIndexChanged, this, &MainWindow::onFunctionTabBarCurrentIndexChanged);
}

void MainWindow::initWindow()
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowIcon(QIcon(":/Palletizing/Cirno.jpg"));
    QRect deskRect = QApplication::desktop()->availableGeometry();
    resize(deskRect.width() * 0.8, deskRect.height() * 0.8);
    setWindowTitle("Palletizing");
    //this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
}

void MainWindow::initContent()
{
    _autoPage = new AutoPage();
    _manualPage = new ManualPage();
    _settingsPage = new SettingsPage();
 
    _ui->stackCenter->addWidget(_autoPage);
    _ui->stackCenter->addWidget(_manualPage);
    _ui->stackCenter->addWidget(_settingsPage);
    _ui->stackCenter->setCurrentIndex(0);
}

bool MainWindow::initSettings()
{
    QString configPath = QCoreApplication::applicationDirPath() + "/Palletizing.ini";

    // 检查文件是否存在
    if (!QFile::exists(configPath))
    {
        // 创建默认配置
        QSettings defaultSettings(configPath, QSettings::IniFormat);

        defaultSettings.sync(); // 确保写入磁盘

        if (defaultSettings.status() != QSettings::NoError)
        {
            return false;
        }
    }

    // 创建 QSettings 对象
    _settings = new QSettings(configPath, QSettings::IniFormat);

    // 检查读取状态
    if (_settings->status() != QSettings::NoError)
    {
        // 可以选择：
        // 1. 删除损坏的文件并重新创建
        // 2. 继续使用（QSettings 会返回默认值）
        // 3. 退出程序

        if (_settings->status() == QSettings::FormatError)
        {
            // 备份旧文件
            QString backupPath = configPath + ".backup";
            QFile::remove(backupPath);
            QFile::copy(configPath, backupPath);

            // 删除损坏的文件
            QFile::remove(configPath);

            // 递归调用重新初始化
            delete _settings;
            _settings = nullptr;
            initSettings();
            return false;
        }
    }

    return true;
}

void MainWindow::initModules()
{
    _mani4A = std::make_shared<Mani4A>();
    _scriptController = new ScriptController(this);

    connect(_scriptController, &ScriptController::scriptStarted, this, &MainWindow::onScriptStarted);
    connect(_scriptController, &ScriptController::scriptFinished, this, &MainWindow::onScriptFinished);
    connect(_scriptController, &ScriptController::scriptLineChanged, this, &MainWindow::onScriptLineChanged);
    connect(_scriptController, &ScriptController::scriptErrorOccurred, this, &MainWindow::onScriptErrorOccurred);
}

void MainWindow::onScriptStarted()
{
    _ui->tbFunctionArea->setEnabled(false);
}

void MainWindow::onScriptFinished(const QString& errorMsg)
{
    _ui->tbFunctionArea->setEnabled(true);
}

void MainWindow::onScriptLineChanged(int line)
{
    
}

void MainWindow::onScriptErrorOccurred(const QString& errorMsg)
{
	this->appendLogMessage(errorMsg, 2);
}

void MainWindow::onSettingsChanged()
{
    
}

void MainWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    static bool firstShow = true;
    if (!firstShow)
        return;

    firstShow = false;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::systemInit);
    timer->setSingleShot(true);
    timer->setInterval(100);
    timer->start();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    _settingsPage->saveToIniFile();

    QWidget::closeEvent(event);
}

void MainWindow::appendLogMessage(const QString& message, int level)
{
    if (level == 0)
        ElaMessageBar::success(ElaMessageBarType::BottomRight, tr("Success"), message, _messageDisplayDuration, this);
    else if (level == 1)
        ElaMessageBar::warning(ElaMessageBarType::BottomRight, tr("Warning"), message, _messageDisplayDuration, this);
    else if (level == 2)
        ElaMessageBar::error(ElaMessageBarType::BottomRight, tr("Error"), message, _messageDisplayDuration, this);
}

void MainWindow::systemInit()
{
    if (int err = _mani4A->open())
    {
        this->appendLogMessage(tr("控制器打开失败，错误码: %1").arg(err), 2);
        return;
    }
    this->appendLogMessage(tr("控制器打开成功"), 0);

    emit systemInitialized();
}

void MainWindow::systemDeInit()
{
    _mani4A->close();
}

void MainWindow::onFunctionTabBarCurrentIndexChanged(int index, int preIndex)
{
    if (preIndex == 2)
    {
        _settingsPage->saveToIniFile();
    }

    _ui->stackCenter->setCurrentIndex(index);
}