#pragma once
#include <QWidget>
class CircleLedWidget;

namespace Palletizing
{
	class MonitorWidget : public QWidget
	{
		Q_OBJECT
	public:
		explicit MonitorWidget(QWidget* parent = nullptr);
		~MonitorWidget() = default;
	private slots:
		void updateStatus();
	private:
		std::vector<CircleLedWidget*> _leds;
	};
}