#pragma once
#include "ModuleComponentWidget_export.h"
#include <queue>
#include <limits>
#include <optional>

#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QScrollArea>
#include <QLineEdit>
class QPropertyAnimation;

class MODULECOMPONENTWIDGET_EXPORT LabelErrorCode : public QLabel
{
	Q_OBJECT
public:
	explicit LabelErrorCode(QWidget *parent = nullptr) noexcept;
	void setInactive();
	void setWarning();
	void setMaunal();
	void setAuto();
	void setStop();
	void setError();

protected:
	virtual void paintEvent(QPaintEvent *event) override;
};

class MODULECOMPONENTWIDGET_EXPORT FramelessButton : public QPushButton
{
	Q_OBJECT
public:
	explicit FramelessButton(QWidget *parent = nullptr);
	virtual ~FramelessButton() noexcept = default;
};

class MODULECOMPONENTWIDGET_EXPORT ColorButton : public QPushButton
{
	Q_OBJECT
public:
	explicit ColorButton(QWidget *parent = nullptr);
	void setColor(const QColor &color);
};

class MODULECOMPONENTWIDGET_EXPORT ColorLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ColorLabel(QWidget* parent = nullptr);
	void setColor(const QColor& color);
};

class MODULECOMPONENTWIDGET_EXPORT ToggleSwitch : public QWidget
{
	Q_OBJECT
public:
	explicit ToggleSwitch(QWidget *parent = nullptr);
	void setChecked(bool checked);
	bool isChecked() const;
	void setCheckedColor(const QColor& color);
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)
    qreal progress() const;
    void setProgress(qreal value);
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
signals:
	void checkedChanged(bool checked);
private:
	bool _checked = false;
	QColor _checkedColor = QColor(64, 158, 255);
    qreal _progress = 0.0;
    QPropertyAnimation* _anim = nullptr;
};

class MODULECOMPONENTWIDGET_EXPORT TabBar : public QWidget
{
	Q_OBJECT
public:
	explicit TabBar(QWidget* parent = nullptr);
	explicit TabBar(Qt::Orientation orient, QWidget *parent = nullptr);
	void setOrientation(Qt::Orientation orient);
	void setIconBeside(bool beside);
	void setItemSize(int size);
	int getItemSize() const;
	void setIconSize(const QSize& size);
	void setItemSpacing(int spacing);
	void setBackgroundColor(const QColor& color);
	void setCheckedColor(const QColor& color);
	void addTab(const QString &text, const QIcon& icon);
	void addFootTab(const QString& text, const QIcon& icon);
	void setCurrentIndex(int index);
	int currentIndex() const;
signals:
	void currentIndexChanged(int index, int previousIndex);
protected:
	virtual void changeEvent(QEvent* event) override;
private:
	Qt::Orientation _orient { Qt::Horizontal };
	int _itemSize = 120;
	int _itemSpacing = 8;
	QSize _iconSize = QSize(32, 32);
	bool _iconBeside = false;
};

class MODULECOMPONENTWIDGET_EXPORT TabBarItem : public QToolButton
{
	Q_OBJECT
public:
	explicit TabBarItem(Qt::Orientation orient, QWidget* parent = nullptr);
	void setOrientation(Qt::Orientation orient);
	void setIconBeside(bool beside);
	void setCheckedColor(const QColor& color);
	void setItemSpacing(int spacing);
protected:		
	virtual void paintEvent(QPaintEvent* event) override;
private:
	QString makeStyleSheet() const;
private:
	Qt::Orientation _orient;
	int _itemSpacing = 4;
	bool _iconBeside = false;
	QColor _checkedColor = QColor(64, 158, 255);
};

class MODULECOMPONENTWIDGET_EXPORT ScriptIndicateButton : public QPushButton
{
	Q_OBJECT
public:
	explicit ScriptIndicateButton(QWidget *parent);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
};

class MODULECOMPONENTWIDGET_EXPORT FlowLabel : public QLabel
{
	Q_OBJECT
public:
	explicit FlowLabel(QWidget *parent);
public slots:
	void startFlow();
	void stopFlow();

protected:
	virtual void paintEvent(QPaintEvent *event) override;

private:
	int _stripeOffset = 0;
};

