QT       += core gui widgets webengine webenginewidgets

TARGET = LightweightQtDRMStreamViewer
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


SOURCES += \
    main.cpp \
    Core/StreamingProviderStore.cpp \
    Core/StreamingProviderParser.cpp \
    Core/StreamingProviderWriter.cpp \
    Core/ConfigManager.cpp \
    Core/BrowserWindowProcess.cpp \
    Gui/BaseWindow.cpp \
    Gui/TitleBar.cpp \
    Gui/FlowLayout.cpp \
    Gui/ProviderButton.cpp \
    Widgets/MainWindow.cpp \
    Widgets/BrowserWindow.cpp \
    Widgets/ConfigWindow.cpp \
    Widgets/ProviderEditWidget.cpp \
    Util/UrlRequestInterceptor.cpp \
    Util/UserAgent.cpp \
    Util/RandomString.cpp

HEADERS += \
    Core/StreamingProviderStore.hpp \
    Core/StreamingProviderParser.hpp \
    Core/StreamingProviderWriter.hpp \
    Core/ConfigManager.hpp \
    Core/BrowserWindowProcess.hpp \
    Gui/BaseWindow.hpp \
    Gui/TitleBar.hpp \
    Gui/FlowLayout.hpp \
    Gui/ProviderButton.hpp \
    Widgets/MainWindow.hpp \
    Widgets/ConfigWindow.hpp \
    Widgets/BrowserWindow.hpp \
    Widgets/ProviderEditWidget.hpp \
    Util/UrlRequestInterceptor.hpp \
    Util/UserAgent.hpp \
    Util/RandomString.hpp

RESOURCES += \
    Resources/Resources.qrc
