#pragma once
#include "ElaScrollPage.h"
class ElaComboBox;
class ElaLineEdit;
class QGroupBox;
class ElaToggleSwitch;

namespace Palletizing
{
	class SettingsPage : public ElaScrollPage
	{
		Q_OBJECT
	public:
		explicit SettingsPage(QWidget* parent = nullptr);
		~SettingsPage() override;
	public:
		void readFromIniFile();
		bool saveToIniFile();
	private:
		QGroupBox* initBasicGroupBox();
	signals:
		void settingsChanged();
	private:
		ElaLineEdit* _editBoxLength{ nullptr };
		ElaLineEdit* _editBoxWidth{ nullptr };
		ElaLineEdit* _editBoxHeight{ nullptr };
		ElaLineEdit* _editPlateLength{ nullptr };
		ElaLineEdit* _editPlateWidth{ nullptr };
	};
}