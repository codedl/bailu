#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QFileSystemModel *filemodel = new QFileSystemModel;
    filemodel->setRootPath(QDir::currentPath());

    QTreeView *tree = new QTreeView;//树形视图
    tree->setModel(filemodel);
    tree->setRootIndex(filemodel->index(QDir::currentPath()));
    tree->show();
}

void Widget::on_pushButton_2_clicked()
{
    QFileSystemModel *filemodel = new QFileSystemModel;
    filemodel->setRootPath(QDir::currentPath());

    QListView *list = new QListView;//列表视图
    list->setModel(filemodel);
    list->setRootIndex(filemodel->index(QDir::currentPath()));
    list->show();
}

void Widget::on_pushButton_3_clicked()
{
    QStandardItemModel *itemModel = new QStandardItemModel;
    QStandardItem *parentItem = itemModel->invisibleRootItem();
    //creator item0
    QStandardItem *item0 = new QStandardItem;
    item0->setText("A");
    QPixmap pix0(50,50);
    pix0.fill("red");
    item0->setIcon(pix0);
    item0->setToolTip("indexA");
    parentItem->appendRow(item0);//add to model

    QStandardItem *item1 = new QStandardItem;
    item1->setText("B");
    QPixmap pix1(50,50);
    pix1.fill("blue");
    item1->setIcon(pix1);
    item1->setToolTip("indexB");
    parentItem->appendRow(item1);

    QStandardItem *item2 = new QStandardItem;
    QPixmap pix2(50,50);
    pix2.fill("green");
    item2->setData("C",Qt::EditRole);
    item2->setData("indexC",Qt::ToolTipRole);
    item2->setData(QIcon(pix2),Qt::DecorationRole);
    parentItem->appendRow(item2);

    QTreeView *treeView = new QTreeView;
    treeView->setModel(itemModel);
    treeView->show();

    QModelIndex indexA = itemModel->index(0,0,QModelIndex());
    qDebug() << "indexA row count:" << itemModel->rowCount(indexA);
    QModelIndex indexB = itemModel->index(0,0,indexA);
    qDebug() << "indexB text:" << itemModel->data(indexB,Qt::EditRole).toString();
    qDebug() << "indexB toolTip:" << itemModel->data(indexB,Qt::ToolTipRole).toString();

}

void Widget::on_pushButton_4_clicked()
{
    QListWidget *listWidget = new QListWidget;
    new QListWidgetItem("a",listWidget);
    QListWidgetItem *listItem = new QListWidgetItem;
    listItem->setText("b");
    listItem->setToolTip("this is b");
    listWidget->insertItem(1,listItem);
    listWidget->sortItems(Qt::DescendingOrder);
    listWidget->show();
}

void Widget::on_pushButton_5_clicked()
{
    QTreeWidget *treeWidget = new QTreeWidget;
    treeWidget->setColumnCount(2);
    QStringList headers;
    headers << "name" << "years";
    treeWidget->setHeaderLabels(headers);
    QTreeWidgetItem *grade1 = new QTreeWidgetItem(treeWidget);
    grade1->setText(0,"Grade1");
    QTreeWidgetItem *student = new QTreeWidgetItem(grade1);
    student->setText(0,"Tom");
    student->setText(1,"1986");
    QTreeWidgetItem *grade2 = new QTreeWidgetItem(treeWidget,grade1);
    grade2->setText(1,"Grade2");
    treeWidget->show();
}

void Widget::on_pushButton_6_clicked()
{
    QTableWidget *tableWidget = new QTableWidget(3,2);
    QTableWidgetItem *item =  new QTableWidgetItem("qt");
    tableWidget->setItem(1,1,item);
    QTableWidgetItem *headv = new QTableWidgetItem("first");
    tableWidget->setVerticalHeaderItem(0,headv);
    QTableWidgetItem *headh = new QTableWidgetItem("ID");
    tableWidget->setHorizontalHeaderItem(0,headh);
    tableWidget->show();
}
