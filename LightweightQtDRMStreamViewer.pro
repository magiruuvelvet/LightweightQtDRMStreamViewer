QT       += core gui widgets webengine webenginewidgets

TARGET = LightweightQtDRMStreamViewer
TEMPLATE = app

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000


SOURCES += \
    main.cpp \
    MainWindow.cpp \
    StreamingProviderStore.cpp \
    StreamingProviderParser.cpp \
    BrowserWindow.cpp

HEADERS += \
    MainWindow.hpp \
    StreamingProviderStore.hpp \
    StreamingProviderParser.hpp \
    BrowserWindow.hpp

RESOURCES += \
    res/resources.qrc
