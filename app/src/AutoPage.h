#pragma once
#include <QWidget>
#include "Palletizer.h"
class QPushButton;
namespace Ui {
	class AutoPage;
}

namespace Palletizing
{
	class MotionInfoDisplayWidget;
	class AutoPage : public QWidget
	{
		Q_OBJECT
	public:
		explicit AutoPage(QWidget* parent = nullptr);
		~AutoPage() = default;
	private slots:
		void onScriptStarted();
		void onScriptFinished(const QString& errorMsg);
		void onScriptMotionExecuting(const QString& motionInfo);
		void onBtnStart();
		void onBtnStop();
		void onPalletizingUpdated(const Palletizer::PalletizingOutput& output);
	private:
		/*QPushButton* _btnStart{ nullptr };
		QPushButton* _btnStop{ nullptr };
		MotionInfoDisplayWidget* _motionInfoDisplay{ nullptr };*/
		Ui::AutoPage* _ui{ nullptr };
	};
}