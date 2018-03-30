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
    Gui/TitleBar.cpp \
    Gui/FlowLayout.cpp \
    Widgets/MainWindow.cpp \
    Widgets/BrowserWindow.cpp \
    Core/ConfigManager.cpp \
    Util/UrlRequestInterceptor.cpp \
    Gui/BaseWindow.cpp

HEADERS += \
    Core/StreamingProviderStore.hpp \
    Core/StreamingProviderParser.hpp \
    Gui/TitleBar.hpp \
    Gui/FlowLayout.hpp \
    Widgets/MainWindow.hpp \
    Widgets/BrowserWindow.hpp \
    Core/ConfigManager.hpp \
    Util/UrlRequestInterceptor.hpp \
    Gui/BaseWindow.hpp

RESOURCES += \
    Resources/Resources.qrc
