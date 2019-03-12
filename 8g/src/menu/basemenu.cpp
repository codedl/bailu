#include "basemenu.h"

//构造
tBaseMenu::tBaseMenu()
{
  sysKey = NULL;
  mainKey = NULL;
  subKey = NULL;
  isNormal = false;
  fileName = "";
  levelStr = "";
}

//析构
tBaseMenu::~tBaseMenu()
{

}

//初始化主菜单
void tBaseMenu::resetMainKeyProperty(mainKeyDef* key)
{
  key->keyValue = 0x0;
  key->funcCode = 0x0;

  key->tagName = "";
  key->keyValueStr = "";
  key->funcCodeStr = "";
  key->enName = "";
  key->chName = "";
  key->enHelp = "";
  key->chHelp = "";
}

//初始化子菜单
void tBaseMenu::resetSubKeyProperty(subKeyDef* key)
{
  key->keyValue = 0x0;
  key->funcCode = 0x0;
  key->keyTimes = 0;

  key->tagName = "";
  key->keyValueStr = "";
  key->funcCodeStr = "";
  key->enName = "";
  key->chName = "";
  key->enHelp = "";
  key->chHelp = "";

  key->isEnabled = false;
  key->isBlank = false;
  key->isInput = false;
  key->isReverse = false;
  key->isList = false;
  key->isUnit = false;
  key->isExecute = false;
  key->isMore = false;
  key->isReturn = false;
  key->isPassword = false;
  key->isDate = false;
  key->isTime = false;
  key->isDateTime = false;
  key->isIP = false;
  key->isMac = false;
}

//获取子菜单列表
void tBaseMenu::getSubKeyList(subKeyDef* key, QDomNode* node)
{
  if (node == NULL) return;

  int tagLen = 0;

  for (int i = 0; i < node->childNodes().count(); i++)
  {
    if (i == 0)
    {
      key->rightNode = (subKeyDef *)malloc(sizeof (struct subKeyDef));
      key->rightNode->leftNode = key;
      key->rightNode->rightNode = NULL;
      key->rightNode->upNode = NULL;
      key->rightNode->downNode = NULL;
      key = key->rightNode;
    } else
    {
      key->downNode = (subKeyDef *)malloc(sizeof (struct subKeyDef));
      key->downNode->leftNode = NULL;
      key->downNode->rightNode = NULL;
      key->downNode->upNode = key;
      key->downNode->downNode = NULL;
      key = key->downNode;
    }

    resetSubKeyProperty(key);
    for (int j = 0; j < node->childNodes().item(i).attributes().count(); j++)
    {
      setSubKeyProperty(key, node->childNodes().item(i).attributes().item(j));
    }

    tagLen = strlen(node->childNodes().item(i).nodeName().trimmed().toStdString().c_str()) * sizeof(char) + 1;
    key->tagName = (char*)malloc(tagLen);
    memcpy(key->tagName, node->childNodes().item(i).nodeName().trimmed().toStdString().c_str(), tagLen);

    nodeLevel++;

    if (node->childNodes().item(i).childNodes().count() > 0)
    {
      getSubKeyList(key, &node->childNodes().item(i));
    } else
    {
      nodeLevel = 1;
    }
  }
}

