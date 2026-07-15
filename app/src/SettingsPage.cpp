#include "SettingsPage.h"
#include "MainWindow.h"

#include "ElaText.h"
#include "ElaComboBox.h"
#include "ElaLineEdit.h"
#include "ElaScrollArea.h"
#include "ElaToggleSwitch.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QBoxLayout>

using namespace Palletizing;

SettingsPage::SettingsPage(QWidget* parent) : ElaScrollPage(parent)
{
	this->setTitleVisible(false);
	ElaScrollArea* contentWidget = new ElaScrollArea(this);
	QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
	contentLayout->setSpacing(32);
	QGroupBox* grpBox = initBasicGroupBox();
	contentLayout->addWidget(grpBox);
	this->addCentralWidget(contentWidget, true, true, 0);

	this->readFromIniFile();
}

SettingsPage::~SettingsPage()
{
}

void SettingsPage::readFromIniFile()
{
	auto settings = MainWindow::getMainWindow()->getSettings();

	int boxLength = settings->value("basic/boxLength", 100).toInt();
	_editBoxLength->setText(QString::number(boxLength));
	int boxWidth = settings->value("basic/boxWidth", 100).toInt();
	_editBoxWidth->setText(QString::number(boxWidth));
	int boxHeight = settings->value("basic/boxHeight", 100).toInt();
	_editBoxHeight->setText(QString::number(boxHeight));
	int plateLength = settings->value("basic/plateLength", 100).toInt();
	_editPlateLength->setText(QString::number(plateLength));
	int plateWidth = settings->value("basic/plateWidth", 100).toInt();
	_editPlateWidth->setText(QString::number(plateWidth));
}

bool SettingsPage::saveToIniFile()
{
	auto settings = MainWindow::getMainWindow()->getSettings();

	settings->setValue("basic/boxLength", _editBoxLength->text().toInt());
	settings->setValue("basic/boxWidth", _editBoxWidth->text().toInt());
	settings->setValue("basic/boxHeight", _editBoxHeight->text().toInt());
	settings->setValue("basic/plateLength", _editPlateLength->text().toInt());
	settings->setValue("basic/plateWidth", _editPlateWidth->text().toInt());

	settings->sync();

	emit settingsChanged();

	return true;
}

QGroupBox* SettingsPage::initBasicGroupBox()
{
	QGroupBox* grpBox = new QGroupBox(this);
	grpBox->setFont(QFont("Microsoft YaHei", 12));
	grpBox->setTitle(tr("基础设置"));
	QFormLayout* grpBoxLayout = new QFormLayout(grpBox);
	grpBoxLayout->setSpacing(80);
	grpBoxLayout->setHorizontalSpacing(80);
	grpBoxLayout->setVerticalSpacing(24);
	ElaText* lb = new ElaText(tr("箱体长度(mm)"), this);
	lb->setTextPixelSize(16);
	_editBoxLength = new ElaLineEdit(this);
	_editBoxLength->setMaximumWidth(120);
	_editBoxLength->setIsClearButtonEnable(false);
	_editBoxLength->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_editBoxLength->setText("0");
	grpBoxLayout->addRow(lb, _editBoxLength);
	lb = new ElaText(tr("箱体宽度(mm)"), this);
	lb->setTextPixelSize(16);
	_editBoxWidth = new ElaLineEdit(this);
	_editBoxWidth->setMaximumWidth(120);
	_editBoxWidth->setIsClearButtonEnable(false);
	_editBoxWidth->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_editBoxWidth->setText("0");
	grpBoxLayout->addRow(lb, _editBoxWidth);
	lb = new ElaText(tr("箱体高度(mm)"), this);
	lb->setTextPixelSize(16);
	_editBoxHeight = new ElaLineEdit(this);
	_editBoxHeight->setMaximumWidth(120);
	_editBoxHeight->setIsClearButtonEnable(false);
	_editBoxHeight->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_editBoxHeight->setText("0");
	grpBoxLayout->addRow(lb, _editBoxHeight);
	lb = new ElaText(tr("托盘长度(mm)"), this);
	lb->setTextPixelSize(16);
	_editPlateLength = new ElaLineEdit(this);
	_editPlateLength->setMaximumWidth(120);
	_editPlateLength->setIsClearButtonEnable(false);
	_editPlateLength->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_editPlateLength->setText("0");
	grpBoxLayout->addRow(lb, _editPlateLength);
	lb = new ElaText(tr("托盘宽度(mm)"), this);
	lb->setTextPixelSize(16);
	_editPlateWidth = new ElaLineEdit(this);
	_editPlateWidth->setMaximumWidth(120);
	_editPlateWidth->setIsClearButtonEnable(false);
	_editPlateWidth->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	_editPlateWidth->setText("0");
	grpBoxLayout->addRow(lb, _editPlateWidth);

	return grpBox;
}