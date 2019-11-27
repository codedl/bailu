/********************************************************************************
** Spectrum Analyzer Menus Created From Xml File
**
**  Created: 2011-11-01 10:00:00
**       by: Qt Creator 1.3.1
**   Author: ChangHouXi
** Mofified:
**
** Warning! no full completed
********************************************************************************/

#ifndef BASEMENU_H
#define BASEMENU_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtXml/QDomDocument>
#include <QtXml/QXmlStreamReader>

#include "../common/global.h"

//菜单基类
class tBaseMenu : public QObject
{
    Q_OBJECT
  public:
    tBaseMenu();                                              //构造
    ~tBaseMenu();                                             //析构

    bool isNormal;                                            //自检结果
  protected:
    void resetMainKeyProperty(mainKeyDef* key);               //初始化主菜单
    void resetSubKeyProperty(subKeyDef* key);                 //初始化子菜单

    void getSubKeyList(subKeyDef* key, QDomNode* node);       //获取子菜单列表
    bool getSystemMenu(void);                                 //获取系统菜单

    void setMainKeyProperty(mainKeyDef* key, QDomNode node);  //设置主键属性
    void setSubKeyProperty(subKeyDef* key, QDomNode node);    //设置子键属性

    bool loadFromXmlFile(QString xmlFileName);                //加载XML文件
  private:
    QString fileName;                                         //文件名称
    QString levelStr;                                         //等级字符串
    int nodeLevel;                                            //节点等级(从0开始)
    mainKeyDef* mainKey;                                      //主键
    subKeyDef* subKey;                                        //子键

    void printSubKeyList(subKeyDef* key);                     //打印子菜单
    void printSysMenu(void);                                  //打印系统菜单
};

#endif
