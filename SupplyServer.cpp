#include "SupplyServer.h"

SupplyServer::SupplyServer(QWidget *parent)
    : QWidget(parent)
{
    initUdp();
    initSql();
    initUI();
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

    //    query.exec("drop table erp_roles");
    //    query.exec("drop table erp_roles_log");

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

    //    query.exec("drop table erp_users");
    //    query.exec("drop table erp_users_log");

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

    query.exec("select count(id) from erp_users");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_users values(?,?,?,?,?,?,?)");//加入管理员
        query.bindValue(0,id.getId());
        query.bindValue(1,id.getId());
        query.bindValue(2,1);
        query.bindValue(3,"admin");
        query.bindValue(4,"");
        query.bindValue(5,"管理");
        query.bindValue(6,"2017-07-24");
        query.exec();
    }

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

//    query.exec("drop table erp_custs");
//    query.exec("drop table erp_custs_log");

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

//    query.exec("drop table erp_order");
//    query.exec("drop table erp_order_log");

    cmd = "create table if not exists erp_order(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "order_numb text,";//订单编号
    cmd += "order_date text,";//订单日期
    cmd += "order_area text,";//所属区域
    cmd += "order_sale text,";//销售经理
    cmd += "order_cust text,";//客户名称
    cmd += "order_view text,";//评审编号
    cmd += "order_quan text,";//订货数量
    cmd += "order_dead text,";//交货日期
    cmd += "order_mark text,";//备注内容
    cmd += "order_prod text,";//在产数量
    cmd += "order_stck text,";//入库数量
    cmd += "order_lnum text,";//未发数量
    cmd += "order_dnum text)";//发货数量
    query.exec(cmd);

    cmd = "create table if not exists erp_order_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "order_numb text,";//订单编号
    cmd += "order_date text,";//订单日期
    cmd += "order_area text,";//所属区域
    cmd += "order_sale text,";//销售经理
    cmd += "order_cust text,";//客户名称
    cmd += "order_view text,";//评审编号
    cmd += "order_quan text,";//订货数量
    cmd += "order_dead text,";//交货日期
    cmd += "order_mark text,";//备注内容
    cmd += "order_prod text,";//在产数量
    cmd += "order_stck text,";//入库数量
    cmd += "order_lnum text,";//未发数量
    cmd += "order_dnum text)";//发货数量
    query.exec(cmd);

//    query.exec("drop table erp_sends");
//    query.exec("drop table erp_sends_log");

    cmd = "create table if not exists erp_sends(";
    cmd += "id integer primary key,";
    cmd += "logs_guid interger,";
    cmd += "logs_sign interger,";
    cmd += "send_numb text,";//订单单号
    cmd += "send_view text,";//评审单号
    cmd += "send_mode text,";//发货方式
    cmd += "send_code text,";//货运单号
    cmd += "send_prce text,";//运费
    cmd += "send_mark text)";//货运单号
    query.exec(cmd);

    cmd = "create table if not exists erp_sends_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "send_numb text,";//订单单号
    cmd += "send_view text,";//评审单号
    cmd += "send_mode text,";//发货方式
    cmd += "send_code text,";//货运单号
    cmd += "send_prce text)";//运费
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
    cmd += "prod_dead text,";//交货日期
    cmd += "prod_quan text,";//在产数量
    cmd += "prod_pnum text,";//生产单号
    cmd += "prod_type text,";//产品大类
    cmd += "prod_code text,";//产品编号
    cmd += "prod_name text,";//产品名称
    cmd += "prod_mode text,";//产品规格
    cmd += "prod_mnum text)";//仪表编号
    query.exec(cmd);

    cmd = "create table if not exists erp_prods_log(";
    cmd += "id integer primary key,";
    cmd += "logs_sign integer,";
    cmd += "tabs_guid integer,";
    cmd += "prod_numb text,";//订单编号
    cmd += "prod_date text,";//订单日期
    cmd += "prod_view text,";//评审编号
    cmd += "prod_cust text,";//客户名称
    cmd += "prod_dead text,";//交货日期
    cmd += "prod_quan text,";//在产数量
    cmd += "prod_pnum text,";//生产单号
    cmd += "prod_type text,";//产品大类
    cmd += "prod_code text,";//产品编号
    cmd += "prod_name text,";//产品名称
    cmd += "prod_mode text,";//产品规格
    cmd += "prod_mnum text)";//仪表编号
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

