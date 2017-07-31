#include "SupplyServer.h"

SupplyServer::SupplyServer(QWidget *parent)
    : QWidget(parent)
{
    initUdp();
    initSql();
    //    initUI();
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
        qDebug() << "erp.db not exist";
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("erp.db");
    if (!db.open())
        qDebug() << "erp.db open fail";

    qint64 uuid = id.getId();

    createTabRoles(uuid);
    createTabUsers(uuid);
    createTabSales(uuid);
    createTabCusts(uuid);
    createTabOrder(uuid);
    createTabSends(uuid);
    createTabProds(uuid);
    createTabPurch(uuid);
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

    //    QTableView *tab = new QTableView(this);
    //    QVBoxLayout *lay = new QVBoxLayout;
    //    lay->addWidget(tab);
    //    this->setLayout(lay);
    //    this->resize(1024,768);
    //    QSqlTableModel *m_u8 = new QSqlTableModel(this,u8);
    //    m_u8->setTable("IA_Individual");
    //    m_u8->select();
    //    tab->setModel(m_u8);
}

void SupplyServer::recvNetJson(QJsonObject obj)
{
    QString cmd = obj.value("command").toString();
    if (cmd == "erp_login")
        excuteCmdLogin(obj);
    if (cmd == "erp_roles")
        excuteCmdRoles(obj);
    if (cmd == "erp_users")
        excuteCmdUsers(obj);
    if (cmd == "erp_sales")
        excuteCmdSales(obj);
    if (cmd == "erp_custs")
        excuteCmdCusts(obj);
    if (cmd == "erp_order")
        excuteCmdOrder(obj);
    if (cmd == "erp_sends")
        excuteCmdSends(obj);
    if (cmd == "erp_plans")
        plansJson(obj);
    if (cmd == "erp_prods")
        prodsJson(obj);
}

void SupplyServer::createTabRoles(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_roles");
    query.exec("drop table erp_roles_log");

    cmd = "create table if not exists erp_roles(";//创建角色表
    cmd += "role_uuid integer primary key,";//角色ID
    cmd += "role_guid interger,";//操作ID
    cmd += "role_sign interger,";//操作标识
    cmd += "role_name text,";//角色名称
    cmd += "role_mark text)";//角色备注
    if (!query.exec(cmd))
        qDebug() << "erp_roles create fail";

    cmd = "create table if not exists erp_roles_log(";//创建角色日志表
    cmd += "role_guid integer primary key,";//操作ID
    cmd += "role_sign integer,";//操作标识
    cmd += "role_uuid integer,";//角色ID
    cmd += "role_name text,";//角色名称
    cmd += "role_mark text)";//角色备注
    if (!query.exec(cmd))
        qDebug() << "erp_roles create fail";

    query.exec("select count(role_guid) from erp_roles");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_roles values(?,?,?,?,?)");//加入管理
        query.bindValue(0,uuid);//角色ID
        query.bindValue(1,id.getId());//操作ID
        query.bindValue(2,1);//操作标识
        query.bindValue(3,"管理");//角色名称
        query.bindValue(4,"系统管理员");//角色备注
        if (!query.exec())
            qDebug() << "erp_roles add fail";
    }
    query.clear();
}

void SupplyServer::excuteCmdRoles(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_roles where role_guid>:role_guid");
        query.bindValue(":role_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_roles");
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
}

