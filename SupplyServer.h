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

#define ORDER_ID 0
#define ORDER_GUID 1
#define ORDER_SIGN 2
#define ORDER_NUMB 3
#define ORDER_DATE 4
#define ORDER_AREA 5
#define ORDER_SALE 6
#define ORDER_CUST 7
#define ORDER_VIEW 8
#define ORDER_QUAN 9
#define ORDER_DEAD 10
#define ORDER_MARK 11
#define ORDER_PROD 12
#define ORDER_STCK 13
#define ORDER_LNUM 14
#define ORDER_DNUM 15

#define SEND_ID 0
#define SEND_GUID 1
#define SEND_SIGN 2
#define SEND_NUMB 3
#define SEND_VIEW 4
#define SEND_MODE 5
#define SEND_CODE 6
#define SEND_PRCE 7
#define SEND_MARK 8

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
    void sendsJson(QJsonObject obj);
    void prodsJson(QJsonObject obj);
private:
    Snowflake id;
    UdpSocket udp;
    QSqlDatabase db;
};

#endif // SUPPLYSERVER_H
