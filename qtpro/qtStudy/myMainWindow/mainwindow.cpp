#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myaction.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMenu * editmenu = ui->menuBar->addMenu(tr("编辑&E"));
    QAction *openaction=editmenu->addAction(QIcon(":/images/images/open.png"),tr("打开文件(&O)"));
    openaction->setShortcut( QKeySequence("Ctrl+O"));
    ui->mainToolBar->addAction(openaction);

    QActionGroup *group = new QActionGroup(this);
    QAction *actionL = group->addAction(tr("左对齐(&L)"));
    QAction *actionR = group->addAction(tr("右对齐(&R)"));
    QAction *actionC = group->addAction(tr("居中(&C)"));
    actionL->setCheckable(true);
    actionR->setCheckable(true);
    actionC->setCheckable(true);

    actionL->setChecked(true);

    editmenu->addSeparator();//添加间隔下划线
    editmenu->addAction(actionL);
    editmenu->addAction(actionR);
    editmenu->addAction(actionC);

    QToolButton *toolBtn = new QToolButton(this);
    toolBtn->setText(tr("颜色"));
    QMenu *colormenu = new QMenu(this);
    colormenu->addAction(tr("红色"));
    colormenu->addAction(tr("绿色"));
    toolBtn->setMenu(colormenu);
    toolBtn->setPopupMode(QToolButton::MenuButtonPopup);
    ui->mainToolBar->addWidget(toolBtn);

    QSpinBox *spin = new QSpinBox(this);
    ui->mainToolBar->addWidget(spin);

    ui->statusBar->showMessage( tr("欢迎使用多文档编辑器"),3000);
    QLabel *label = new QLabel(this);
    label->setText(tr("www.qter.org"));
    label->setFrameStyle(QFrame::Box | QFrame::Sunken);
//    ui->statusBar->addWidget(label);
    ui->statusBar->addPermanentWidget(label);

    myaction *action = new myaction;
    editmenu->addAction(action);
    connect(action,SIGNAL(getText(QString)),this,SLOT(setText(QString)));

    //设置文本格式
    QTextDocument *document = ui->textEdit->document();
    QTextFrame *frame = document->rootFrame();
    QTextFrameFormat format;
    format.setBorderBrush(Qt::red);
    format.setBorder(3);
    frame->setFrameFormat(format);

    QTextFrameFormat frameFormat;
    frameFormat.setMargin(5);
    frameFormat.setBackground(Qt::lightGray);
    frameFormat.setPadding(5);
    frameFormat.setBorder(3);
    frameFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Dotted);
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.insertFrame(frameFormat);

    QAction *textAction = new QAction(tr("文本框架"),this);
    ui->mainToolBar->addAction(textAction);
    connect(textAction,SIGNAL(triggered()),this,SLOT(showTextFrame()));

    QAction *blockAction = new QAction(tr("文本块"),this);
    ui->mainToolBar->addAction(blockAction);
    connect(blockAction,SIGNAL(triggered()),this,SLOT(showTextBlock()));

    QAction *fontAction = new QAction(tr("字体"),this);
    fontAction->setCheckable(true);
    ui->mainToolBar->addAction(fontAction);
    connect(fontAction,&QAction::toggled,this,&MainWindow::setTextFont);

    QAction *listAction = new QAction(tr("列表"),this);
    QAction *tableAction = new QAction(tr("表格"),this);
    QAction *imageAction = new QAction(tr("图片"),this);
    ui->mainToolBar->addAction(listAction);
    ui->mainToolBar->addAction(tableAction);
    ui->mainToolBar->addAction(imageAction);
    connect(listAction,&QAction::triggered,this,&MainWindow::insertList);
    connect(tableAction,&QAction::triggered,this,&MainWindow::insertTable);
    connect(imageAction,&QAction::triggered,this,&MainWindow::insertImage);

    QAction *findAction = new QAction(tr("查找"),this);
    ui->mainToolBar->addAction(findAction);
    connect(findAction,&QAction::triggered,this,&MainWindow::textFind);

    dialog = new QDialog(this);
    lineEdit = new QLineEdit(dialog);
    QPushButton *btn = new QPushButton(dialog);
    btn->setText(tr("查找下一个"));
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(lineEdit);
    layout->addWidget(btn);
    dialog->setLayout(layout);
    connect(btn,&QPushButton::clicked,this,&MainWindow::findNext);

    lighter = new mysyntaxhighlighter(ui->textEdit->document());//语法高亮

    ui->textEdit->append(tr("<h1><font color = red>使用html</fond></h1>"));

    setAcceptDrops(true);
    QLabel *imgLabel = new QLabel(this);
    QPixmap pix(":/images/images/open.png");
    imgLabel->setPixmap(pix);
    imgLabel->resize(pix.size());
    imgLabel->move(100,100);
    imgLabel->setAttribute(Qt::WA_DeleteOnClose);

    QAction *printAction = new QAction(tr("打印"),this);
    QAction *previewAction = new QAction(tr("预览"),this);
    QAction *pdfAction = new QAction(tr("生成pdf"),this);
    ui->mainToolBar->addAction(printAction);
    ui->mainToolBar->addAction(previewAction);
    ui->mainToolBar->addAction(pdfAction);
    connect(printAction,&QAction::triggered,this,&MainWindow::doPrint);
    connect(previewAction,&QAction::triggered,this,&MainWindow::doPrintPreview);
    connect(pdfAction,&QAction::triggered,this,&MainWindow::createPdf);
}

