#ifndef SUPPLYSERVER_H
#define SUPPLYSERVER_H

#include <QFile>
#include <QWidget>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonDocument>

#include "UdpSocket.h"
#include "Snowflake.h"

class SupplyServer : public QWidget
{
    Q_OBJECT

signals:
    void sendJson(QJsonObject obj);
public:
    SupplyServer(QWidget *parent = 0);
    ~SupplyServer();
private slots:
    void initUdp();
    void initSql();
    void recvNetJson(QJsonObject obj);
    void loginJson(QJsonObject obj);
    void rolesJson(QJsonObject obj);
    void usersJson(QJsonObject obj);
    void salesJson(QJsonObject obj);
    void custsJson(QJsonObject obj);
    void orderJson(QJsonObject obj);
    void prodsJson(QJsonObject obj);
private:
    Snowflake id;
    UdpSocket udp;
    QSqlDatabase db;
};

#endif // SUPPLYSERVER_H
