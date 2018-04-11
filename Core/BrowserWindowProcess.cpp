#include "BrowserWindowProcess.hpp"

#include <QApplication>
#include <QMutexLocker>

#include "ConfigManager.hpp"

BrowserWindowProcess::BrowserWindowProcess(QObject *parent)
    : QProcess(parent)
{
    QObject::connect(this, &BrowserWindowProcess::started, this, &BrowserWindowProcess::_started);
    QObject::connect(this, QOverload<int, QProcess::ExitStatus>::of(&BrowserWindowProcess::finished),
        this, &BrowserWindowProcess::_finished);
    QObject::connect(this, &BrowserWindowProcess::errorOccurred, this, &BrowserWindowProcess::_errorOccurred);
    QObject::connect(this, &BrowserWindowProcess::stateChanged, this, &BrowserWindowProcess::_stateChanged);

    QObject::connect(this, &BrowserWindowProcess::readyReadStandardOutput, this, &BrowserWindowProcess::_output);
    QObject::connect(this, &BrowserWindowProcess::readyReadStandardError, this, &BrowserWindowProcess::_output);
}

BrowserWindowProcess::~BrowserWindowProcess()
{
}

void BrowserWindowProcess::start(const Provider &provider, OpenMode mode)
{
    const QStringList arguments = ([&]{
        QStringList arguments = {
            "--provider=" + provider.id
        };
        if (Config()->fullScreenMode())
            arguments.append("-fs");
        return arguments;
    })();

    QProcess::start(QApplication::applicationFilePath(), arguments, mode);
    QProcess::waitForStarted();
}

void BrowserWindowProcess::_started()
{
    qDebug() << "started";
}

void BrowserWindowProcess::_finished(int exitCode, ExitStatus exitStatus)
{
    qDebug() << exitCode << exitStatus;
}

void BrowserWindowProcess::_errorOccurred(ProcessError error)
{
    qDebug() << error;
}

void BrowserWindowProcess::_stateChanged(ProcessState state)
{
    qDebug() << state;
}

void BrowserWindowProcess::_output()
{
    QMutexLocker(&this->g_mutex);
    qDebug() << this->readAllStandardOutput().constData();
    qDebug() << this->readAllStandardError().constData();
}