MainWindow::~MainWindow()
{
    delete ui;
}
/*拖动打开文件*/
//void MainWindow::dragEnterEvent(QDragEnterEvent *event)
//{
//    if(event->mimeData()->hasUrls())
//        event->acceptProposedAction();
//    else event->ignore();
//}

//void MainWindow::dropEvent(QDropEvent *event)
//{
//    const QMimeData *mimeData = event->mimeData();
//    if(mimeData->hasUrls()){
//        QList<QUrl> urlLists = mimeData->urls();
//        for(int i=0; i<urlLists.size(); i++)
//            qDebug() << urlLists.at(i);
//        QString filename = urlLists.at(0).toLocalFile();
//        if(!filename.isEmpty()){
//            QFile file(filename);
//            if(!file.open(QIODevice::ReadOnly)) return;
//            QTextStream in(&file);
//            ui->textEdit->setText(in.readAll());
//        }
//    }
//}

void MainWindow::on_action_New_triggered()
{
    QTextEdit *edit = new QTextEdit(this);
    QMdiSubWindow *child = ui->mdiArea->addSubWindow(edit);
    child->setWindowTitle(tr("多文档编辑器窗口"));
    child->show();
}

void MainWindow::on_action_triggered()
{
    ui->dockWidget->show();
}

void MainWindow::setText(const QString &string)
{
    ui->textEdit->setText(string);
}

void MainWindow::showTextFrame()
{
    QTextDocument *document = ui->textEdit->document();
    QTextFrame *rootFrame = document->rootFrame();
    QTextFrame::iterator it;
    for(it = rootFrame->begin(); !(it.atEnd()); ++it){
        QTextFrame *childFrame = it.currentFrame();
        QTextBlock block = it.currentBlock();
        if(childFrame)
            qDebug() << "frame";
        if(block.isValid())
            qDebug() << "block:" << block.text();
    }
}

void MainWindow::showTextBlock()
{
    QTextDocument *document = ui->textEdit->document();
    QTextBlock block = document->firstBlock();
    for(int i=0; i<document->blockCount(); i++){
        qDebug() << tr("block:%1;line:%2,length:%3;text:%4").arg(i).arg(block.firstLineNumber())
                    .arg(block.length()).arg(block.text());
        block = block.next();
    }
}

void MainWindow::setTextFont(bool checked)
{
    if(checked){
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);
    cursor.insertBlock(blockFormat);
    QTextCharFormat charFormat;
    charFormat.setBackground(Qt::lightGray);
    charFormat.setForeground(Qt::blue);
    charFormat.setFont(QFont(tr("宋体"),12,QFont::Bold,true));
    charFormat.setFontUnderline(true);
    cursor.setCharFormat(charFormat);
    cursor.insertText(tr("测试字体"));
    }else{}
}

void MainWindow::insertList()
{
    qDebug() << "insertList";
    QTextListFormat format;
    format.setStyle(QTextListFormat::ListDecimal);
    ui->textEdit->textCursor().insertList(format);
}

void MainWindow::insertTable()
{
    qDebug() << "insertTable";
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextTableFormat format;
    format.setCellSpacing(2);
    format.setCellPadding(5);
    cursor.insertTable(2,2,format);
}

void MainWindow::insertImage()
{
    qDebug() << "insertImage";
    QTextImageFormat format;
    format.setName(":/images/images/open.png");
    ui->textEdit->textCursor().insertImage(format);
}

void MainWindow::textFind()
{
    dialog->show();
}

void MainWindow::findNext()
{
    QString string = lineEdit->text();
    bool isfind = ui->textEdit->find(string,QTextDocument::FindBackward);
    if(isfind)
        qDebug() << tr("line:%1,col:%2")
                    .arg(ui->textEdit->textCursor().blockNumber())
                    .arg(ui->textEdit->textCursor().columnNumber());
}

