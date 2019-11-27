#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    //创建一个表
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("my.db");
    if(!db.open())
        QMessageBox::critical(0,"sqlite","open failed",QMessageBox::Close);
    QStringList list = db.connectionNames();

    foreach (QString str , list) {
        qDebug() << str;
    }
    QSqlQuery query(db);
    if(db.tables().contains("student")){
        qDebug()<<"student contained";
        if(query.exec("delete * from student"))
            qDebug()<<"delete succeed";
        else
            qDebug()<<"delete failed";}
    else{
        query.exec("create table student (id int primary key, "
                   "name varchar, course int)");
//        query.exec("delete * from student");
//        query.exec("insert into student values(1, 'jack', 11)");
//        query.exec("insert into student values(2, 'mary', 11)");
//        query.exec("insert into student values(3, 'tim', 12)");
    }
    if(db.tables().contains("userdata"))
        qDebug()<<"userdata contained";
    else {
        query.exec("create table userdata (id int primary key, "
                   "name varchar, data int)");
//        query.exec("delete * from student");
        //        query.exec("insert into userdata values(1, '10db', 11)");
        //        query.exec("insert into userdata values(2, '20db', 11)");
        //        query.exec("insert into userdata values(3, '30db', 12)");
    }

    ui->setupUi(this);
    //创建一个model，设置数据库和表,手动提交,遍历,然后显示
    model = new QSqlTableModel(this,db);
    model->setTable("student");
    model->query();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    for(int i=0; i<371; i++){
        model->insertRow(i);
        model->setData(model->index(i,0),i);
//        model->submitAll();
    }
    model->database().transaction();
    if(model->submitAll())
        model->database().commit();
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
    qDebug() << "rowcount:" << model->rowCount();
    ui->tableView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    model->database().transaction();
    if(model->submitAll()){
        model->database().commit();
        QMessageBox::information(this,tr("commit"),tr("data commit succeed!"),QMessageBox::Close);
    }else{
        model->database().rollback();
        QMessageBox::critical(this,tr("commit"),tr("data commit failed!"),QMessageBox::Cancel);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    model->revertAll();
}

void MainWindow::on_pushButton_3_clicked()
{
    QSqlQuery query(db);
    int rows = model->rowCount();
    //static int id = ++rows;//定义了一个static变量id，其值不会改变
    //id = ++rows;//rows改变，无法插入不连续的行;
    int id = rows+1;
    qDebug() << "id:" << id ;
    if(model->insertRow(rows))
        qDebug() << "insert succeed";
    else
        qDebug() << query.lastError().driverText();
    model->setData(model->index(rows,0), id);
    model->setData(model->index(rows,1), "dl");
    model->submitAll();
}

void MainWindow::on_pushButton_7_clicked()
{
    QString name = ui->lineEdit->text();
    model->setFilter(QString("name = '%1'").arg(name));
    model->select();
    qDebug() << "row:" << model->rowCount();//满足条件的行数
    for(int i=0; i<model->rowCount(); ++i){
        QSqlRecord record = model->record(i);//满足条件的行
        qDebug() << "id:" << record.value(0).toInt();
        qDebug() << "name:" << record.value(1).toString();
        qDebug() << "course:" << record.value(2).toInt();
    }

    model->setFilter("");
    model->select();
    qDebug() << "row:" << model->rowCount();
    for(int j=0; j<model->rowCount(); j++)
        for(int k=0; k<model->columnCount(); k++)
            qDebug() << "data:" << model->data(model->index(j,k));
}

void MainWindow::on_pushButton_8_clicked()
{
    model->setTable("student");
    model->select();
    //    ui->tableView->setModel(model);
}

void MainWindow::on_pushButton_5_clicked()
{
    model->setSort(0,Qt::AscendingOrder);
    model->select();
}

void MainWindow::on_pushButton_6_clicked()
{
    model->setSort(0,Qt::DescendingOrder);
    model->select();
}

void MainWindow::on_pushButton_4_clicked()
{
    int row = ui->tableView->currentIndex().row();
    model->removeRow(row);
    int ok = QMessageBox::warning(this,tr("remove"),tr("remove or not"),QMessageBox::Yes,QMessageBox::No);
    if(ok == QMessageBox::Yes){
        model->submitAll();
    }else{
        model->revert();
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    static int table = 1;
    table ^= 1;
    if(table)
        model->setTable("userdata");
    else
        model->setTable("student");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    ui->tableView->setModel(model);
}

void MainWindow::on_pushButton_10_clicked()
{
    int cols = model->columnCount();
    if(model->insertColumn(cols))
        qDebug() << "insert succeed";
    else
        qDebug() << "insert failed";
    if(model->setHeaderData(cols,Qt::Horizontal,"new"))
        qDebug()<<"setheaderdata succeed";
    else
        qDebug()<<"setheaderdata failed";
    //    model->setData(model->index(0,cols),0);
    if(model->submitAll())
        qDebug() << "submitall succeed";
    else
        qDebug()<<"submitAll failed";
    model->select();

}
