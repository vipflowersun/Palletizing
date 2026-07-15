#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <map>
#include <any>
struct lua_State;
class LuaManagerPrivate;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef LUAIDE_EXPORTS
// 仅当构建为 DLL 时导出符号
#define LUAIDE_API __declspec(dllexport)
#else
// 静态库或客户端使用时，不使用 dllimport，避免静态库链接失败
#define LUAIDE_API
#endif
#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define LUAIDE_API __attribute__((visibility("default")))
#else
#define LUAIDE_API
#endif
#endif

class LUAIDE_API LuaManager
{
public:
	explicit LuaManager();
	bool isRunning();
	int loadScript(const std::string &script);
	int excuteScript();
	void terminateScript();
	bool getTerminateFlag() const;
	void pauseScript();
	void resumeScript();
	void setStepMode(bool stepMode);
	int getInputTopCount();
	void clearInputStack();
	double getInputDouble(int pos);
	int getInputInt(int pos);
	std::string getInputString(int pos);
	std::map<std::string, std::any> getInputTable(int pos);
	std::vector<std::any> getInputArray(int pos);
	void pushOutputDouble(double num);
	void pushOutputInt(int num);
	void pushOutputString(const std::string str);
	void pushOutputTable(const std::map<std::string, std::any> &table);
	void pushOutputArray(const std::vector<std::any>& array);
	void registerCommand(const std::string &cmdStr, int (*cmd)(lua_State *));
	std::vector<std::string> commands();
	void setLineHook(std::function<void(int)> hook);
	void setScriptStartHook(std::function<void(void)> hook);
	void setScriptFinishHook(std::function<void(const std::string &erro)> hook);
	void insertBreakPoint(int line);
	void removeBreakPoint(int line);
	void pushNilToStack();
	int tableStackNext(int index);
	bool isStackNumber(int index);
	bool isStackString(int index);
	bool isStackTable(int index);
	bool isStackArray(int index);
	void popStack(int index);
	int getCurrentLine();
	void preloadGlobalString(const std::string &key, const std::string &value);
	void preloadGlobalNumber(const std::string &key, double value);
	bool getGlobalString(const std::string& key, std::string& value);
	bool getGlobalNumber(const std::string& key, double& value);
	void setScriptBasePath(const std::string &basePath);
	void setUserData(const std::string key, const std::any &data);
	int getScriptTotalLines() const;
	// 仅能在lua注册函数中调用
	static std::any userData(lua_State *L, const std::string key);

private:
	std::shared_ptr<LuaManagerPrivate> _prv;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif