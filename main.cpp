#include <Widgets/MainWindow.hpp>
#include <QApplication>
#include <QtWebEngine>

#include <QDebug>

#include <Core/StreamingProviderParser.hpp>
#include <Core/StreamingProviderStore.hpp>
#include <Widgets/BrowserWindow.hpp>

int main(int argc, char **argv)
{
    QApplication::setDesktopSettingsAware(false);
    QApplication a(argc, argv);
    a.setApplicationName(QLatin1String("LightweightQtDRMStreamViewer"));
    a.setApplicationDisplayName(QLatin1String("Qt DRM Stream Viewer"));
    a.setApplicationVersion("0.9.2");
    a.setWindowIcon(QIcon(":/app-icon.svgz"));

    StreamingProviderParser parser;
    parser.findAll();
    if (parser.providers().isEmpty())
    {
        qDebug() << "WARNING: No providers found! There is nothing to do, exiting now...";
        // TODO: implement a dialog to inform the user from the GUI environment
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

    QString startupProfile;
    bool fullscreen = false; // only when using startup profile

    if (a.arguments().contains("--fullscreen", Qt::CaseInsensitive) ||
        a.arguments().contains("-fs", Qt::CaseInsensitive))
    {
        fullscreen = true;
    }
    for (auto&& i : a.arguments())
    {
        if (i.startsWith("--provider=", Qt::CaseInsensitive))
        {
            startupProfile = i.mid(11);
        }
    }

    // Skip main interface and directly load the given provider
    if (!startupProfile.isEmpty())
    {
        qDebug() << "Loading provider" << startupProfile;

        StreamingProviderStore::Provider pr = StreamingProviderStore::instance()->provider(startupProfile);
        if (pr.id.isEmpty())
        {
            qDebug() << startupProfile << "No such provider! Exiting...";
            return 1;
        }

        qDebug() << "Loading browser window...";
        BrowserWindow w(pr.titleBarVisible);
        w.reset();

        w.setBaseTitle(pr.name);
        w.setTitleBarColor(pr.titleBarColor, pr.titleBarTextColor);
        w.setWindowTitle("Loading...");
        w.setWindowIcon(QIcon(StreamingProviderStore::instance()->providerStorePath() + '/' + pr.icon));
        w.setCookieStoreId(pr.id);
        w.setUrl(QUrl(pr.url));

        qDebug() << "Everything done. Enjoy your shows/movies :D";
        fullscreen ? w.showFullScreen() : w.show();

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