//获取系统菜单
bool tBaseMenu::getSystemMenu()
{
  //打开XML文件
  QFile file(fileName);
  if (!file.open(QFile::ReadOnly))
  {
    return false;
  }

  //检验是否为XML文件
  QString errStr = "";
  int errLine = 0;
  int errCol = 0;
  QDomDocument doc;

  if (!doc.setContent(&file, false, &errStr, &errLine, &errCol))
  {
    file.close();
    return false;
  }

  QDomElement root;
  QDomNode mainNode;
  QDomNode subNode;
  int tagLen = 0;

  //检验根节点是否为ROOT
  root = doc.documentElement();
  if (root.tagName().trimmed().toUpper() != "ROOT")
  {
    file.close();
    return false;
  }

  //获取第一个主键
  nodeLevel = 0;
  mainNode = root.firstChildElement();

  //循环获取主键
  while (!mainNode.isNull())
  {
    if (sysKey == NULL)
    {
      sysKey = (mainKeyDef *)malloc(sizeof (struct mainKeyDef));

      mainKey = sysKey;
      mainKey->rightNode = NULL;
      mainKey->downNode = NULL;
    } else
    {
      mainKey->downNode = (mainKeyDef *)malloc(sizeof (struct mainKeyDef));

      mainKey = mainKey->downNode;
      mainKey->rightNode = NULL;
      mainKey->downNode = NULL;
    }

    resetMainKeyProperty(mainKey);
    for (int i = 0; i < mainNode.attributes().count(); i++)
    {
      setMainKeyProperty(mainKey, mainNode.attributes().item(i));
    }

    //tagName
    tagLen = strlen(mainNode.nodeName().trimmed().toStdString().c_str()) * sizeof(char) + 1;
    mainKey->tagName = (char*)malloc(tagLen);
    memcpy(mainKey->tagName, mainNode.nodeName().trimmed().toStdString().c_str(), tagLen);

    //循环获取子键
    subNode = mainNode.firstChildElement();

    while (!subNode.isNull())
    {
      if (mainKey->rightNode == NULL)
      {
        mainKey->rightNode = (subKeyDef *)malloc(sizeof (struct subKeyDef));
        subKey = mainKey->rightNode;
        subKey->upNode = NULL;
        subKey->leftNode = NULL;
        subKey->rightNode = NULL;
        subKey->downNode = NULL;
      } else
      {
        subKey->downNode = (subKeyDef *)malloc(sizeof (struct subKeyDef));
        subKey->downNode->upNode = subKey;
        subKey->downNode->leftNode = NULL;
        subKey->downNode->rightNode = NULL;
        subKey->downNode->downNode = NULL;
        subKey = subKey->downNode;
      }

      resetSubKeyProperty(subKey);

      for (int i = 0; i < subNode.attributes().count(); i++)
      {
        setSubKeyProperty(subKey, subNode.attributes().item(i));
      }

      tagLen = strlen(subNode.nodeName().trimmed().toStdString().c_str()) * sizeof(char) + 1;
      subKey->tagName = (char*)malloc(tagLen);
      memcpy(subKey->tagName, subNode.nodeName().trimmed().toStdString().c_str(), tagLen);

      nodeLevel = 1;
      getSubKeyList(subKey, &subNode);
      subNode = subNode.nextSiblingElement();
    }

    //下一个主键
    mainNode = mainNode.nextSiblingElement();
    nodeLevel = 0;
  }

  file.close();

  return sysKey != NULL;
}

//设置主键属性
void tBaseMenu::setMainKeyProperty(mainKeyDef* key, QDomNode node)
{
  if ((key == NULL) || (node.isNull()))
  {
    return;
  }

  int charLen = 0;
  QString nameQString = node.nodeName().trimmed().toUpper();
  QString valueQString = node.nodeValue().trimmed().toUpper();
  QString valueQString2 = node.nodeValue().trimmed();
  charLen = strlen(valueQString.toStdString().c_str()) * sizeof(char) + 1;
  char* proCharString = (char*)malloc(charLen);
  memcpy(proCharString, valueQString.toStdString().c_str(), charLen);

  if (nameQString == "KEYVALUE")
  {
	key->keyValueStr = (char*)malloc(charLen);
	memcpy(key->keyValueStr, proCharString, charLen);
    key->keyValue = 0x0;
  } else if (nameQString == "FUNCCODE")
  {
	key->funcCodeStr = (char*)malloc(charLen);
	memcpy(key->funcCodeStr, proCharString, charLen);
    key->funcCode = 0x0;
  } else if (nameQString == "ENNAME")
  {
    key->enName = (char*)malloc(charLen);
    memcpy(key->enName, proCharString, charLen);
  } else if (nameQString == "CHNAME")
  {
    key->chName = (char*)malloc(charLen);
    memcpy(key->chName, proCharString, charLen);
  } else if (nameQString == "ENHELP")
  {
    key->enHelp = (char*)malloc(charLen);
    memcpy(key->enHelp, valueQString2.toStdString().c_str(), charLen);
  } else if (nameQString == "CHHELP")
  {
    key->chHelp = (char*)malloc(charLen);
    memcpy(key->chHelp, valueQString2.toStdString().c_str(), charLen);
  } else
  {
  }

  free(proCharString);
}

