/*******************************************************************************
 * Copyright [2017]   <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.0.0.1
 * author:      zhaonanlin
 * brief:       UDP处理模块
*******************************************************************************/
#include "UdpSocket.h"

UdpSocket::UdpSocket(QObject *parent) : QUdpSocket(parent)
{

}

void UdpSocket::initSocket(quint16 port)
{
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    for(int i=0;i < addrs.size();i++) {
        if((addrs.at(i)!=QHostAddress::LocalHost) &&
                (addrs.at(i).protocol()==QAbstractSocket::IPv4Protocol))
            txAddr = addrs.at(i);
    }
    txPort = port;

    this->bind(QHostAddress::AnyIPv4,port);
    connect(this, SIGNAL(readyRead()), this, SLOT(recvNetJson()));

    guid.setMachineId(20,20);

    QTimer *t_send = new QTimer(this);//定时处理消息队列
    connect(t_send,SIGNAL(timeout()),this,SLOT(transmitJson()));
    t_send->start(5);
}

void UdpSocket::quitSocket()
{
    this->close();
}

void UdpSocket::recvNetJson()
{
    while (this->hasPendingDatagrams()) {
        QByteArray msg;
        msg.resize(this->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        this->readDatagram(msg.data(), msg.size(), &sender, &senderPort);
        QJsonObject obj = QJsonDocument::fromJson(QByteArray::fromBase64(msg)).object();
        obj.insert("sender",sender.toString());
        qDebug() << "rcev" << obj;
        recv_queue.enqueue(obj);
    }
}

void UdpSocket::recvAppJson(QJsonObject obj)
{
    send_queue.enqueue(obj);
}

void UdpSocket::transmitJson()
{
    if (!send_queue.isEmpty()) {
        QJsonObject obj = send_queue.dequeue();
        QByteArray msg = QJsonDocument(obj).toJson();
        QHostAddress hostAddr(obj.value("sendto").toString());
        this->writeDatagram(msg.toBase64(),hostAddr,txPort);
        this->waitForBytesWritten();
    }
    if (!recv_queue.isEmpty())
        emit sendJson(recv_queue.dequeue());
}

/*********************************END OF FILE**********************************/
