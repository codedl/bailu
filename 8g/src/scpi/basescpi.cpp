#include "basescpi.h"

//构造
tBaseScpi::tBaseScpi()
{
  initCommandList();
}

//析构
tBaseScpi::~tBaseScpi()
{

}

resultDef tBaseScpi::GetFunctionCode(QString str)
{
  _SCPI_keyboardCmd inputStringCmd;
  int inputCmd_num = 0;
  resultDef result;
  result.cmdCode = 0x0;
  result.value = "";
  result.auxValue = 0x0;
  result.auxValue2 = 0x0;
  result.auxValue3 = 0x0;
  AUX_data = 0x0;
  AUX_data2 = 0x0;
  AUX_data3 = 0x0;
  prvNode = NULL;

  initParseCmd();

  if(str.length() > 64)
  {
	result.cmdCode =0;
	return result;
  }

  bool isSystemkey = false;
  unsigned long int code = 0;

  char t[64] = {0};
  char t_system[64] = {0};
  strcpy(t, str.toStdString().c_str());

  for (int k = 0; k < sizeof Mainkey / sizeof Mainkey[0]; k++)
  {
    strupr(t, t_system);

    if (strcmp(t_system, Mainkey[k].data) == 0)
	{
	  isSystemkey = true;
	  code = Mainkey[k].funcode;
	  break;
	} else
	{
	  isSystemkey = false;
	}
  }

  inputStringCmd = StringtoCmd(t);
  int m = parseStringCmd(inputStringCmd);
  pCSTree input_node = root_tree->leftchild;
  pCSTree pTemp_input = input_node;
  pTemp_input = NULL;

  //prevent error resolve, eg. "xxx?;"
  if (!isSystemkey && m == 0)
  {
    input_node = NULL;
  }

  if (input_node != NULL)
  {
    for(int j = 0; j < m; j++)
    {
	  pTemp_input = Search(input_node, parseCmd[j].data);
	  input_node = pTemp_input;

      if (j == m - 1)
      {
    	input_node = prvNode;
    	break;
      }

	  if(input_node == NULL)
	  {
	    break;
	  }
    }
  }

  if (input_node == NULL)
  {
	result.cmdCode = 0;
  } else
  {
	if (isSystemkey)
	{
 	  result.cmdCode = code;
	} else
	{
	  result.cmdCode = input_node->funcode;
	}
  }

  result.value = QString::fromLocal8Bit(valueCmd.data);
  result.auxValue = AUX_data;
  result.auxValue2 = AUX_data2;
  result.auxValue3 = AUX_data3;

  return result;
}

void tBaseScpi::initParseCmd(void)
{
  memset(parseCmd, 0, sizeof parseCmd);
}

void tBaseScpi::CreatTree(pCSTree &tree, int num)
{
  if (tree != NULL)
  {
	UpdataTree(tree,num);
	return;
  }

  CSNode * node;
  node = new CSNode;
  node ->init();
  tree = node;
}

void tBaseScpi::UpdataTree(pCSTree &tree, int num)
{
  CSNode * node = tree;
  CSNode * insert_data;
  insert_data = new CSNode;
  pCSTree pTemp = NULL;

  int n = strlen(node->data);
  for (int i = 0 ; i < num ; i++)
  {
	strcpy(insert_data->data,parseCmd[i].data);
	strcpy(insert_data->data_abbreviation,abbreviationCmd[i].data);
	insert_data->isellipsis = parseCmd[i].isellipsis;

    insert_data->funcode = parseCmd[i].funcode;

    //判断当前节点应插入的位置
    pTemp = InsertChild(node, insert_data);
    node = pTemp;
  }
}