void SupplyServer::createTabUsers(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_users");
    query.exec("drop table erp_users_log");

    cmd = "create table if not exists erp_users(";//创建用户表
    cmd += "user_uuid integer primary key,";
    cmd += "user_guid interger,";
    cmd += "user_sign interger,";
    cmd += "user_name text,";//用户名称
    cmd += "user_pass text,";//用户密码
    cmd += "user_role interger,";//用户角色ID
    cmd += "user_date text)";//加入日期
    if (!query.exec(cmd))
        qDebug() << "erp_users create fail";

    cmd = "create table if not exists erp_users_log(";//创建用户日志表
    cmd += "user_guid integer primary key,";
    cmd += "user_sign integer,";
    cmd += "user_uuid integer,";
    cmd += "user_name text,";
    cmd += "user_pass text,";
    cmd += "user_role interger,";//用户角色ID
    cmd += "user_date text)";
    if (!query.exec(cmd))
        qDebug() << "erp_users create fail";

    query.exec("select count(user_uuid) from erp_users");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_users values(?,?,?,?,?,?,?)");//加入管理员
        query.bindValue(0,uuid);
        query.bindValue(1,uuid);
        query.bindValue(2,1);
        query.bindValue(3,"admin");
        query.bindValue(4,"");
        query.bindValue(5,uuid);
        query.bindValue(6,"2017-07-24");
        if (!query.exec())
            qDebug() << "erp_users add fail";
    }
    query.clear();
}


void SupplyServer::excuteCmdUsers(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("logs_sign").toDouble();
    qint64 tabs_guid = obj.value("tabs_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_users where user_guid>:user_guid");
        query.bindValue(":user_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_users");
            send_obj.insert("user_guid",query.value(0).toDouble());
            send_obj.insert("logs_guid",query.value(1).toDouble());
            send_obj.insert("logs_sign",query.value(2).toDouble());
            send_obj.insert("user_name",query.value(3).toString());
            send_obj.insert("user_pass",query.value(4).toString());
            send_obj.insert("user_role",query.value(5).toDouble());
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
        query.bindValue(5,obj.value("user_role").toDouble());
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
    query.bindValue(5,obj.value("user_role").toDouble());
    query.bindValue(6,obj.value("user_date").toString());
    query.exec();

    query.clear();
}

void SupplyServer::createTabSales(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_sales");
    query.exec("drop table erp_sales_log");

    cmd = "create table if not exists erp_sales(";
    cmd += "sales_uuid integer primary key,";
    cmd += "sales_guid interger,";
    cmd += "sales_sign interger,";
    cmd += "sales_name text,";//销售名称
    cmd += "sales_area text)";//所属区域
    if (!query.exec(cmd))
        qDebug() << "erp_sales create fail";

    cmd = "create table if not exists erp_sales_log(";
    cmd += "sales_guid integer primary key,";
    cmd += "sales_sign integer,";
    cmd += "sales_uuid integer,";
    cmd += "sales_name text,";//销售名称
    cmd += "sales_area text)";//所属区域
    if (!query.exec(cmd))
        qDebug() << "erp_sales create fail";

    query.exec("select count(sales_uuid) from erp_sales");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_sales values(?,?,?,?,?)");//加入销售
        query.bindValue(0,uuid);
        query.bindValue(1,uuid);
        query.bindValue(2,1);
        query.bindValue(3,"销售");
        query.bindValue(4,"山东");
        if (!query.exec())
            qDebug() << "erp_sales add fail";
    }
    query.clear();
}


