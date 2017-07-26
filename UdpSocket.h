/*******************************************************************************
 * Copyright [2017]   <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.0.0.1
 * author:      zhaonanlin
 * brief:       UDP处理模块
*******************************************************************************/
#ifndef SocketUdp_H
#define SocketUdp_H

#include <QUrl>
#include <QUuid>
#include <QTimer>
#include <QQueue>
#include <QObject>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <QApplication>
#include <QNetworkInterface>
#include <QJsonObject>
#include <QJsonDocument>

#include "Snowflake.h"

#define LOCAL_PORT 10000
#define VERSION "erp-0.0.0.1"

class UdpSocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit UdpSocket(QObject *parent = 0);

signals:
    void sendJson(QJsonObject obj);
public:
    void initSocket(quint16 port);
    void quitSocket(void);

private slots:
    void recvNetJson(void);
    void recvAppJson(QJsonObject obj);
    void transmitJson();
private:
    QHostAddress txAddr;
    quint16 txPort;
    QQueue <QJsonObject> send_queue;
    QQueue <QJsonObject> recv_queue;
    Snowflake guid;

};

#endif // SocketUdp_H