CSNode* tBaseScpi::InsertChild(pCSTree &tree,pCSTree & element)
{
  pCSTree now_node = tree;

  //判断其当前节点的孩子节点是否存在此元素，处在的话返回此节点不存在生成新的节点返回其地址
  pCSTree temp = new CSNode;

  if (compareCmd(now_node, element))
  {
	return now_node;
  }

  temp = now_node ->leftchild;

  if (temp == NULL)
  {
	pCSTree pNode = new CSNode;
	strcpy(pNode->data,element->data);
	strcpy(pNode->data_abbreviation,element->data_abbreviation);
	pNode->isellipsis = element->isellipsis;
	pNode->funcode = element->funcode;
	pNode->leftchild = NULL;
	pNode->rightbrother = NULL;
	now_node ->leftchild = pNode;
	return pNode;
  }

  // 找到要插入的位置
  while ((!compareCmd(temp,element)) && temp->rightbrother != NULL) //(temp->data != element->data)&&(temp->data_abbreviation !=element->data_abbreviation)
  {
	temp = temp->rightbrother;
  }

  if (compareCmd(temp, element))
  {
	return temp;
  } else
  {
	pCSTree pNewNode = new CSNode;
	strcpy(pNewNode->data,element->data);
	strcpy(pNewNode->data_abbreviation,element->data_abbreviation);
	pNewNode->isellipsis =element->isellipsis;
	pNewNode->funcode = element->funcode;
	pNewNode->leftchild = NULL;
	pNewNode->rightbrother = NULL;
	temp->rightbrother = pNewNode;
	return pNewNode;
  }

  return NULL;
}

char tBaseScpi::DeleteChild()
{
  char data[20];
  return  data[20];
}

int tBaseScpi::parseStringCmd(_SCPI_keyboardCmd scpi_key)
{
  char buf[64] = {0};
  char ch;
  int count;
  int ellipsis_count_c;
  int ellipsis_count_d;
  bool bfinish = false;
  bool isellipsis = false;
  char a[64] = {0};
  int b[10] = {0};
  int c[10] = {0};
  char d[64] = {0};

  char *q = NULL;
  int a_size = 0;

  strcpy(a, scpi_key.data);
  q = a;
  a_size = strlen(q);

  int num1 = sizeof a /sizeof a[0];

  //查询“：”的个数
  count =0;
  ellipsis_count_c = 0;
  ellipsis_count_d = 0;
  memset(b, 0, 10);
  memset(c, 0, 10);

  for(int j = 0; j < num1; j++)
  {
	if(a[j] == ':')
	{
      b[count] = j;
      count ++;
	} else if (a[j] == 0x0)
	{
      break;
    }
  }

  //拆分指令
  int m;
  int n ;

  for(int k = 0; k < count; k++)
  {
    memset(buf, 0, 20);

    if(k == count -1)
	{
	  m = 0;
      n =0;

      for(m= b[k]; m < a_size; m++)
	  {
	    buf[m-b[k]] = a[m];
	    n++;
	  }

	  while(buf[0] == ':')
      {
		for(m = 1; m <= n; m++)
		{
		  buf[m - 1] = buf[m];
		}
	  }
	} else if (k < count -1)
	{
	  m =0;
	  n=0;

	  for(m = b[k]; m < b[k + 1]; m++)
	  {
	    buf[m-b[k]] = a[m];
		n++;
	  }

	  while(buf[0] == ':')
	  {
		for(m = 1; m <= n; m++)
		{
		  buf[m-1] = buf[m];
		}
	  }
	}

	if(k == count - 1)
	{
	  parseCmd[k].funcode = scpi_key.funcode;  //把字符串的功能付给最后的一节点
	} else
	{
	  parseCmd[k].funcode = 0;
	}

	memset(d, 0, 64);
	strcpy(d, buf);

	if((strcmp(d, "") == 0))
	{
	  return 0;
	}

	int t = strlen(buf);

	if(d[0] == 91)
	{
	  isellipsis = true;

	  for(int i = 1; i < t; i++)
	  {
		while (d[i] == 93)
		{
		  if(d[i+1] == 0x0)
		  {
			d[i] = 0x0;
		  } else
		  {
		    d[i] = d[i+1];
		    d[i+1] = 0x0;
		  }
		}

		d[i-1] = d[i];
	  }
	} else
	{
	  isellipsis = false;
	}

	if(isellipsis)
	{
	  parseCmd[k].isellipsis = true;
	} else
	{
	  parseCmd[k].isellipsis = false;
	}

	strcpy(parseCmd[k].data, d);
	LowertoUpper(parseCmd[k].data, k);
  }

  return count;
}