void SupplyServer::excuteCmdSales(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 sales_guid = id.getId();
    qint64 sales_sign = obj.value("sales_sign").toDouble();
    qint64 sales_uuid = obj.value("sales_uuid").toDouble();

    switch (sales_sign) {
    case 0://查询
        sales_guid = obj.value("sales_guid").toDouble();
        query.prepare("select * from erp_sales where sales_guid>:sales_guid");
        query.bindValue(":sales_guid",sales_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_sales");
            send_obj.insert("sales_uuid",query.value(0).toDouble());
            send_obj.insert("sales_guid",query.value(1).toDouble());
            send_obj.insert("sales_sign",query.value(2).toDouble());
            send_obj.insert("sales_name",query.value(3).toString());
            send_obj.insert("sales_area",query.value(4).toString());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        sales_uuid = sales_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_sales values(?,?,?,?,?)");
        query.bindValue(0,sales_uuid);
        query.bindValue(1,sales_guid);
        query.bindValue(2,sales_sign);
        query.bindValue(3,obj.value("sales_name").toString());
        query.bindValue(4,obj.value("sales_area").toString());
        if (!query.exec())
            qDebug() << "erp_sales replace fail";
        break;
    default:
        break;
    }
    query.prepare("insert into erp_sales_log values(?,?,?,?,?)");
    query.bindValue(0,sales_guid);
    query.bindValue(1,sales_sign);
    query.bindValue(2,sales_uuid);
    query.bindValue(3,obj.value("sales_name").toString());
    query.bindValue(4,obj.value("sales_area").toString());
    if (!query.exec())
        qDebug() << "erp_sales replace fail";

    query.clear();
}

void SupplyServer::createTabCusts(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_custs");
    query.exec("drop table erp_custs_log");

    cmd = "create table if not exists erp_custs(";
    cmd += "custs_uuid integer primary key,";
    cmd += "custs_guid integer,";
    cmd += "custs_sign integer,";
    cmd += "custs_name text,";//客户名称
    cmd += "custs_sale integer)";//销售ID
    if (!query.exec(cmd))
        qDebug() << "erp_custs create fail";

    cmd = "create table if not exists erp_custs_log(";
    cmd += "custs_guid integer primary key,";
    cmd += "custs_sign integer,";
    cmd += "custs_uuid integer,";
    cmd += "custs_name text,";
    cmd += "custs_sale integer)";
    if (!query.exec(cmd))
        qDebug() << "erp_custs create fail";

    query.exec("select count(custs_guid) from erp_custs");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_custs values(?,?,?,?,?)");//加入销售
        query.bindValue(0,uuid);
        query.bindValue(1,uuid);
        query.bindValue(2,1);
        query.bindValue(3,"青岛海尔");
        query.bindValue(4,uuid);
        if (!query.exec())
            qDebug() << "erp_custs add fail";
    }
    query.clear();
}