void SupplyServer::initUI()
{
//    qDebug() << "Available drivers:";
//    QStringList drivers = QSqlDatabase::drivers(); //查询支持的数据库驱动
//    foreach(QString driver, drivers)
//        qDebug() << "\t" << driver;

    QSqlDatabase u8 = QSqlDatabase::addDatabase("QODBC", "dbTemp");
    u8.setHostName("192.168.1.8");
    QString dsn = QString::fromLocal8Bit("DRIVER={SQL SERVER};SERVER=192.168.1.8;DATABASE=UFDATA_005_2017");
    u8.setDatabaseName(dsn);
    u8.setUserName("sa");
//    u8.setPassword("123456");
    if(!u8.open()) //连接数据库
            qDebug() << "open fail" << u8.lastError();

//    QSqlQuery query(u8);

//    query.exec("SELECT Name FROM Master..SysDatabases ORDER BY Name");
//    while (query.next()) //查询服务器所有数据库
//        qDebug() << query.value(0).toString();

//    query.exec("SELECT Name FROM SysObjects Where XType='U' ORDER BY Name");
//    while (query.next())
//        qDebug() << query.value(0).toString();

    QTableView *tab = new QTableView(this);
    QVBoxLayout *lay = new QVBoxLayout;
    lay->addWidget(tab);
    this->setLayout(lay);
    this->resize(1024,768);
    QSqlTableModel *m_u8 = new QSqlTableModel(this,u8);
    m_u8->setTable("IA_Individual");
    m_u8->select();
    tab->setModel(m_u8);
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
    if (cmd == "erp_sends")
        sendsJson(obj);
}

void SupplyServer::loginJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QString user_name = obj.value("user_name").toString();
    QString user_pass = obj.value("user_pass").toString();
    bool logs_stat = false;
    query.prepare("select user_pass from erp_users where user_name==:user_name");
    query.bindValue(":user_name",user_name);
    query.exec();
    if (query.next()) {//将最新的操作记录返回
        if (user_pass == query.value(0).toString())
            logs_stat = true;
    }
    QJsonObject send_obj;
    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_login");
    send_obj.insert("logs_stat",logs_stat);

    sendJson(send_obj);
}

