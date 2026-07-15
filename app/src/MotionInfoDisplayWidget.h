#pragma once
#include "ElaListView.h"

class QPropertyAnimation;
class QResizeEvent;
class QStandardItemModel;

namespace Palletizing
{
	class MotionInfoDisplayWidget : public ElaListView
	{
		Q_OBJECT
	public:
		explicit MotionInfoDisplayWidget(QWidget* parent = nullptr);
		~MotionInfoDisplayWidget() = default;
	public slots:
		void updateMotionInfo(const QString& motionInfo);
	private:
		void updateItemStyle();
		void updateItemHeight();
		void scrollToBottomAnimated();
		void resizeEvent(QResizeEvent* event) override;

		int _maxDisplayCount{ 1 };
		QStandardItemModel* _model{ nullptr };
		QPropertyAnimation* _scrollAnimation{ nullptr };
	};
}