void tBaseScpi::LowertoUpper(char *element, int n)
{
  int m = strlen(element);
  int t;
  char a[20] = {0};
  char c[10] = {0};
  char b[10] = {0};
  char *p;

  t = 0;
  strcpy(a, element);

  for(int i = 0; i < m; i++)
  {
	if (a[i] >= 65 && a[i] <= 90)
	{
	  c[i] = a[i];
	  t = i;
	} else
	{
	  break;
	}
  }

  if (a[m-1] == 63)
  {
    c[t+1] = 63;
  }

  strcpy(b, c);

  for (int j = 0; j < MAX_GROUP; j++)
  {
	if(b[j] == 0x0)
	{
	  break;
	}
  }

  strcpy(abbreviationCmd[n].data, b);
}

bool tBaseScpi::compareCmd(pCSTree& tree,pCSTree& element)
{
  return (tree != NULL) && (strcmp(tree->data, element->data) == 0 || strcmp(tree->data_abbreviation, element->data_abbreviation) == 0);
}

char* tBaseScpi::strupr(char * element,char *p)
{
  char a[64] = {0};
  char b[64] = {0};
  strcpy(a, element);
  int m = strlen(element);

  for (int i = 0; i < m; i++)
  {
	if(a[i] >= 97 && a[i] <= 122)
	{
	  b[i] = a[i] - 32;
	} else
	{
	  b[i] = a[i];
	}
  }

  strcpy(p, b);
  return p;
}

_SCPI_keyboardCmd tBaseScpi::StringtoCmd(char* element)
{
  _SCPI_keyboardCmd node;
  memset(node.data, 0, 64);
  node.funcode = 0;

  char a[64] = {0};
  int b[10] = {0};
  int space[10] = {0};
  char c[64] = {0};
  char d[64] = {0};
  char value[64] = {0};

  int count_b = 0;
  int num = strlen(element);

  strcpy(a, element);

  for (int j = 0; j < num; j++)
  {
    if (a[j] == ':')
	{
      b[count_b] = j;
	  count_b++;
	} else if (a[j] == 0x0)
	{
	  break;
	}
   }

  for(int i = b[0]; i < num; i++)
  {
	a[i - b[0]] = a[i];
  }

  for (int i = num - b[0]; i < num; i++)
  {
	a[i] = 0x0;
  }

  int space_count = 0;
  int n = strlen(a);

  for (int k= 0; k < n; k++)
  {
	if (a[k] == 32)
	{
	  space[space_count] = k;
	  space_count++;
	} else if (a[k] == 0x0)
	{
	  break;
	}
  }

  for (int m = 0; m < n; m++)
  {
	if (m < space[0] || space[0] == 0)
	{
	  c[m] = a[m];
	} else
	{
	  d[m-space[0]] = a[m];

	  if (a[m] == 0x0)
	  {
		break;
	  }
	}
  }

  strcpy(node.data, c);
  int d_count = strlen(d);

  for(int i =0 ; i < d_count; i++)
  {
	int t = 0;

	while (d[i] == 32)
	{
	  d[i] = d[i+t+1];

	  if (d[i] != 32)
	  {
		d[i+t+1] = 32;
		break;
	  }

	  t++;
	}
  }

  int j = 0;

  memset(value, 0, 64);
  memset(valueCmd.data, 0, 64);

  while (d[j] != 0x0)
  {
	value[j] = d[j];
	j++;
  }

  strcpy(valueCmd.data, value);

  return node;
}

