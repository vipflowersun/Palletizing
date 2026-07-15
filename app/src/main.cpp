#include <QApplication>
#include <QScreen>
#include <QProcess>
#include <QMessageBox>

#include <filesystem>

#include "MainWindow.h"

static QtMessageHandler s_defaultHandler = nullptr;

static void customMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // 过滤 QWindowsWindow::setGeometry 警告
    if (type == QtWarningMsg && msg.contains("QWindowsWindow::setGeometry"))
        return;

    if (s_defaultHandler)
        s_defaultHandler(type, context, msg);
}

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#else
    //根据实际屏幕缩放比例更改
    qputenv("QT_SCALE_FACTOR", "1.5");
#endif
#endif
    QApplication a(argc, argv);

    s_defaultHandler = qInstallMessageHandler(customMessageHandler);

    a.setApplicationName(QObject::tr("CoWeld"));
    a.setOrganizationName("YY CO");

    QFont f = a.font();
    f.setFamily("Microsoft YaHei");
    f.setPointSize(10);
    a.setFont(f);

    //检测进程路径是否包含非英文字符
    QString appPath = QCoreApplication::applicationDirPath();
    for (const QChar& ch : appPath)
    {
        if (ch.unicode() > 127)
        {
            QMessageBox::critical(nullptr, QObject::tr("错误"), QObject::tr("应用程序路径包含非英文字符。"));
            return 0;
        }
    }

    Palletizing::MainWindow w;
    w.show();

    return a.exec();
}