void SupplyServer::rolesJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_roles where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
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
        tabs_guid = logs_guid;
    case 2://删除只作更改
    case 3://修改
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

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_roles");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::usersJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_users where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_users");
            send_obj.insert("tabs_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("user_name",query.value(3).toString());
            send_obj.insert("user_pass",query.value(4).toString());
            send_obj.insert("user_role",query.value(5).toString());
            send_obj.insert("user_date",query.value(6).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_users values(?,?,?,?,?,?,?)");
        query.bindValue(0,tabs_guid);
        query.bindValue(1,logs_guid);
        query.bindValue(2,logs_sign);
        query.bindValue(3,obj.value("user_name").toString());
        query.bindValue(4,obj.value("user_pass").toString());
        query.bindValue(5,obj.value("user_role").toString());
        query.bindValue(6,obj.value("user_date").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("insert into erp_users_log values(?,?,?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(3,obj.value("user_name").toString());
    query.bindValue(4,obj.value("user_pass").toString());
    query.bindValue(5,obj.value("user_role").toString());
    query.bindValue(6,obj.value("user_date").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_users");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::salesJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_sales where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_sales");
            send_obj.insert("tabs_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("sale_name",query.value(3).toString());
            send_obj.insert("sale_area",query.value(4).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_sales values(?,?,?,?,?)");
        query.bindValue(0,tabs_guid);
        query.bindValue(1,logs_guid);
        query.bindValue(2,logs_sign);
        query.bindValue(3,obj.value("sale_name").toString());
        query.bindValue(4,obj.value("sale_area").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("insert into erp_sales_log values(?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(3,obj.value("sale_name").toString());
    query.bindValue(4,obj.value("sale_area").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_sales");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::custsJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_custs where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_custs");
            send_obj.insert("tabs_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("cust_name",query.value(3).toString());
            send_obj.insert("cust_sale",query.value(4).toString());
            send_obj.insert("cust_area",query.value(5).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_custs values(?,?,?,?,?,?)");
        query.bindValue(0,tabs_guid);
        query.bindValue(1,logs_guid);
        query.bindValue(2,logs_sign);
        query.bindValue(3,obj.value("cust_name").toString());
        query.bindValue(4,obj.value("cust_sale").toString());
        query.bindValue(5,obj.value("cust_area").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("insert into erp_custs_log values(?,?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(3,obj.value("cust_name").toString());
    query.bindValue(4,obj.value("cust_sale").toString());
    query.bindValue(5,obj.value("cust_area").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_custs");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::orderJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_order where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_order");
            send_obj.insert("tabs_guid",query.value(ORDER_ID).toDouble());
            send_obj.insert("logs_guid",query.value(ORDER_GUID).toDouble());
            send_obj.insert("logs_sign",query.value(ORDER_SIGN).toDouble());
            send_obj.insert("order_numb",query.value(ORDER_NUMB).toString());
            send_obj.insert("order_date",query.value(ORDER_DATE).toString());
            send_obj.insert("order_view",query.value(ORDER_VIEW).toString());
            send_obj.insert("order_cust",query.value(ORDER_CUST).toString());
            send_obj.insert("order_sale",query.value(ORDER_SALE).toString());
            send_obj.insert("order_area",query.value(ORDER_AREA).toString());
            send_obj.insert("order_dead",query.value(ORDER_DEAD).toString());
            send_obj.insert("order_quan",query.value(ORDER_QUAN).toString());
            send_obj.insert("order_mark",query.value(ORDER_MARK).toString());
            send_obj.insert("order_prod",query.value(ORDER_PROD).toString());
            send_obj.insert("order_stck",query.value(ORDER_STCK).toString());
            send_obj.insert("order_lnum",query.value(ORDER_LNUM).toString());
            send_obj.insert("order_dnum",query.value(ORDER_DNUM).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_order values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        query.bindValue(ORDER_ID,tabs_guid);
        query.bindValue(ORDER_GUID,logs_guid);
        query.bindValue(ORDER_SIGN,logs_sign);
        query.bindValue(ORDER_NUMB,obj.value("order_numb").toString());
        query.bindValue(ORDER_DATE,obj.value("order_date").toString());
        query.bindValue(ORDER_VIEW,obj.value("order_view").toString());
        query.bindValue(ORDER_CUST,obj.value("order_cust").toString());
        query.bindValue(ORDER_SALE,obj.value("order_sale").toString());
        query.bindValue(ORDER_AREA,obj.value("order_area").toString());
        query.bindValue(ORDER_DEAD,obj.value("order_dead").toString());
        query.bindValue(ORDER_QUAN,obj.value("order_quan").toString());
        query.bindValue(ORDER_MARK,obj.value("order_mark").toString());
        query.bindValue(ORDER_PROD,obj.value("order_prod").toString());
        query.bindValue(ORDER_STCK,obj.value("order_stck").toString());
        query.bindValue(ORDER_LNUM,obj.value("order_lnum").toString());
        query.bindValue(ORDER_DNUM,obj.value("order_dnum").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("replace into erp_order_log values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(ORDER_NUMB,obj.value("order_numb").toString());
    query.bindValue(ORDER_DATE,obj.value("order_date").toString());
    query.bindValue(ORDER_VIEW,obj.value("order_view").toString());
    query.bindValue(ORDER_CUST,obj.value("order_cust").toString());
    query.bindValue(ORDER_SALE,obj.value("order_sale").toString());
    query.bindValue(ORDER_AREA,obj.value("order_area").toString());
    query.bindValue(ORDER_DEAD,obj.value("order_dead").toString());
    query.bindValue(ORDER_QUAN,obj.value("order_quan").toString());
    query.bindValue(ORDER_MARK,obj.value("order_mark").toString());
    query.bindValue(ORDER_PROD,obj.value("order_prod").toString());
    query.bindValue(ORDER_STCK,obj.value("order_stck").toString());
    query.bindValue(ORDER_LNUM,obj.value("order_lnum").toString());
    query.bindValue(ORDER_DNUM,obj.value("order_dnum").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_order");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::sendsJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_sends where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_sends");
            send_obj.insert("tabs_guid",query.value(SEND_ID).toDouble());
            send_obj.insert("logs_guid",query.value(SEND_GUID).toDouble());
            send_obj.insert("logs_sign",query.value(SEND_SIGN).toDouble());
            send_obj.insert("send_numb",query.value(SEND_NUMB).toString());
            send_obj.insert("send_view",query.value(SEND_VIEW).toString());
            send_obj.insert("send_mode",query.value(SEND_MODE).toString());
            send_obj.insert("send_code",query.value(SEND_CODE).toString());
            send_obj.insert("send_prce",query.value(SEND_PRCE).toString());
            send_obj.insert("send_mark",query.value(SEND_MARK).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_sends values(?,?,?,?,?,?,?,?,?)");
        query.bindValue(SEND_ID,tabs_guid);
        query.bindValue(SEND_GUID,logs_guid);
        query.bindValue(SEND_SIGN,logs_sign);
        query.bindValue(SEND_NUMB,obj.value("send_numb").toString());
        query.bindValue(SEND_VIEW,obj.value("send_view").toString());
        query.bindValue(SEND_MODE,obj.value("send_mode").toString());
        query.bindValue(SEND_CODE,obj.value("send_code").toString());
        query.bindValue(SEND_PRCE,obj.value("send_prce").toString());
        query.bindValue(SEND_MARK,obj.value("send_mark").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("replace into erp_sends_log values(?,?,?,?,?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(SEND_NUMB,obj.value("send_numb").toString());
    query.bindValue(SEND_VIEW,obj.value("send_view").toString());
    query.bindValue(SEND_MODE,obj.value("send_mode").toString());
    query.bindValue(SEND_CODE,obj.value("send_code").toString());
    query.bindValue(SEND_PRCE,obj.value("send_prce").toString());
    query.bindValue(SEND_MARK,obj.value("send_mark").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_sends");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::prodsJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_prods where logs_guid>:logs_guid");
        query.bindValue(":logs_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("logs_cmmd","erp_prods");
            send_obj.insert("tabs_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("prod_numb",query.value(PROD_NUMB).toString());
            send_obj.insert("prod_date",query.value(PROD_DATE).toString());
            send_obj.insert("prod_view",query.value(PROD_VIEW).toString());
            send_obj.insert("prod_cust",query.value(PROD_CUST).toString());
            send_obj.insert("prod_dead",query.value(PROD_DEAD).toString());
            send_obj.insert("prod_quan",query.value(PROD_QUAN).toString());
            send_obj.insert("prod_pnum",query.value(PROD_PNUM).toString());
            send_obj.insert("prod_type",query.value(PROD_TYPE).toString());
            send_obj.insert("prod_code",query.value(PROD_CODE).toString());
            send_obj.insert("prod_name",query.value(PROD_NAME).toString());
            send_obj.insert("prod_mode",query.value(PROD_MODE).toString());
            send_obj.insert("prod_mnum",query.value(PROD_MNUM).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_prods values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        query.bindValue(PROD_ID,tabs_guid);
        query.bindValue(PROD_GUID,logs_guid);
        query.bindValue(PROD_SIGN,logs_sign);
        query.bindValue(PROD_NUMB,obj.value("prod_numb").toString());
        query.bindValue(PROD_DATE,obj.value("prod_date").toString());
        query.bindValue(PROD_VIEW,obj.value("prod_view").toString());
        query.bindValue(PROD_CUST,obj.value("prod_cust").toString());
        query.bindValue(PROD_DEAD,obj.value("prod_dead").toString());
        query.bindValue(PROD_QUAN,obj.value("prod_quan").toString());
        query.bindValue(PROD_PNUM,obj.value("prod_pnum").toString());
        query.bindValue(PROD_TYPE,obj.value("prod_type").toString());
        query.bindValue(PROD_CODE,obj.value("prod_code").toString());
        query.bindValue(PROD_NAME,obj.value("prod_name").toString());
        query.bindValue(PROD_MODE,obj.value("prod_mode").toString());
        query.bindValue(PROD_MNUM,obj.value("prod_mnum").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("replace into erp_prods_log values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
    query.bindValue(0,logs_guid);
    query.bindValue(1,logs_sign);
    query.bindValue(2,tabs_guid);
    query.bindValue(PROD_NUMB,obj.value("prod_numb").toString());
    query.bindValue(PROD_DATE,obj.value("prod_date").toString());
    query.bindValue(PROD_VIEW,obj.value("prod_view").toString());
    query.bindValue(PROD_CUST,obj.value("prod_cust").toString());
    query.bindValue(PROD_DEAD,obj.value("prod_dead").toString());
    query.bindValue(PROD_QUAN,obj.value("prod_quan").toString());
    query.bindValue(PROD_PNUM,obj.value("prod_pnum").toString());
    query.bindValue(PROD_TYPE,obj.value("prod_type").toString());
    query.bindValue(PROD_CODE,obj.value("prod_code").toString());
    query.bindValue(PROD_NAME,obj.value("prod_name").toString());
    query.bindValue(PROD_MODE,obj.value("prod_mode").toString());
    query.bindValue(PROD_MNUM,obj.value("prod_mnum").toString());
    query.exec();

    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("logs_cmmd","erp_prods");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}