CSNode* tBaseScpi::Search(pCSTree &tree, char *element)
{
  pCSTree now_node = tree;
  pCSTree temp = tree;
  pCSTree temp1 = tree;
  pCSTree temp2 = tree;
  pCSTree temp3 = tree;

  if (compareString(now_node,element))
  {
    prvNode = now_node;

	if(now_node ->leftchild==NULL)
	{
		return now_node;
	}
	return now_node ->leftchild;
  }

  temp = now_node;

  while (!compareString(temp, element) && temp->rightbrother != NULL)
  {
	temp = temp->rightbrother;
  }

  if (compareString(temp, element))
  {
    prvNode = temp;

	if (temp ->leftchild == NULL)
	{
	  return temp;
	}
	return temp ->leftchild;
  }

  temp3 = now_node;
  if (temp3 != NULL && temp3->leftchild != NULL && temp3->isellipsis)
  {
	temp1 = temp3->leftchild;
    if (compareString(temp1, element))
    {
   	  prvNode = temp1;

	  if(temp1->leftchild==NULL)
	  {
		return temp1;
	  } else
	  {
	    return temp1 ->leftchild;
	  }
    }

    while (!compareString(temp1, element) && temp1->rightbrother != NULL)
    {
	  temp1 = temp1->rightbrother;

	  if (temp1 != NULL && temp1->leftchild != NULL && temp1->isellipsis)
	  {
        temp1=temp1->leftchild;
	  }
    }
    if (compareString(temp1, element))
    {
      prvNode = temp1;

	  if(temp1->leftchild == NULL)
	  {
	    return temp1;
	  } else
	  {
	    return temp1 ->leftchild;
	  }
    }
  } else
  {
	while (!compareString(temp3, element) && temp3->rightbrother != NULL)
	{
	  temp3 = temp3->rightbrother;

	  if (temp3 != NULL && temp3->leftchild != NULL && temp3->isellipsis)
	  {
		temp3=temp3->leftchild;
	  }
    }
	if (compareString(temp3, element))
	{
	  prvNode = temp3;

	  if(temp3->leftchild == NULL)
	  {
	    return temp3;
	  }	else
	  {
		return temp3 ->leftchild;
	  }
	}
  }
  return NULL;
}

bool tBaseScpi::compareString(pCSTree& tree, char* element)
{
  char c[64] = {0};
  char d[64] = {0};
  char e[64] = {0};

  strupr(tree->data, c);
  strupr(element, d);
  strupr(tree->data_abbreviation, e);

  char f[64] = {0};
  strcpy(f, d);

  char chrEof = f[strlen(f) - 1];
  f[strlen(f) - 1] = 0x0;

  if (chrEof != '?')
  {
    if ((strcmp(f, "MARKER") == 0) ||(strcmp(f, "MARK") == 0))
    {
   	  AUX_data = d[strlen(d) - 1];
   	  return (strcmp(c, f) == 0 || strcmp(e, f) == 0);
    } else if (strcmp(f, "TRAC") == 0 || strcmp(f, "TRACE") == 0)
    {
      AUX_data2 = d[strlen(d) - 1];
      return (strcmp(c, f)==0||strcmp(e, f)==0);
    }
  }

  return (strcmp(c, d) == 0 || strcmp(e, d) == 0);
}

//获取指令集合
void tBaseScpi::initCommandList(void)
{
  // 构造树
  root_tree = NULL;
  pCSTree root = (CSNode *)malloc(sizeof(CSNode));
  root = NULL;

  for(unsigned long i = 0; i < sizeof test / sizeof test[0]; i++)
  {
    initParseCmd();
    int t = parseStringCmd(test[i]);
	CreatTree(root, t);
  }

  root_tree = root;
}
