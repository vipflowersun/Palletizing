#ifndef ELAWAITINGDIALOG_H
#define ELAWAITINGDIALOG_H
#include <QAbstractNativeEventFilter>
#include <QDialog>

#include "ElaAppBar.h"
#include "ElaProperty.h"
class ElaWaitingDialogPrivate;
class ELA_EXPORT ElaWaitingDialog : public QDialog
{
    Q_OBJECT
        Q_Q_CREATE(ElaWaitingDialog)
        Q_TAKEOVER_NATIVEEVENT_H
public:
    explicit ElaWaitingDialog(QWidget* parent);
    ~ElaWaitingDialog() override;
    Q_SLOT virtual void onCancelButtonClicked();
    void setCentralWidget(QWidget* centralWidget);
    void setTitleText(const QString& title);
	void setCancelButtonShow(bool show);
    void setCancelButtonText(QString text);
	void setMaskEnabled(bool enabled);
    void setIsBusying(bool busying);
	void setMaximumValue(double value);
	void setMinimumValue(double value);
    Q_SLOT void setValue(double value);

    void close();
Q_SIGNALS:
    Q_SIGNAL void cancelButtonClicked();

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
};

#endif // ELAWAITINGDIALOG_H
