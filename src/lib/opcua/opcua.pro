QT += core  network

TEMPLATE = lib
DEFINES += OPCUA_LIBRARY
include($$PWD/../../../libopcua.pri)
#include($$PWD/../../../dependence/open62541_win_vs2017/open62541.pri)

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    clientmanager.h \
    configparser.h \
    opcua_global.h \
    opcuaclient.h \
    opcuaserver.h \
    open62541.h \
    servermanager.h

SOURCES += \
    clientmanager.cpp \
    configparser.cpp \
    opcuaclient.cpp \
    opcuaserver.cpp \
    open62541.c \
    servermanager.cpp

LIBS += -L$$PWD/lib/ -lWS2_32 -liphlpapi

CONFIG(debug,debug|release) : TARGET = opcuad
CONFIG(release,debug|release) : TARGET = opcua

headers.files = $$HEADERS
headers.path = $$BIN_DIST_DIR/opcua/include

target.path = $$BIN_DIST_DIR/opcua/lib

inspri.files = opcua.pri
inspri.path = $$BIN_DIST_DIR/opcua

insbin.files = ./bin/*
insbin.path = $$BIN_DIST_DIR/opcua/bin

INSTALLS += target headers inspri insbin

DLLDESTDIR = $$BIN_DIST_DIR/opcua/bin

win32-msvc*{
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}