void MainWindow::doPrint()
{
    QPrinter printer;
    QPrintDialog dialog(&printer,this);
    if(ui->textEdit->textCursor().hasSelection())
        dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if(dialog.exec() == QDialog::Accepted)
        ui->textEdit->print(&printer);
}

void MainWindow::doPrintPreview()
{
    QPrinter printer;
    QPrintPreviewDialog dialog(&printer,this);
    connect(&dialog,&QPrintPreviewDialog::paintRequested,this,&MainWindow::PrintPreview);
    dialog.exec();
}

void MainWindow::PrintPreview(QPrinter *printer)
{
    ui->textEdit->print(printer);
}

void MainWindow::createPdf()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("导出pdf文件"),QString(),"*.pdf");
    if(!fileName.isEmpty()){
        if(QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer;
        printer.setOutputFileName(fileName);
        printer.setOutputFormat(QPrinter::PdfFormat);
        ui->textEdit->print(&printer);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)   //鼠标按下事件
{
    // 第一步：获取图片
    // 将鼠标指针所在位置的部件强制转换为QLabel类型
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if(!child->inherits("QLabel")) return; // 如果部件不是QLabel则直接返回
    QPixmap pixmap = *child->pixmap();     // 获取QLabel中的图片

    // 第二步：自定义MIME类型
    QByteArray itemData;                                     // 创建字节数组
    QDataStream dataStream(&itemData, QIODevice::WriteOnly); // 创建数据流
    // 将图片信息，位置信息输入到字节数组中
    dataStream << pixmap << QPoint(event->pos() - child->pos());

    // 第三步：将数据放入QMimeData中
    QMimeData *mimeData = new QMimeData;  // 创建QMimeData用来存放要移动的数据
    // 将字节数组放入QMimeData中，这里的MIME类型是我们自己定义的
    mimeData->setData("myimage/png", itemData);

    // 第四步：将QMimeData数据放入QDrag中
    QDrag *drag = new QDrag(this);      // 创建QDrag，它用来移动数据
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);//在移动过程中显示图片，若不设置则默认显示一个小矩形
    drag->setHotSpot(event->pos() - child->pos()); // 拖动时鼠标指针的位置不变

    // 第五步：给原图片添加阴影
    QPixmap tempPixmap = pixmap; // 使原图片添加阴影
    QPainter painter;            // 创建QPainter，用来绘制QPixmap
    painter.begin(&tempPixmap);
    // 在图片的外接矩形中添加一层透明的淡黑色形成阴影效果
    painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
    painter.end();
    child->setPixmap(tempPixmap); // 在移动图片过程中，让原图片添加一层黑色阴影

    // 第六步：执行拖放操作
    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction)
            == Qt::MoveAction)  // 设置拖放可以是移动和复制操作，默认是复制操作
        child->close();         // 如果是移动操作，那么拖放完成后关闭原标签
    else {
        child->show();            // 如果是复制操作，那么拖放完成后显示标签
        child->setPixmap(pixmap); // 显示原图片，不再使用阴影
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) // 拖动进入事件
{
      // 如果有我们定义的MIME类型数据，则进行移动操作
     if (event->mimeData()->hasFormat("myimage/png")) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
     } else {
         event->ignore();
     }
}
void MainWindow::dragMoveEvent(QDragMoveEvent *event)   // 拖动事件
{
     if (event->mimeData()->hasFormat("myimage/png")) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
     } else {
         event->ignore();
     }
}

void MainWindow::dropEvent(QDropEvent *event) // 放下事件
{
    if (event->mimeData()->hasFormat("myimage/png")) {
         QByteArray itemData = event->mimeData()->data("myimage/png");
         QDataStream dataStream(&itemData, QIODevice::ReadOnly);
         QPixmap pixmap;
         QPoint offset;
         // 使用数据流将字节数组中的数据读入到QPixmap和QPoint变量中
         dataStream >> pixmap >> offset;
         // 新建标签，为其添加图片，并根据图片大小设置标签的大小
         QLabel *newLabel = new QLabel(this);
         newLabel->setPixmap(pixmap);
         newLabel->resize(pixmap.size());
   // 让图片移动到放下的位置，不设置的话，图片会默认显示在(0,0)点即窗口左上角
         newLabel->move(event->pos() - offset);
         newLabel->show();
         newLabel->setAttribute(Qt::WA_DeleteOnClose);
         event->setDropAction(Qt::MoveAction);
         event->accept();
     } else {
         event->ignore();
     }
}
