# -------------------------------------------------
# Project created by QtCreator 2010-08-14T20:45:54
# -------------------------------------------------
LIBS += -lexiv2
TARGET = ExifStatistics
TEMPLATE = app
INCLUDEPATH += include/
SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/collector.cpp \
    src/entryreader.cpp \
    src/makefilter.cpp \
    src/modelfilter.cpp \
    src/tabexporter.cpp \
    src/htmlexporter.cpp
HEADERS += include/mainwindow.h \
    include/collector.h \
    include/entryreader.h \
    include/ifilter.h \
    include/makefilter.h \
    include/modelfilter.h \
    include/htmlexporter.h \
    include/tabexporter.h \
    include/exporter.h
FORMS += ui/mainwindow.ui
