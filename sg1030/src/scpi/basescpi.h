/*
 * scpi.h
 *
 *  Created on: 2011-12-7
 *      Author: Administrator
 */

#ifndef BASESCPI_H_
#define BASESCPI_H_

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "command.h"
#include "../common/global.h"

//SCPI基类
class tBaseScpi : public QObject
{
    Q_OBJECT
  public:
    tBaseScpi();    //构造
    ~tBaseScpi();   //析构

    resultDef GetFunctionCode(QString str);
  protected:
    pCSTree root_tree;
    pCSTree prvNode;

    void initParseCmd(void);
    void CreatTree(pCSTree &tree, int num);
    void UpdataTree(pCSTree &tree, int num);
    CSNode* InsertChild(pCSTree &tree,pCSTree & element);
    char DeleteChild();
    int parseStringCmd(_SCPI_keyboardCmd scpi_key);
    void LowertoUpper(char *element, int n);
    bool compareCmd(pCSTree& tree,pCSTree& element);
    char* strupr(char* element,char *p);
    _SCPI_keyboardCmd StringtoCmd(char* element);
    CSNode* Search(pCSTree &tree,char *element);
    bool compareString(pCSTree& tree, char* element);
  private:
    char AUX_data;
    char AUX_data2;

    void initCommandList(void);                                  //获取指令集合
};

#endif /* SCPI_H_ */
