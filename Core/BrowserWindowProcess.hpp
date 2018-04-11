#ifndef BROWSERWINDOWPROCESS_HPP
#define BROWSERWINDOWPROCESS_HPP

#include <QProcess>
#include <QMutex>
#include "StreamingProviderStore.hpp"

class BrowserWindowProcess : public QProcess
{
    Q_OBJECT

public:
    BrowserWindowProcess(QObject *parent = nullptr);
    ~BrowserWindowProcess();

    void start(const Provider &provider, OpenMode mode = ReadOnly);

private slots:
    void _started();
    void _finished(int exitCode, ExitStatus exitStatus);
    void _errorOccurred(ProcessError);
    void _stateChanged(ProcessState);

    void _output();

private:
    QMutex g_mutex;
};

#endif // BROWSERWINDOWPROCESS_HPP
