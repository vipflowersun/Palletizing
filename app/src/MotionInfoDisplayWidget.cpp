#include "MotionInfoDisplayWidget.h"

#include <QAbstractItemView>
#include <QColor>
#include <QMetaObject>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStandardItemModel>

namespace Palletizing
{
	MotionInfoDisplayWidget::MotionInfoDisplayWidget(QWidget* parent)
		: ElaListView(parent)
		, _model(new QStandardItemModel(this))
		, _scrollAnimation(new QPropertyAnimation(verticalScrollBar(), "value", this))
	{
		setModel(_model);
		setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setSelectionMode(QAbstractItemView::NoSelection);
		setEditTriggers(QAbstractItemView::NoEditTriggers);
		setFocusPolicy(Qt::NoFocus);
		setUniformItemSizes(true);
		setSpacing(2);
		QFont listFont = font();
		listFont.setPointSize(18);
		setFont(listFont);
		updateItemHeight();

		_scrollAnimation->setDuration(220);
		_scrollAnimation->setEasingCurve(QEasingCurve::OutCubic);
	}

	void MotionInfoDisplayWidget::updateItemStyle()
	{
		for (int row = 0; row < _model->rowCount(); ++row)
		{
			QStandardItem* item = _model->item(row);
			const bool isCurrentMotion = row == _model->rowCount() - 1;
			item->setBackground(isCurrentMotion ? QColor(45, 125, 220) : QColor(235, 240, 247));
			item->setForeground(isCurrentMotion ? QColor(Qt::white) : QColor(45, 55, 70));
		}
	}

	void MotionInfoDisplayWidget::updateItemHeight()
	{
		const int itemCount = qMax(1, _maxDisplayCount);
		const int availableHeight = viewport()->height();
		const int totalSpacing = spacing() * (itemCount - 1);
		const int itemHeight = qMax(1, (availableHeight - totalSpacing) / itemCount);
		setItemHeight(itemHeight);
	}

	void MotionInfoDisplayWidget::resizeEvent(QResizeEvent* event)
	{
		ElaListView::resizeEvent(event);
		updateItemHeight();
	}

	void MotionInfoDisplayWidget::updateMotionInfo(const QString& motionInfo)
	{
		_model->appendRow(new QStandardItem(motionInfo));

		while (_model->rowCount() > _maxDisplayCount)
		{
			delete _model->takeRow(0).value(0);
		}

		updateItemStyle();
		scrollToBottomAnimated();
	}

	void MotionInfoDisplayWidget::scrollToBottomAnimated()
	{
		QMetaObject::invokeMethod(this, [this]()
		{
			QScrollBar* scrollBar = verticalScrollBar();
			const int targetValue = scrollBar->maximum();
			_scrollAnimation->stop();
			if (scrollBar->value() == targetValue)
			{
				return;
			}

			_scrollAnimation->setStartValue(scrollBar->value());
			_scrollAnimation->setEndValue(targetValue);
			_scrollAnimation->start();
		}, Qt::QueuedConnection);
	}
}
