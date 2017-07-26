#-------------------------------------------------
#
# Project created by QtCreator 2017-07-26T09:02:22
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SupplyServer
TEMPLATE = app


SOURCES += main.cpp\
        SupplyServer.cpp \
    UdpSocket.cpp \
    Snowflake.cpp

HEADERS  += SupplyServer.h \
    UdpSocket.h \
    Snowflake.h