//设置子键属性
void tBaseMenu::setSubKeyProperty(subKeyDef* key,  QDomNode node)
{
  if ((key == NULL) || (node.isNull()))
  {
    return;
  }

  int charLen = 0;
  QString nameQString = node.nodeName().trimmed().toUpper();
  QString valueQString = node.nodeValue().trimmed();
  charLen = strlen(valueQString.toStdString().c_str()) * sizeof(char) + 1;
  char* proCharString = (char*)malloc(charLen);
  memcpy(proCharString, valueQString.toStdString().c_str(), charLen);

  if (nameQString == "KEYVALUE")
  {
	key->keyValueStr = (char*)malloc(charLen);
	memcpy(key->keyValueStr, proCharString, charLen);
    key->keyValue = 0x0;
  } else if (nameQString == "FUNCCODE")
  {
	key->funcCodeStr = (char*)malloc(charLen);
	memcpy(key->funcCodeStr, proCharString, charLen);
    key->funcCode = 0x0;
  } else if (nameQString == "ENNAME")
  {
    key->enName = (char*)malloc(charLen);
    memcpy(key->enName, proCharString, charLen);
  } else if (nameQString == "CHNAME")
  {
    key->chName = (char*)malloc(charLen);
    memcpy(key->chName, proCharString, charLen);
  } else if (nameQString == "ENHELP")
  {
    key->enHelp = (char*)malloc(charLen);
    memcpy(key->enHelp, proCharString, charLen);
  } else if (nameQString == "CHHELP")
  {
    key->chHelp = (char*)malloc(charLen);
    memcpy(key->chHelp, proCharString, charLen);
  } else if (nameQString == "ISENABLE")
  {
	key->isEnabled = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISBLANK")
  {
	key->isBlank = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISINPUT")
  {
	key->isInput = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISREVERSE")
  {
	key->isReverse = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISLIST")
  {
	key->isList = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISUNIT")
  {
	key->isUnit = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISEXECUTE")
  {
	key->isExecute = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISMORE")
  {
	key->isMore = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISRETURN")
  {
	key->isReturn = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISPASSWORD")
  {
	key->isPassword = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISDATE")
  {
	key->isDate = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISTIME")
  {
	key->isTime = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISDATETIME")
  {
	key->isDateTime = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISIP")
  {
	key->isIP = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  } else if (nameQString == "ISMAC")
  {
	key->isMac = (valueQString.toUpper() == "TRUE") || (valueQString == "1");
  }

  else
  {

  }
}

//加载XML文件
bool tBaseMenu::loadFromXmlFile(QString xmlFileName)
{
  if ((xmlFileName.isNull()) || (xmlFileName.isEmpty()))
  {
    return false;
  }
  fileName = xmlFileName;

  if (!QFile::exists(fileName))
  {
    return false;
  }

  isNormal = getSystemMenu();

  printSysMenu();

  return true;
}

//打印子菜单
void tBaseMenu::printSubKeyList(subKeyDef* key)
{
  if (key == NULL) return;

  QString spaceStr = "";
  QString printStr = "";

  key = key->rightNode;

  while (key != NULL)
  {
    spaceStr = "";
    for (int i = 0; i < nodeLevel * 4; i++)
    {
      spaceStr += " ";
    }
    printStr = spaceStr;
    printStr += key->tagName;
    PRINTMSG(printStr.toStdString().c_str());

    if (key->rightNode != NULL)
    {
      nodeLevel++;
      printSubKeyList(key);
      nodeLevel--;
    }

    key = key->downNode;
  }
}

//打印系统菜单
void tBaseMenu::printSysMenu(void)
{
  #ifndef __DEBUG
    return;
  #endif

  #if __DEBUG
    return;

    PRINTMSG("");
    PRINTMSG("*******************************************");
    PRINTMSG("***        system menu key list        ****");
    PRINTMSG("*******************************************");

    mainKeyDef* mainKeyTmp;
    subKeyDef* subKeyTmp;
    QString spaceStr = "";
    QString printStr = "";

    if (sysKey != NULL)
    {
      mainKeyTmp = sysKey;

      while (mainKeyTmp != NULL)
      {
        nodeLevel = 0;
        spaceStr = "";
        for (int i = 0; i < nodeLevel * 4; i++)
        {
          spaceStr += " " ;
        }
        printStr = spaceStr;
        printStr += mainKeyTmp->tagName;

        PRINTMSG("");
        PRINTMSG(printStr.toStdString().c_str());

        nodeLevel = 0;
        subKeyTmp = mainKeyTmp->rightNode;

        while (subKeyTmp != NULL)
        {
          nodeLevel = 1;
          spaceStr = "";
          for (int i = 0; i < nodeLevel * 4; i++)
          {
            spaceStr += " " ;
          }
          printStr = spaceStr;
          printStr += subKeyTmp->tagName;
          PRINTMSG(printStr.toStdString().c_str());

          nodeLevel = 2;
          printSubKeyList(subKeyTmp);

          subKeyTmp = subKeyTmp->downNode;
        }

        mainKeyTmp = mainKeyTmp->downNode;
      }
    }

    PRINTMSG("*******************************************");
    PRINTMSG("");

  #endif
}
