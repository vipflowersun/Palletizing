#include "ScriptController.h"
#include "LuaFunction.h"
#include "LuaManager.h"

#include "spdlog/spdlog.h"

#include <QDebug>

using namespace ModuleScript;

ScriptController::ScriptController(QObject* parent) noexcept : QObject(parent)
{
	_luaMgr = std::make_shared<LuaManager>();
	_luaMgr->setLineHook([this](int line) {
		emit scriptLineChanged(line);
	});
	_luaMgr->setScriptStartHook([this]() {
		emit scriptStarted();
	});
	_luaMgr->setScriptFinishHook([this](const std::string& error) {
		if (!error.empty())
			SPDLOG_DEBUG(error);
		emit scriptFinished(QString::fromStdString(error));
	});

	_luaMgr->setUserData("scriptController", this);

	this->registLuaCommands();
}

ScriptController::~ScriptController()
{
	_luaMgr->terminateScript();
	while (_luaMgr->isRunning())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

std::weak_ptr<LuaManager> ScriptController::luaManager() const
{
	return _luaMgr;
}

void ScriptController::pushError(const std::string& error)
{
	std::lock_guard<std::mutex> lock(_mutexErrors);
	_errors.push_back(error);
	if (_errors.size() > 100)
		_errors.pop_front();
}

void ScriptController::clearError()
{
	std::lock_guard<std::mutex> lock(_mutexErrors);
	_errors = std::list<std::string>();
}

std::list<std::string> ScriptController::getError()
{
	std::lock_guard<std::mutex> lock(_mutexErrors);
	return _errors;
}

void ScriptController::registLuaCommands()
{
	_luaMgr->registerCommand("sleep", LuaFunction::sleep);
	_luaMgr->registerCommand("print", LuaFunction::print);
	_luaMgr->registerCommand("plan", LuaFunction::plan);
	_luaMgr->registerCommand("palletLevel", LuaFunction::palletLevel);
	_luaMgr->registerCommand("getTotalLevel", LuaFunction::getTotalLevel);
}

std::vector<std::string> ScriptController::luaRegFunctions() const
{
	return _luaMgr->commands();
}

bool ScriptController::runScript(const std::string& scrip)
{
	_luaMgr->setScriptBasePath("./scripts");

	if (_luaMgr->loadScript(scrip))
		return false;

	if (_luaMgr->excuteScript())
		return false;
	
	return true;
}

void ScriptController::terminateScript()
{
	_luaMgr->terminateScript();
}

void ScriptController::pauseScript()
{
	_luaMgr->pauseScript();
}

void ScriptController::resumeScript()
{
	_luaMgr->resumeScript();
}

bool ScriptController::isScriptRunning() const
{
	return _luaMgr->isRunning();
}

int ScriptController::getScriptTotalLines() const
{
	return _luaMgr->getScriptTotalLines();
}
