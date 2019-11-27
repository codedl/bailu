#include "palette.h"

palette::palette(QWidget *parent)
    : QDialog(parent)
{
    createLeft();
    createRight();
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(leftFrame);
    mainLayout->addWidget(rightFrame);
}

palette::~palette()
{

}

void palette::createLeft(){
    windowLabel = new QLabel(tr("window"));
    windowComboBox = new QComboBox(this);
    fillcolorlist(windowComboBox);
    connect(windowComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ShowWindow()));

    leftFrame = new QFrame(this);
    QGridLayout *layout = new QGridLayout(leftFrame);
    layout->addWidget(windowLabel,0,0);
    layout->addWidget(windowComboBox,0,1);
}
void palette::fillcolorlist(QComboBox *comboBox){
    QStringList colors = QColor::colorNames();
    QString color;
    foreach (color, colors) {
        QPixmap pix(QSize(70,20));
        pix.fill(QColor(color));
        comboBox->addItem(QIcon(pix),NULL);
        comboBox->setIconSize(QSize(70,20));
        comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    }
}

void palette::ShowWindow(){
    QStringList colors = QColor::colorNames();
    QColor color = QColor(colors[windowComboBox->currentIndex()]);
    QPalette p = rightFrame->palette();
    p.setColor(QPalette::Window,color);
    rightFrame->setPalette(p);
    rightFrame->update();
}

void palette::createRight(){
    rightFrame = new QFrame(this);
    QGridLayout *layout = new QGridLayout(rightFrame);
    rightFrame->show();
}
