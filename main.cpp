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
    a.setApplicationVersion("0.9.10");
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
        const char *msg = "WARNING: No providers found! There is nothing to do, exiting now...";
        qDebug() << QObject::tr(msg);
        QMessageBox::critical(nullptr, a.applicationDisplayName(),
                              QObject::tr(msg),
                              QMessageBox::Ok, QMessageBox::Ok);
        return 1;
    }
    else
    {
        qDebug() << "Found providers:" << parser.providers();
    }

    for (auto&& i : parser.providers())
    {
        if (parser.parse(i))
        {
            qDebug() << "Added" << i << "to the list of streaming providers.";
        }
        else
        {
            qDebug() << "The file for provider" << i << "has issues. Please check the template. File skipped!";
        }
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

        Provider pr = StreamingProviderStore::instance()->provider(Config()->startupProfile());
        if (pr.id.isEmpty())
        {
            qDebug() << Config()->startupProfile() << "No such provider! Exiting...";
            return 1;
        }

        qDebug() << "Loading browser window...";
        BrowserWindow w;
        w.reset();

        pr.titleBarHasPermanentTitle ?
            w.setBaseTitle(pr.titleBarPermanentTitle, true) :
            w.setBaseTitle(pr.name);
        w.setTitleBarVisibility(pr.titleBarVisible);
        w.setTitleBarColor(pr.titleBarColor, pr.titleBarTextColor);
        w.setWindowTitle("Loading...");
        w.setWindowIcon(pr.icon);
        w.setProfile(pr.id);
        w.setUrl(pr.url);

        qDebug() << "Everything done. Enjoy your shows/movies :D";
        Config()->fullScreenMode() ? w.showFullScreen() : w.show();

        return a.exec();
    }
    else
    {
        qDebug() << "Loading interface...";
        MainWindow w;

        qDebug() << "Everything done. Enjoy your shows/movies :D";
        w.show();

        return a.exec();
    }

    return 0;
}
