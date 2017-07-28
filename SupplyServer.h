#ifndef SUPPLYSERVER_H
#define SUPPLYSERVER_H

#include <QFile>
#include <QWidget>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonDocument>

#include <QLayout>
#include <QTableView>
#include <QSqlTableModel>

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

#define PROD_ID 0
#define PROD_GUID 1
#define PROD_SIGN 2
#define PROD_NUMB 3 //订单编号
#define PROD_DATE 4 //订单日期
#define PROD_VIEW 5 //评审编号
#define PROD_CUST 6 //客户名称
#define PROD_DEAD 7 //交货日期
#define PROD_QUAN 8 //在产数量
#define PROD_PNUM 9 //生产单号
#define PROD_TYPE 10//产品大类
#define PROD_CODE 11//产品编号
#define PROD_NAME 12//产品名称
#define PROD_MODE 13//产品规格
#define PROD_MNUM 14//仪表编号

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
    void initUI();
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
