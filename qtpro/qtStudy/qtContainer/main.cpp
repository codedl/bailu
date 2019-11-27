#include <QDebug>

int main(void)
{
    QList<int> list;
    list << 1 << 2 << 3 << 4 << 5;
    QListIterator<int> i (list);
    while(i.hasNext()){
        qDebug()<<i.next();
    }
    i.toBack();
    while(i.hasPrevious()){
        qDebug()<<i.previous();
    }
    list.clear();
    QMutableListIterator<int> mlt (list);
    for(int j=0; j<10; ++j){
        mlt.insert(j);
    }
    for(mlt.toFront();mlt.hasNext();)
        qDebug()<<mlt.next();
    for(mlt.toBack();mlt.hasPrevious();){
        if(mlt.previous() %2 ==0)
            mlt.remove();
        else
            mlt.setValue(mlt.peekNext()*10);

    }
    for(mlt.toFront();mlt.hasNext();)
        qDebug()<<mlt.next();

    list.clear();
    for(int i=0; i<10; i++){
        list.insert(list.end(),i);
    }
    QList<int>::iterator it;
    for(it=list.begin();it!=list.end();++it){
        qDebug() << (*it);
        *it *= 10;
    }
    QList<int>::const_iterator ci;
    for(ci=list.constBegin();ci!=list.constEnd();++ci){
        qDebug()<<*ci;
    }

    QMap<QString, QString> map;
    map.insert("anhui","100");
    map.insert("anqing","200");
    map.insert("tongcheng","300");
    QMapIterator<QString, QString>mit(map);
    while(mit.hasNext())
        qDebug()<<" "<<mit.key() <<" "<<mit.next().value();

    QMutableMapIterator<QString,QString>mmi(map);
    if(mmi.findNext("200")){
        qDebug()<<mmi.key();
        mmi.setValue("150");
    }
    else
        qDebug()<<"not find next";
    QMapIterator<QString, QString>nmit(map);
    for(nmit.toFront();nmit.hasNext();)
        qDebug()<<" "<<nmit.key() <<" "<<nmit.next().value();

    return 0;
}
