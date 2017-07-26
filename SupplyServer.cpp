#include "SupplyServer.h"

SupplyServer::SupplyServer(QWidget *parent)
    : QWidget(parent)
{
    initUdp();
    initSql();
}

SupplyServer::~SupplyServer()
{

}

void SupplyServer::initUdp()
{
    udp.initSocket(10000);
    connect(this,SIGNAL(sendJson(QJsonObject)),&udp,SLOT(recvAppJson(QJsonObject)));
    connect(&udp,SIGNAL(sendJson(QJsonObject)),this,SLOT(recvNetJson(QJsonObject)));
}

void SupplyServer::initSql()
{
    QFile file("erp.db");
    if (!file.exists()) {
        file.open(QIODevice::ReadWrite);
        file.close();
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("erp.db");
    db.open();

    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_roles");
    query.exec("drop table erp_roles_log");

    cmd = "create table if not exists erp_roles(";//创建角色表
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "role_name text,";//角色名称
    cmd += "role_mark text)";//角色备注
    query.exec(cmd);

    cmd = "create table if not exists erp_roles_log(";//创建角色日志表
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";//操作标识
    cmd += "tabs_guid integer,";//角色ID
    cmd += "role_name text,";
    cmd += "role_mark text)";
    query.exec(cmd);

    query.exec("drop table erp_users");
    query.exec("drop table erp_users_log");

    cmd = "create table if not exists erp_users(";//创建用户表
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "user_name text,";//用户名称
    cmd += "user_pass text,";//用户密码
    cmd += "user_role text,";//用户角色
    cmd += "user_date text)";//加入日期
    query.exec(cmd);

    cmd = "create table if not exists erp_users_log(";//创建用户日志表
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "user_name text,";
    cmd += "user_pass text,";
    cmd += "user_role text,";
    cmd += "user_date text)";
    query.exec(cmd);

    query.prepare("insert into erp_users values(?,?,?,?,?,?,?)");//加入管理员
    query.bindValue(0,id.getId());
    query.bindValue(1,id.getId());
    query.bindValue(2,1);
    query.bindValue(3,"admin");
    query.bindValue(4,"");
    query.bindValue(5,"管理");
    query.bindValue(6,"2017-07-24");
    query.exec();

    query.exec("drop table erp_sales");
    query.exec("drop table erp_sales_log");

    cmd = "create table if not exists erp_sales(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "sale_name text,";//销售名称
    cmd += "sale_area text)";//所属区域
    query.exec(cmd);

    cmd = "create table if not exists erp_sales_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "sale_name text,";
    cmd += "sale_area text)";
    query.exec(cmd);

    query.exec("drop table erp_custs");
    query.exec("drop table erp_custs_log");

    cmd = "create table if not exists erp_custs(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "cust_name text,";//客户名称
    cmd += "cust_sale text,";//销售名称
    cmd += "cust_area text)";//所属区域
    query.exec(cmd);

    cmd = "create table if not exists erp_custs_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "cust_name text,";
    cmd += "cust_sale text,";
    cmd += "cust_area text)";
    query.exec(cmd);

    query.exec("drop table erp_orders");
    query.exec("drop table erp_orders_log");

    cmd = "create table if not exists erp_orders(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "order_numb text,";//订单编号
    cmd += "order_date text,";//订单日期
    cmd += "order_view text,";//评审编号
    cmd += "order_cust text,";//客户名称
    cmd += "order_sale text,";//销售经理
    cmd += "order_area text,";//所属区域
    cmd += "order_dead text,";//交货日期
    cmd += "order_quan text,";//订货数量
    cmd += "order_prod text,";//在产数量
    cmd += "order_stck text,";//入库数量
    cmd += "order_dnum text)";//发货数量
    query.exec(cmd);

    cmd = "create table if not exists erp_orders_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "order_numb text,";
    cmd += "order_date text,";
    cmd += "order_view text,";
    cmd += "order_cust text,";
    cmd += "order_sale text,";
    cmd += "order_area text,";
    cmd += "order_dead text,";
    cmd += "order_quan text,";
    cmd += "order_prod text,";
    cmd += "order_stck text,";
    cmd += "order_dnum text)";
    query.exec(cmd);

    query.exec("drop table erp_prods");
    query.exec("drop table erp_prods_log");

    cmd = "create table if not exists erp_prods(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "prod_numb text,";//订单编号
    cmd += "prod_date text,";//订单日期
    cmd += "prod_view text,";//评审编号
    cmd += "prod_cust text,";//客户名称
    cmd += "prod_sale text,";//销售名称
    cmd += "prod_area text,";//所属区域
    cmd += "prod_dead text,";//交货日期
    cmd += "prod_need text,";//订货数量
    cmd += "prod_quan text,";//在产数量
    cmd += "prod_pnum text,";//生产单号
    cmd += "prod_type text,";//产品大类
    cmd += "prod_code text,";//产品编号
    cmd += "prod_name text,";//产品名称
    cmd += "prod_mode text,";//产品规格
    cmd += "prod_mnum text,";//仪表编号
    cmd += "prod_stck text)";//入库标志
    query.exec(cmd);

    cmd = "create table if not exists erp_prods_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "prod_numb text,";
    cmd += "prod_date text,";
    cmd += "prod_view text,";
    cmd += "prod_cust text,";
    cmd += "prod_sale text,";
    cmd += "prod_area text,";
    cmd += "prod_dead text,";
    cmd += "prod_need text,";
    cmd += "prod_quan text,";
    cmd += "prod_pnum text,";
    cmd += "prod_type text,";
    cmd += "prod_code text,";
    cmd += "prod_name text,";
    cmd += "prod_mode text,";
    cmd += "prod_mnum text,";
    cmd += "prod_stck text)";
    query.exec(cmd);

    cmd = "create table if not exists erp_purchs(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "purch_numb text,";
    cmd += "purch_code text,";
    cmd += "purch_name text,";
    cmd += "purch_unit text,";
    cmd += "purch_lack text,";
    cmd += "purch_quan text,";
    cmd += "purch_date text,";
    cmd += "purch_bout text,";
    cmd += "purch_expt text,";
    cmd += "purch_real text,";
    cmd += "purch_come text,";
    cmd += "purch_oway text,";
    cmd += "purch_ofix text,";
    cmd += "purch_mark text)";
    query.exec(cmd);

    cmd = "create table if not exists erp_purchs_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";;
    cmd += "purch_numb text,";
    cmd += "purch_code text,";
    cmd += "purch_name text,";
    cmd += "purch_unit text,";
    cmd += "purch_lack text,";
    cmd += "purch_quan text,";
    cmd += "purch_date text,";
    cmd += "purch_bout text,";
    cmd += "purch_expt text,";
    cmd += "purch_real text,";
    cmd += "purch_come text,";
    cmd += "purch_oway text,";
    cmd += "purch_ofix text,";
    cmd += "purch_mark text)";
    query.exec(cmd);
}

void SupplyServer::recvNetJson(QJsonObject obj)
{
    QString cmd = obj.value("logs_cmmd").toString();
    if (cmd == "erp_login")
        loginJson(obj);
    if (cmd == "erp_roles")
        rolesJson(obj);
    if (cmd == "erp_users")
        usersJson(obj);
    if (cmd == "erp_sales")
        salesJson(obj);
    if (cmd == "erp_custs")
        custsJson(obj);
    if (cmd == "erp_order")
        orderJson(obj);
    if (cmd == "erp_prods")
        prodsJson(obj);
}

void SupplyServer::loginJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QString user_name = obj.value("user_name").toString();
    QString user_pass = obj.value("user_pass").toString();
    bool error = true;
    query.prepare("select user_pass from erp_users where user_name==:user_name");
    query.bindValue(":user_name",user_name);
    query.exec();
    if (query.next()) {//将最新的操作记录返回
        if (user_pass == query.value(0).toString())
            error = false;
    }
    QJsonObject send_obj;
    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_login");
    send_obj.insert("error",error);

    sendJson(send_obj);
}