class MODULECOMPONENTWIDGET_EXPORT FifoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit FifoWidget(QWidget *parent = nullptr);
	~FifoWidget();
	void setMaxPartCount(int count);
	void setFrameColor(const QColor &color);
	void setFrameRadius(int radius);
	void setBackgroundColor(const QColor& color);
public slots:
	void pop();
	void push(QWidget* wid);
	void clear();

private:
	QSize calcPartSize() const;
	void resetPartsGeomoetry();

protected:
	void paintEvent(QPaintEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	int _indexLast = 0;
	int _count = 0;
	int _maxPartCount = 5;
	QColor _rectColor = Qt::black;
	QColor _backgroundColor = Qt::white;
	int _frameRadius = 8;
	const int _margin = 8;
	const int _animationDuration = 400;
	QVector<QWidget*> _waitingWiget;
};

class MODULECOMPONENTWIDGET_EXPORT CircleLedWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CircleLedWidget(QWidget *parent = nullptr);

public:
	void setOn(bool on);
	bool isOn() const;
	void setColor(const QColor &color);
	void setClickable(bool clickable);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	virtual void enterEvent(QEnterEvent *event) override;
#else
	virtual void enterEvent(QEvent *event) override;
#endif
	virtual void leaveEvent(QEvent *event) override;
signals:
	void clicked(bool on);

protected:
	bool _on = false;
	QColor _color = Qt::green;
	bool _clickable = false;
};

class MODULECOMPONENTWIDGET_EXPORT RectLedWidget : public CircleLedWidget
{
	Q_OBJECT
public:
	explicit RectLedWidget(QWidget* parent = nullptr);
protected:
	virtual void paintEvent(QPaintEvent* event) override;
};

class MODULECOMPONENTWIDGET_EXPORT IOWidget : public QScrollArea
{
	Q_OBJECT
public:
	explicit IOWidget(QWidget *parent = nullptr);
	void setIOResCount(int diCount, int doCount);
	void setDIName(int index, const QString &name);
	void setDOName(int index, const QString &name);
	void setDI(uint64_t DI);
	void setDO(uint64_t DO);
signals:
	void DOClicked(int index, bool on);

private:
	std::vector<CircleLedWidget *> _diLeds;
	std::vector<CircleLedWidget *> _doLeds;
};

class MODULECOMPONENTWIDGET_EXPORT ProgressLable : public QLabel
{
	Q_OBJECT
public:
	explicit ProgressLable(QWidget *parent = nullptr);
	void setValue(double value, const QString& unit);
	void setMinMax(double min, double max);
	void setAbs(bool abs);
protected:
	virtual void paintEvent(QPaintEvent* event) override;
private:
	double _min = std::numeric_limits<double>::quiet_NaN();
	double _max = std::numeric_limits<double>::quiet_NaN();
	std::optional<double> _value;
	bool _abs = false;
};

class MODULECOMPONENTWIDGET_EXPORT PackedWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PackedWidget(QWidget *parent = nullptr);
	void setItemSize(int size);
	void setItemFixedWidth(bool enable, int width);
	void setItemWidget(QWidget* widget);
	void setText(const QString& text);
	void setIcon(const QIcon& icon);
	void setIconSize(const QSize& size);
	void setBackgroundColor(const QColor& color);
protected:
	virtual void paintEvent(QPaintEvent* event) override;
private:
	int _itemSize = 32;
	QIcon _icon;
	QSize _iconSize = QSize(0, 0);
	QColor _backgroundColor = Qt::lightGray;
	bool _fixedWidth = false;
	int _fixedWidthValue = 0;
};

class MODULECOMPONENTWIDGET_EXPORT SelectableLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	explicit SelectableLineEdit(QWidget* parent = nullptr);
	bool isSelected() const;
	void setSelected(bool b);
protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
signals:
	void selected(bool select);
private:
	bool _isSelected = false;
};
