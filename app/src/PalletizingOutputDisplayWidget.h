#pragma once
#include <QWidget>
#include "Palletizer.h"

Q_DECLARE_METATYPE(Palletizer::PalletizingOutput)

namespace Palletizing
{
	class PalletizingOutputDisplayWidget : public QWidget
	{
		Q_OBJECT
	public:
		explicit PalletizingOutputDisplayWidget(QWidget* parent = nullptr);
		~PalletizingOutputDisplayWidget() = default;
	public slots:
		void setPalletizingOutput(double palletLength, double palletWidth, const Palletizer::PalletizingOutput& output);

	protected:
		void paintEvent(QPaintEvent* event) override;

	private:
		double m_palletLength = 0.0;
		double m_palletWidth = 0.0;
		Palletizer::PalletizingOutput m_output;
	};
}
