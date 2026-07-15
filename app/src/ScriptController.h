#pragma once
#include <QObject>

#include <mutex>

#include "Palletizer.h"
class LuaManager;

namespace ModuleScript
{
    class ScriptController : public QObject
    {
        Q_OBJECT
    public:
        explicit ScriptController(QObject* parent = nullptr) noexcept;
        virtual ~ScriptController();
    public:
        std::weak_ptr<LuaManager> luaManager() const;
        void pushError(const std::string& error);
        void clearError();
        std::list<std::string> getError();
        std::vector<std::string> luaRegFunctions() const;
        bool runScript(const std::string& scrip);
        void terminateScript();
        void pauseScript();
        void resumeScript();
		bool isScriptRunning() const;
		int getScriptTotalLines() const;
    private:
        void registLuaCommands();
    signals:
        void scriptStarted();
        void scriptFinished(const QString& errorMsg);
		void scriptLineChanged(int line);
		void scriptErrorOccurred(const QString& errorMsg);
		void motionExecuting(const QString& motionInfo);
		void palletizingUpdated(const Palletizer::PalletizingOutput& output);
    private:
        std::shared_ptr<LuaManager> _luaMgr;
        std::list<std::string> _errors;
        std::mutex _mutexErrors;
    };
}
