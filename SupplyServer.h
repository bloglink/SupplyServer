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

#define PROD_UUID 0x00 //生产ID
#define PROD_GUID 0x01 //操作ID
#define PROD_SIGN 0x02 //操作标识
#define PROD_NUMB 0x03 //生产单号
#define PROD_QUAN 0x04 //生产数量
#define PROD_TYPE 0x05 //产品大类
#define PROD_CODE 0x06 //产品编号
#define PROD_NAME 0x07 //产品名称
#define PROD_MODE 0x08 //产品规格
#define PROD_MNUM 0x09 //仪表编号

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
    void createTabRoles(qint64 uuid);
    void excuteCmdRoles(QJsonObject obj);
    void createTabUsers(qint64 uuid);
    void excuteCmdUsers(QJsonObject obj);
    void createTabSales(qint64 uuid);
    void excuteCmdSales(QJsonObject obj);
    void createTabCusts(qint64 uuid);
    void excuteCmdCusts(QJsonObject obj);
    void createTabOrder(qint64 uuid);
    void createTabSends(qint64 uuid);
    void createTabProds(qint64 uuid);
    void createTabPurch(qint64 uuid);
    void recvNetJson(QJsonObject obj);
    void excuteCmdLogin(QJsonObject obj);




    void orderJson(QJsonObject obj);
    void sendsJson(QJsonObject obj);
    void plansJson(QJsonObject obj);
    void prodsJson(QJsonObject obj);
private:
    Snowflake id;
    UdpSocket udp;
    QSqlDatabase db;
};

#endif // SUPPLYSERVER_H
