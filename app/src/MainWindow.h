#pragma once
#include <QWidget>
#include <QSettings>
namespace Ui {
    class MainWindow;
}
namespace spdlog {
    class logger;
};
namespace ModuleScript {
    class ScriptController;
}

namespace Palletizing
{
    class AutoPage;
	class ManualPage;
    class SettingsPage;
    class Mani4A;
    class MainWindow : public QWidget
    {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;
    public:
        static bool isTabletMode();
        static MainWindow* getMainWindow();
        std::shared_ptr<spdlog::logger> logger() const;
        ModuleScript::ScriptController* getScriptController() const;
        QSettings* getSettings() const;
        std::shared_ptr<Mani4A> getMani4A() const;
    public slots:
		//0: success, 1: warning, 2: error
        void appendLogMessage(const QString& message, int level);
    private:
        void initSPDLogger();
        void initUi();
        void initWindow();
        void initContent();
        bool initSettings();  // 初始化配置文件
        void initModules();
    private slots:
        void systemInit();
        void systemDeInit();
        void onFunctionTabBarCurrentIndexChanged(int index, int preIndex);
        /*脚本事件处理*/
        void onScriptStarted();
        void onScriptFinished(const QString& errorMsg);
		void onScriptLineChanged(int line);
        void onScriptErrorOccurred(const QString& errorMsg);
        /*设置事件处理*/
		void onSettingsChanged();
	protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual void closeEvent(QCloseEvent* event) override;
    signals:
        void systemInitialized();
    private:
        Ui::MainWindow* _ui{ nullptr };
        std::shared_ptr<spdlog::logger> _logger;
        QSettings* _settings{ nullptr };
        int _messageDisplayDuration{ 1000 };
		AutoPage* _autoPage{ nullptr };
		ManualPage* _manualPage{ nullptr };
        SettingsPage* _settingsPage{ nullptr };
		std::shared_ptr<Mani4A> _mani4A{ nullptr };
        ModuleScript::ScriptController* _scriptController{ nullptr };
    };
}