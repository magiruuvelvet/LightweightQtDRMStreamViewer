#include "UserAgent.hpp"

#include <QApplication>
#include <QSysInfo>

/**

[ARM] Widevine for Chrome OS User-Agent fix

Mozilla/5.0 (X11; CrOS {ARCH} {VERSION}) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/{CR_VERSION} Safari/537.36

ARCH:
 x86_64
 armv7l
 i686

VERSION:
 10032.86.0

CR_VERSION:
 63.0.3239.140

*/

const QString UserAgent::GetUserAgent(const QString &current_ua)
{
    auto ua = current_ua;

    const auto ProcessorArch = QSysInfo::currentCpuArchitecture();

    // ARM
    if (ProcessorArch.contains("arm", Qt::CaseInsensitive))
    {
        // manipulate user-agent on ARM platforms to stop widevine from complaining
        //  > Mozilla/5.0 (X11; CrOS {ARCH} {VERSION}) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/{CR_VERSION} Safari/537.36
        ua = "Mozilla/5.0 (X11; CrOS {ARCH} {VERSION}) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/{CR_VERSION} Safari/537.36";
        ua.replace("{ARCH}",       "armv7l");
        ua.replace("{VERSION}",    "10032.86.0");
        ua.replace("{CR_VERSION}", "63.0.3239.140");

        // inject app name and version
        ua.replace("Chrome", qApp->applicationName() + '/' + qApp->applicationVersion() + " Chrome");
        return ua;
    }
    else
    {
        // default user-agent
        // just inject app name and version into user-agent
        //  > Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) QtWebEngine/5.9.1 Chrome/56.0.2924.122 Safari/537.36
        ua.replace("QtWebEngine", qApp->applicationName() + '/' + qApp->applicationVersion() + " QtWebEngine");
        return ua;
    }
}
