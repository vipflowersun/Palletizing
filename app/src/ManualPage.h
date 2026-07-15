#pragma once
#include "ElaScrollPage.h"

#include <QString>

namespace Palletizing
{
	class ManualPage : public ElaScrollPage
	{
		Q_OBJECT
	public:
		explicit ManualPage(QWidget* parent = nullptr);
		~ManualPage() = default;
	Q_SIGNALS:
		void jogStarted(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed);
		void jogStopped(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed);
		void gripperToggled(int robot, int gripper, bool opened);
		void unloadingPointRecordRequested();
	public slots:
		void onLoadingPointRecordRequested1();
		void onUnloadingPointRecordRequested1();
		void onLoadingPointRecordRequested2();
		void onUnloadingPointRecordRequested2();
		void onJogStarted(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed);
		void onJogStopped(int robot, const QString& axis, bool positive, bool cartesian, bool highSpeed);
		void onGripperToggled(int robot, int gripper, bool opened);
	};
}