void SupplyServer::rolesJson(QJsonObject obj)
{
    QSqlQuery query(db);

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_sign = obj.value("logs_sign").toDouble();
        query.prepare("select * from erp_roles where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            QJsonObject send_obj;
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_roles");
            send_obj.insert("tabs_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("role_name",query.value(3).toString());
            send_obj.insert("role_mark",query.value(4).toString());
            sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
    case 2://删除只作更改
    case 3://修改
        tabs_guid = logs_guid;
        query.prepare("replace into erp_roles values(?,?,?,?,?)");
        query.bindValue(0,tabs_guid);
        query.bindValue(1,logs_guid);
        query.bindValue(2,logs_sign);
        query.bindValue(3,obj.value("role_name").toString());
        query.bindValue(4,obj.value("role_mark").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("insert into erp_roles_log values(?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(3,obj.value("role_name").toString());
    query.bindValue(4,obj.value("role_mark").toString());
    query.exec();
}

void SupplyServer::usersJson(QJsonObject obj)
{

}

void SupplyServer::salesJson(QJsonObject obj)
{

}

void SupplyServer::custsJson(QJsonObject obj)
{

}

void SupplyServer::orderJson(QJsonObject obj)
{

}

void SupplyServer::prodsJson(QJsonObject obj)
{

}