void SupplyServer::excuteCmdCusts(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 custs_guid = id.getId();
    qint64 custs_sign = obj.value("custs_sign").toDouble();
    qint64 custs_uuid = obj.value("custs_uuid").toDouble();

    switch (custs_sign) {
    case 0://查询
        custs_guid = obj.value("custs_guid").toDouble();
        query.prepare("select * from erp_custs where custs_guid>:custs_guid");
        query.bindValue(":custs_guid",custs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_custs");
            send_obj.insert("custs_uuid",query.value(0).toDouble());
            send_obj.insert("custs_guid",query.value(1).toDouble());
            send_obj.insert("custs_sign",query.value(2).toDouble());
            send_obj.insert("custs_name",query.value(3).toString());
            send_obj.insert("custs_sale",query.value(4).toDouble());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        custs_uuid = custs_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_custs values(?,?,?,?,?)");
        query.bindValue(0,custs_uuid);
        query.bindValue(1,custs_guid);
        query.bindValue(2,custs_sign);
        query.bindValue(3,obj.value("custs_name").toString());
        query.bindValue(4,obj.value("custs_sale").toDouble());
        if (!query.exec())
            qDebug() << "erp_custs replace fail";
        break;
    default:
        break;
    }
    query.prepare("insert into erp_custs_log values(?,?,?,?,?)");
    query.bindValue(0,custs_guid);
    query.bindValue(1,custs_sign);
    query.bindValue(2,custs_uuid);
    query.bindValue(3,obj.value("custs_name").toString());
    query.bindValue(4,obj.value("custs_sale").toDouble());
    if (!query.exec())
        qDebug() << "erp_custs replace fail";

    query.clear();
}

void SupplyServer::createTabOrder(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_order");
    query.exec("drop table erp_order_log");

    cmd = "create table if not exists erp_order(";
    cmd += "order_uuid integer primary key,";
    cmd += "order_guid integer,";
    cmd += "order_sign integer,";
    cmd += "order_numb text,";//订单编号
    cmd += "order_date text,";//订单日期
    cmd += "order_cust integer,";//客户信息
    cmd += "order_view text,";//评审编号
    cmd += "order_quan text,";//订货数量
    cmd += "order_dead text,";//交货日期
    cmd += "order_mark text)";//备注内容
    if (!query.exec(cmd))
        qDebug() << "erp_order create fail";

    cmd = "create table if not exists erp_order_log(";
    cmd += "order_guid integer primary key,";
    cmd += "order_sign integer,";
    cmd += "order_uuid integer,";
    cmd += "order_numb text,";//订单编号
    cmd += "order_date text,";//订单日期
    cmd += "order_cust integer,";//客户信息
    cmd += "order_view text,";//评审编号
    cmd += "order_quan text,";//订货数量
    cmd += "order_dead text,";//交货日期
    cmd += "order_mark text)";//备注内容
    if (!query.exec(cmd))
        qDebug() << "erp_order create fail" << uuid;

    query.exec("select count(order_uuid) from erp_order");
    query.next();
    if (query.value(0).toInt() == 0) {
        query.prepare("insert into erp_order values(?,?,?,?,?,?,?,?,?,?)");//加入测试订单
        query.bindValue(0,uuid);
        query.bindValue(1,uuid);
        query.bindValue(2,1);
        query.bindValue(3,"179907001");
        query.bindValue(4,"2017-07-01");
        query.bindValue(5,uuid);
        query.bindValue(6,"G179907001");
        query.bindValue(7,"5");
        query.bindValue(8,"2017-07-24");
        query.bindValue(9,"xx");
        if (!query.exec())
            qDebug() << "erp_order add fail";
    }

    query.clear();
}

void SupplyServer::excuteCmdOrder(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject objs;

    qint64 order_guid = id.getId();
    qint64 order_sign = obj.value("order_sign").toDouble();
    qint64 order_uuid = obj.value("order_uuid").toDouble();

    switch (order_sign) {
    case 0://查询
        order_guid = obj.value("order_guid").toDouble();
        query.prepare("select * from erp_order where order_guid>:order_guid");
        query.bindValue(":order_guid",order_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            objs.insert("sendto",obj.value("sender").toString());
            objs.insert("command","erp_order");
            objs.insert("order_uuid",query.value(0x00).toDouble());
            objs.insert("order_guid",query.value(0x01).toDouble());
            objs.insert("order_sign",query.value(0x02).toDouble());
            objs.insert("order_numb",query.value(0x03).toString());
            objs.insert("order_date",query.value(0x04).toString());
            objs.insert("order_cust",query.value(0x05).toDouble());
            objs.insert("order_view",query.value(0x06).toString());
            objs.insert("order_dead",query.value(0x07).toString());
            objs.insert("order_quan",query.value(0x08).toString());
            objs.insert("order_mark",query.value(0x09).toString());
            emit sendJson(objs);
        }
        return;
        break;
    case 1://增加
        order_uuid = order_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_order values(?,?,?,?,?,?,?,?,?,?)");
        query.bindValue(0x00,order_uuid);
        query.bindValue(0x01,order_guid);
        query.bindValue(0x02,order_sign);
        query.bindValue(0x03,obj.value("order_numb").toString());
        query.bindValue(0x04,obj.value("order_date").toString());
        query.bindValue(0x05,obj.value("order_cust").toDouble());
        query.bindValue(0x06,obj.value("order_view").toString());
        query.bindValue(0x07,obj.value("order_dead").toString());
        query.bindValue(0x08,obj.value("order_quan").toString());
        query.bindValue(0x09,obj.value("order_mark").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.prepare("replace into erp_order_log values(?,?,?,?,?,?,?,?,?,?)");
    query.bindValue(0x00,order_guid);
    query.bindValue(0x01,order_sign);
    query.bindValue(0x02,order_uuid);
    query.bindValue(0x03,obj.value("order_numb").toString());
    query.bindValue(0x04,obj.value("order_date").toString());
    query.bindValue(0x05,obj.value("order_cust").toDouble());
    query.bindValue(0x06,obj.value("order_view").toString());
    query.bindValue(0x07,obj.value("order_dead").toString());
    query.bindValue(0x08,obj.value("order_quan").toString());
    query.bindValue(0x09,obj.value("order_mark").toString());
    query.exec();
    query.clear();
}

void SupplyServer::createTabSends(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_sends");
    query.exec("drop table erp_sends_log");

    cmd = "create table if not exists erp_sends(";
    cmd += "sends_uuid integer primary key,";
    cmd += "sends_guid integer,";
    cmd += "sends_sign integer,";
    cmd += "sends_numb text,";//订单单号
    cmd += "sends_date text,";//发货日期
    cmd += "sends_cust text,";//客户名称
    cmd += "sends_mode text,";//发货方式
    cmd += "sends_code text,";//货运单号
    cmd += "sends_prce text,";//运费
    cmd += "sends_quan text,";//发货数量
    cmd += "sends_mark text)";//货运单号
    if (!query.exec(cmd))
        qDebug() << "erp_sends create fail";

    cmd = "create table if not exists erp_sends_log(";
    cmd += "sends_guid integer primary key,";
    cmd += "sends_sign integer,";
    cmd += "sends_uuid integer,";
    cmd += "sends_numb text,";//订单单号
    cmd += "sends_date text,";//发货日期
    cmd += "sends_cust text,";//客户名称
    cmd += "sends_mode text,";//发货方式
    cmd += "sends_code text,";//货运单号
    cmd += "sends_prce text,";//运费
    cmd += "sends_quan text,";//发货数量
    cmd += "sends_mark text)";//货运单号
    if (!query.exec(cmd))
        qDebug() << "erp_sends create fail" << uuid;

    query.clear();
}

void SupplyServer::excuteCmdSends(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject objs;

    qint64 sends_guid = id.getId();
    qint64 sends_sign = obj.value("sends_sign").toDouble();
    qint64 sends_uuid = obj.value("sends_uuid").toDouble();

    switch (sends_sign) {
    case 0://查询
        sends_guid = obj.value("sends_guid").toDouble();
        query.prepare("select * from erp_sends where sends_guid>:sends_guid");
        query.bindValue(":sends_guid",sends_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            objs.insert("sendto",obj.value("sender").toString());
            objs.insert("command","erp_sends");
            objs.insert("sends_uuid",query.value(0x00).toDouble());
            objs.insert("sends_guid",query.value(0x01).toDouble());
            objs.insert("sends_sign",query.value(0x02).toDouble());
            objs.insert("sends_numb",query.value(0x03).toString());
            objs.insert("sends_date",query.value(0x04).toString());
            objs.insert("sends_cust",query.value(0x05).toString());
            objs.insert("sends_mode",query.value(0x06).toString());
            objs.insert("sends_code",query.value(0x07).toString());
            objs.insert("sends_prce",query.value(0x08).toString());
            objs.insert("sends_quan",query.value(0x09).toString());
            objs.insert("sends_mark",query.value(0x0A).toString());
            emit sendJson(objs);
        }
        return;
        break;
    case 1://增加
        sends_uuid = sends_guid;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_sends values(?,?,?,?,?,?,?,?,?,?,?)");
        query.bindValue(0x00,sends_uuid);
        query.bindValue(0x01,sends_guid);
        query.bindValue(0x02,sends_sign);
        query.bindValue(0x03,obj.value("sends_numb").toString());
        query.bindValue(0x04,obj.value("sends_date").toString());
        query.bindValue(0x05,obj.value("sends_cust").toString());
        query.bindValue(0x06,obj.value("sends_mode").toString());
        query.bindValue(0x07,obj.value("sends_code").toString());
        query.bindValue(0x08,obj.value("sends_prce").toString());
        query.bindValue(0x09,obj.value("sends_quan").toString());
        query.bindValue(0x0A,obj.value("sends_mark").toString());
        if (!query.exec())
            qDebug() << "replace erp_sends fail";
        break;
    default:
        break;
    }
    query.clear();
}

void SupplyServer::createTabProds(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    query.exec("drop table erp_plans");
    query.exec("drop table erp_prods");
    query.exec("drop table erp_prods_log");

    cmd = "create table if not exists erp_plans(";//创建生产表
    cmd += "plan_uuid integer primary key,";//生产计划ID
    cmd += "plan_guid integer,";//操作ID
    cmd += "plan_sign integer,";//操作标识
    cmd += "plan_numb text,";//生产单号
    cmd += "plan_quan text,";//生产数量
    cmd += "plan_type text,";//产品大类
    cmd += "plan_code text,";//产品编号
    cmd += "plan_name text,";//产品名称
    cmd += "plan_mode text,";//产品规格
    cmd += "plan_mnum text)";//仪表编号
    if (!query.exec(cmd))
        qDebug() << "erp_plans create fail";

    cmd = "create table if not exists erp_prods(";//创建生产详细表
    cmd += "prod_uuid integer primary key,";//生产详细ID
    cmd += "prod_guid integer,";//操作ID
    cmd += "prod_sign integer,";//操作标识
    cmd += "prod_puid integer,";//生产计划ID
    cmd += "prod_ouid integer,";//订单ID
    cmd += "prod_mnum text)";//仪表编号
    if (!query.exec(cmd))
        qDebug() << "erp_prods create fail" << uuid;
}

void SupplyServer::createTabPurch(qint64 uuid)
{
    QSqlQuery query(db);
    QString cmd;

    cmd = "create table if not exists erp_purch(";
    cmd += "prch_uuid integer primary key,";
    cmd += "prch_guid interger,";
    cmd += "prch_sign interger,";
    cmd += "prch_numb text,";
    cmd += "prch_code text,";
    cmd += "prch_name text,";
    cmd += "prch_unit text,";
    cmd += "prch_lack text,";
    cmd += "prch_quan text,";
    cmd += "prch_date text,";
    cmd += "prch_bout text,";
    cmd += "prch_expt text,";
    cmd += "prch_real text,";
    cmd += "prch_come text,";
    cmd += "prch_oway text,";
    cmd += "prch_ofix text,";
    cmd += "prch_mark text)";
    if (!query.exec(cmd))
        qDebug() << "erp_purch create fail" << uuid;
}



void SupplyServer::excuteCmdLogin(QJsonObject obj)
{
    QSqlQuery query(db);
    QString user_name = obj.value("user_name").toString();
    QString user_pass = obj.value("user_pass").toString();
    bool logs_stat = false;
    query.prepare("select user_pass from erp_users where user_name==:user_name");
    query.bindValue(":user_name",user_name);
    if (!query.exec())
        qDebug() << "user not found";
    if (query.next()) {//将最新的操作记录返回
        if (user_pass == query.value(0).toString())
            logs_stat = true;
    }
    QJsonObject send_obj;
    send_obj.insert("sendto",obj.value("sender").toString());
    send_obj.insert("command","erp_login");
    send_obj.insert("logs_stat",logs_stat);

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
            send_obj.insert("command","erp_order");
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
    send_obj.insert("command","erp_order");
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
            send_obj.insert("command","erp_sends");
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
    send_obj.insert("command","erp_sends");
    send_obj.insert("logs_sign",0);
    sendJson(send_obj);
}

void SupplyServer::plansJson(QJsonObject obj)
{

}

void SupplyServer::prodsJson(QJsonObject obj)
{
    QSqlQuery query(db);
    QJsonObject send_obj;

    qint64 logs_guid = id.getId();
    qint64 logs_sign = obj.value("prod_sign").toDouble();
    qint64 tabs_guid = obj.value("prod_guid").toDouble();

    switch (logs_sign) {
    case 0://查询
        logs_guid = obj.value("logs_guid").toDouble();
        query.prepare("select * from erp_prods where prod_guid>:prod_guid");
        query.bindValue(":prod_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_prods");
            send_obj.insert("prod_uuid",query.value(0).toDouble());
            send_obj.insert("prod_guid",query.value(1).toDouble());
            send_obj.insert("prod_sign",query.value(2).toDouble());
            send_obj.insert("prod_numb",query.value(PROD_NUMB).toString());
            send_obj.insert("prod_quan",query.value(PROD_QUAN).toString());
            send_obj.insert("prod_type",query.value(PROD_TYPE).toString());
            send_obj.insert("prod_code",query.value(PROD_CODE).toString());
            send_obj.insert("prod_name",query.value(PROD_NAME).toString());
            send_obj.insert("prod_mode",query.value(PROD_MODE).toString());
            send_obj.insert("prod_mnum",query.value(PROD_MNUM).toString());
            emit sendJson(send_obj);
        }
        query.prepare("select * from erp_prods where prod_guid>:prod_guid");
        query.bindValue(":prod_guid",logs_guid);
        query.exec();
        while (query.next()) {//将最新的操作记录返回
            send_obj.insert("sendto",obj.value("sender").toString());
            send_obj.insert("command","erp_prods");
            send_obj.insert("prods_uuid",query.value(0).toDouble());
            send_obj.insert("prods_guid",query.value(1).toDouble());
            send_obj.insert("prods_sign",query.value(2).toDouble());
            send_obj.insert("prods_puid",query.value(3).toDouble());
            send_obj.insert("prods_ouid",query.value(4).toDouble());
            send_obj.insert("prods_mnum",query.value(5).toDouble());
            emit sendJson(send_obj);
        }
        return;
        break;
    case 1://增加
        tabs_guid = logs_guid;
        query.prepare("replace into erp_prod values(?,?,?,?,?,?,?,?,?,?)");
        query.bindValue(PROD_UUID,tabs_guid);
        query.bindValue(PROD_GUID,logs_guid);
        query.bindValue(PROD_SIGN,logs_sign);
        query.bindValue(PROD_NUMB,obj.value("prod_numb").toString());
        query.bindValue(PROD_QUAN,obj.value("prod_quan").toString());
        query.bindValue(PROD_TYPE,obj.value("prod_type").toString());
        query.bindValue(PROD_CODE,obj.value("prod_code").toString());
        query.bindValue(PROD_NAME,obj.value("prod_name").toString());
        query.bindValue(PROD_MODE,obj.value("prod_mode").toString());
        query.bindValue(PROD_MNUM,obj.value("prod_mnum").toString());
        query.exec();
        qDebug() << obj.value("prod_quan").toInt();
        for (int i=0; i < obj.value("prod_quan").toInt(); i++) {
            query.prepare("replace into erp_prods values(?,?,?,?,?,?)");
            query.bindValue(0,id.getId());
            query.bindValue(1,logs_guid);
            query.bindValue(2,logs_sign);
            query.bindValue(3,tabs_guid);
            query.bindValue(4,0);
            query.bindValue(5,obj.value("prod_mnum").toInt()+i);
            query.exec();
        }
        break;
    case 2://删除
    case 3://修改
        query.prepare("replace into erp_prods values(?,?,?,?,?,?)");
        query.bindValue(0,tabs_guid);
        query.bindValue(1,logs_guid);
        query.bindValue(2,logs_sign);
        query.bindValue(3,obj.value("prod_uuid").toDouble());
        query.bindValue(4,obj.value("prod_ouid").toDouble());
        query.bindValue(5,obj.value("prod_mnum").toString());
        query.exec();
        break;
    default:
        break;
    }
    query.clear();
}
