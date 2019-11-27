#include <QCoreApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//添加数据库驱动
    db.setDatabaseName("mysql.db");//数据库命名
    if(db.open())//打开数据库
        qDebug()<<QObject::tr("open succeed");
    else
        qDebug()<<QObject::tr("open failed");

    QSqlQuery query(db);

    query.exec("create table mytable(id int primary key,name varchar(20))");
    query.exec("insert into mytable values(0,'dingle')");
    query.exec("insert into mytable values(1,'xiaolan')");
    query.exec("insert into mytable values(2,'baobei')");
    query.exec("select * from mytable");
    qDebug()<<QObject::tr("select succeed");
    while(query.next()){
        //遍历数据库
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }
    //获取数据库行数
    int rows;
    if(db.driver()->hasFeature(QSqlDriver::QuerySize)){
        rows = query.size();
        qDebug()<<QObject::tr("hasFeature")<<rows;
    }else{
        query.last();
        rows = query.at()+1;
        qDebug()<<QObject::tr("not hasFeature")<<rows;
    }

    query.seek(1);
    qDebug()<<QObject::tr("current index:")<<query.at();
    QSqlRecord record = query.record();
    qDebug()<<record.value("id").toInt()<<record.value("name").toString();
    QSqlField field = record.field(1);
    qDebug()<<field.name()<<field.value().toString();

    query.last();
    int id = query.value(0).toInt() + 1;
    qDebug()<<QObject::tr("last id:") <<id;

    query.prepare("insert into mytable(id, name) values(:id, :name)");
    QString name = "laopo";
    query.bindValue(":id",id);
    query.bindValue(":name",name);
    bool ok = query.exec();
    if(ok)
        qDebug()<<QObject::tr("insert succeed");
    else
        qDebug()<<QObject::tr("insert failed");

    query.exec("select * from mytable");
    ok = query.first();
    while(query.next()){
        //遍历数据库
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }

    query.prepare("insert into mytable(id, name) values(?, ?)");
    QVariantList ids;
    ids << ++id << ++id << ++id;
    query.addBindValue(ids);
    QVariantList names;
    names << "dl" << "wql" << "love";
    query.addBindValue(names);
    ok = query.execBatch();
    if(ok)
        qDebug()<<QObject::tr("QVariantList insert succeed");
    else
        qDebug()<< query.lastError().driverText();

    query.exec("select * from mytable");
    ok = query.first();
    while(query.next()){
        //遍历数据库
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }

    query.exec("update mytable set name = 'girl' where id > 2");//更新
    query.exec("select * from mytable");
    ok = query.first();
    while(query.next()){
        //遍历数据库
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }

    query.exec("delete from mytable where id > 5");
    query.exec("select * from mytable");
    ok = query.first();
    while(query.next()){
        //遍历数据库
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }

    if(db.driver()->hasFeature(QSqlDriver::Transactions)){
        qDebug()<<QObject::tr("has Transactions");
        db.transaction();
        query.exec("insert into mytable values(100,'years')");
        query.exec("select * from mytable");
        ok = query.first();
        while(query.next()){
            //遍历数据库
            qDebug()<<query.value(0).toInt()<<query.value(1).toString();
        }
        db.commit();
    }else{
        qDebug()<<QObject::tr("not has Transactions");
    }

    if(query.exec("select * from mytable where id = 2")){
        query.next();
        qDebug()<<query.value(0).toInt()<<query.value(1).toString();
    }else
        qDebug() << QObject::tr("select failed");
    return a.exec();
}
