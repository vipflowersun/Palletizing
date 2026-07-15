#include "PalletizingOutputDisplayWidget.h"

#include <QPainter>
#include <QPaintEvent>
#include <algorithm>

namespace Palletizing
{
	PalletizingOutputDisplayWidget::PalletizingOutputDisplayWidget(QWidget* parent)
		: QWidget(parent)
	{
		qRegisterMetaType<Palletizer::PalletizingOutput>("Palletizer::PalletizingOutput");
		setAutoFillBackground(true);
		setMinimumSize(120, 120);
	}

	void PalletizingOutputDisplayWidget::setPalletizingOutput(
		double palletLength, double palletWidth, const Palletizer::PalletizingOutput& output)
	{
		m_palletLength = palletLength;
		m_palletWidth = palletWidth;
		m_output = output;
		update();
	}

	void PalletizingOutputDisplayWidget::paintEvent(QPaintEvent* event)
	{
		Q_UNUSED(event);

		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.fillRect(rect(), palette().brush(QPalette::Base));

		if (m_palletLength <= 0.0 || m_palletWidth <= 0.0)
		{
			return;
		}

		const int margin = 12;
		const double availableWidth = std::max(0, width() - margin * 2);
		const double availableHeight = std::max(0, height() - margin * 2);
		const double scale = std::min(availableWidth / m_palletLength, availableHeight / m_palletWidth);
		if (scale <= 0.0)
		{
			return;
		}

		const double palletPixelWidth = m_palletLength * scale;
		const double palletPixelHeight = m_palletWidth * scale;
		const double originX = (width() - palletPixelWidth) / 2.0;
		const double originY = (height() - palletPixelHeight) / 2.0;

		painter.save();
		painter.translate(originX, originY);
		painter.scale(scale, scale);

		QPen palletPen(QColor(45, 45, 45));
		palletPen.setWidthF(std::max(2.0, 3.0 / scale));
		painter.setPen(palletPen);
		painter.setBrush(Qt::NoBrush);
		painter.drawRect(QRectF(0.0, 0.0, m_palletLength, m_palletWidth));

		if (m_output.success)
		{
			const double stackOffsetX = (m_palletLength - static_cast<double>(m_output.occupiedLength)) / 2.0;
			const double stackOffsetY = (m_palletWidth - static_cast<double>(m_output.occupiedWidth)) / 2.0;

			QPen boxPen(QColor(35, 85, 135));
			boxPen.setWidthF(std::max(1.0, 1.5 / scale));
			painter.setPen(boxPen);
			painter.setBrush(QColor(135, 190, 235, 180));

			for (const Palletizer::BoxPlacement& placement : m_output.placements)
			{
				if (placement.length <= 0 || placement.width <= 0)
				{
					continue;
				}

				const double left = stackOffsetX + placement.centerX - static_cast<double>(placement.length) / 2.0;
				const double top = stackOffsetY + placement.centerY - static_cast<double>(placement.width) / 2.0;
				painter.drawRect(QRectF(left, top,
					static_cast<double>(placement.length), static_cast<double>(placement.width)));
			}
		}

		painter.restore();
	}
}
