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
    Core/ConfigManager.cpp \
    Gui/BaseWindow.cpp \
    Gui/TitleBar.cpp \
    Gui/FlowLayout.cpp \
    Widgets/MainWindow.cpp \
    Widgets/BrowserWindow.cpp \
    Util/UrlRequestInterceptor.cpp \
    Util/UserAgent.cpp

HEADERS += \
    Core/StreamingProviderStore.hpp \
    Core/StreamingProviderParser.hpp \
    Core/ConfigManager.hpp \
    Gui/BaseWindow.hpp \
    Gui/TitleBar.hpp \
    Gui/FlowLayout.hpp \
    Widgets/MainWindow.hpp \
    Widgets/BrowserWindow.hpp \
    Util/UrlRequestInterceptor.hpp \
    Util/UserAgent.hpp

RESOURCES += \
    Resources/Resources.qrc
