#include <QApplication>
#include <QMessageBox>
#include <QtWebEngine>

#include <Core/ConfigManager.hpp>
#include <Core/StreamingProviderParser.hpp>
#include <Core/StreamingProviderStore.hpp>

#include <Widgets/MainWindow.hpp>
#include <Widgets/BrowserWindow.hpp>

#include <QDebug>

#include <QFile>
#include <QFileInfo>
#include <QByteArray>
void compress_plugin(const QString &in, int level = -1)
{
    if (!QFileInfo::exists(in))
        return;

    QFile ifile(in);
    if (ifile.open(QFile::ReadOnly | QFile::Text))
    {
        QByteArray out = qCompress(ifile.readAll(), level);
        ifile.close();

        QFile ofile(in+".qgz");
        if (ofile.open(QFile::WriteOnly))
        {
            ofile.write(out);
            ofile.close();
        }
        out.clear();
    }
}

int main(int argc, char **argv)
{
    QApplication::setDesktopSettingsAware(false);
    QApplication a(argc, argv);
    a.setApplicationName(QLatin1String("LightweightQtDRMStreamViewer"));
    a.setApplicationDisplayName(QLatin1String("Qt DRM Stream Viewer"));
    a.setApplicationVersion("0.3");
    a.setWindowIcon(QIcon(":/app-icon.svgz"));

    if (a.arguments().contains("-c"))
    {
        compress_plugin(a.arguments().at(2), 9);
        return 0;
    }

    StreamingProviderParser parser;
    parser.findAll();
    if (parser.providers().isEmpty())
    {
        qDebug() << "Warning: No providers found.";
    }
    else
    {
        qDebug() << "Found providers:" << parser.providers();
    }

    qDebug() << "\n-------------------------\n";
    for (auto&& i : parser.providers())
    {
        // parse provider file
        StreamingProviderParser::StatusCode status = parser.parse(i);

        switch (status)
        {
            case StreamingProviderParser::SUCCESS:
                qDebug() << "Added" << i << "to the list of streaming providers.";
                break;
            case StreamingProviderParser::FILE_ERROR:
                qDebug() << "File" << i << "is faulty and was skipped!";
                break;
            case StreamingProviderParser::SYNTAX_ERROR:
                qDebug() << "The file for provider" << i << "has issues. Please check the template. File skipped!";
                break;
            case StreamingProviderParser::FILE_EMPTY:
                qDebug() << "File" << i << "is empty and was skipped!";
                break;
            case StreamingProviderParser::ALREADY_IN_LIST:
                qDebug() << "Provider" << QFileInfo(i).baseName() << "is already in the list from a higher priority target! Skipped.";
                break;
        }

        StreamingProviderStore::instance()->sort();

        qDebug() << "\n-------------------------\n";
    }

    qDebug() << "Initializing Qt Web Engine...";
    QtWebEngine::initialize();

    if (a.arguments().contains("--fullscreen", Qt::CaseInsensitive) ||
        a.arguments().contains("-fs", Qt::CaseInsensitive))
    {
        qDebug() << "Full screen mode was activated.";
        Config()->fullScreenMode() = true;
    }
    for (auto&& i : a.arguments())
    {
        if (i.startsWith("--provider=", Qt::CaseInsensitive))
        {
            Config()->startupProfile() = i.mid(11);
        }
    }

    // Skip main interface and directly load the given provider
    if (!Config()->startupProfile().isEmpty())
    {
        qDebug() << "Loading provider" << Config()->startupProfile();

        const Provider pr = StreamingProviderStore::instance()->provider(Config()->startupProfile());
        if (pr.id.isEmpty())
        {
            qDebug() << Config()->startupProfile() << "No such provider! Exiting...";
            return 1;
        }

        qDebug() << "Loading browser window...";
        BrowserWindow *w = BrowserWindow::getInstance();
        w->setProfile(pr);

        qDebug() << "Everything done. Enjoy your shows/movies :D";
        Config()->fullScreenMode() ? w->showFullScreen() : w->show();

        auto status_code = a.exec();
        delete BrowserWindow::getInstance();
        return status_code;
    }
    else
    {
        qDebug() << "Loading interface...";
        MainWindow w;

        qDebug() << "Everything done. Enjoy your shows/movies :D";
        w.show();

        auto status_code = a.exec();
        //delete BrowserWindow::getInstance();
        return status_code;
    }

    return 0;
}
