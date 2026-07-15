#ifndef ELACONFIRMDIALOG_H
#define ELACONFIRMDIALOG_H
#include <QAbstractNativeEventFilter>
#include <QDialog>

#include "ElaAppBar.h"
#include "ElaProperty.h"
class ElaConfirmDialogPrivate;
class ELA_EXPORT ElaConfirmDialog : public QDialog
{
    Q_OBJECT
        Q_Q_CREATE(ElaConfirmDialog)
        Q_TAKEOVER_NATIVEEVENT_H
public:
    explicit ElaConfirmDialog(QWidget* parent);
    ~ElaConfirmDialog() override;
    Q_SLOT virtual void onLeftButtonClicked();
    Q_SLOT virtual void onRightButtonClicked();
    void setCentralWidget(QWidget* centralWidget);
	void setTitleText(const QString& title);
	void setSubTitleText(const QString& subTitle);
    void setLeftButtonText(QString text);
    void setRightButtonText(QString text);

    void close();
Q_SIGNALS:
    Q_SIGNAL void leftButtonClicked();
    Q_SIGNAL void rightButtonClicked();

protected:
    virtual void showEvent(QShowEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
};

#endif // ELACONFIRMDIALOG_H
