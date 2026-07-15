#pragma once
#include <QObject>
#include <string>
#include <memory>
#include "ModuleAbstract_export.h"
namespace spdlog {
	class logger;
};

class MODULEABSTRACT_EXPORT ModuleAbstract : public QObject
{
	Q_OBJECT
protected:
	explicit ModuleAbstract(QObject* parent = nullptr) noexcept;
	virtual ~ModuleAbstract() = default;
public:
	std::shared_ptr<spdlog::logger> logger() const { return _logger; }
	void setLogger(const std::shared_ptr<spdlog::logger>& logger) { _logger = logger; }
public:
	virtual std::string moduleName() = 0;
	virtual std::string moduleVersion() = 0;
signals:
	void logRequested(const QString& message, int level);  //level: 0-info, 1-warning, 2-error
protected:
	std::shared_ptr<spdlog::logger> _logger;
};