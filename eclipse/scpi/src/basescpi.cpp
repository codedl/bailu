#include "basescpi.h"
static struct _SCPI_keyboard parseCmd[10];
static  _SCPI_keyboard abbreviationCmd[10];
//构造
tBaseScpi::tBaseScpi()
{
	initCommandList();
}

//析构
tBaseScpi::~tBaseScpi()
{

}
#if 0
//获取功能码
resultDef tBaseScpi::GetFunctionCode(string str)
{
	_SCPI_keyboardCmd inputStringCmd;
	int inputCmd_num = 0;
	resultDef result;
	result.cmdCode = 0x0;
	result.value = "";
	result.auxValue = 0x0;
	result.auxValue2 = 0x0;
	AUX_data = 0x0;
	AUX_data2 = 0x0;
	prvNode = NULL;

	initParseCmd();

	if (str.length() > 64)
	{
		result.cmdCode = 0;
		return result;
	}

	bool isSystemkey = false;
	unsigned long int code = 0;

	char t[64] = { 0 };
	char t_system[64] = { 0 };
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

	if (input_node != NULL)
	{
		for (int j = 0; j < m; j++)
		{
			pTemp_input = Search(input_node, parseCmd[j].data);
			input_node = pTemp_input;

			if (j == m - 1)
			{
				input_node = prvNode;
				break;
			}

			if (input_node == NULL)
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

	return result;
}
#endif
//初始化命令参数
void tBaseScpi::initParseCmd(void)
{
	memset(parseCmd, 0, sizeof parseCmd);
}
//@num表示一个SCPI指令中每个以:开头的指令的个数
//@tree表示
void tBaseScpi::CreatTree(pCSTree &tree, int num)
{
	if (tree != NULL)
	{
		UpdataTree(tree, num);
		return;
	}

	CSNode * node;
	node = new CSNode;
	node ->init();
	tree = node;
}

//更新命令树
void tBaseScpi::UpdataTree(pCSTree &tree, int num)
{
	CSNode * node = tree;
	CSNode * insert_data = new CSNode;
	pCSTree pTemp = NULL;
	//int n = strlen(node->data);
	for (int i = 0; i < num; i++)
	{
		printf("\nparseCmd[%d].data:%s\n",i,parseCmd[i].data);
		insert_data->init();
		strcpy(insert_data->data, parseCmd[i].data);
		strcpy(insert_data->data_abbreviation, abbreviationCmd[i].data);
		insert_data->isellipsis = parseCmd[i].isellipsis;
		insert_data->funcode = parseCmd[i].funcode;

		//判断当前节点应插入的位置
		printf("node->data:%s\n",node->data);
		pTemp = InsertChild(node, insert_data);
		node = pTemp;
	}
	free(insert_data);
}

//插入命令子树
CSNode* tBaseScpi::InsertChild(pCSTree &tree, pCSTree & element)
{
	pCSTree now_node = tree;

//判断其当前节点的孩子节点是否存在此元素，存在的话返回此节点	
	if (compareCmd(now_node, element))
	{
		return now_node;
	}
//不存在生成新的节点返回其地址
	//pCSTree temp = new CSNode;
	//temp->init();
	pCSTree temp = now_node ->leftchild;
	printf("now_node->leftchild:%s\n",temp->data);
	if (temp == NULL)
	{
		pCSTree pNode = new CSNode;
		pNode->init();
		strcpy(pNode->data, element->data);
		strcpy(pNode->data_abbreviation, element->data_abbreviation);
		pNode->isellipsis = element->isellipsis;
		pNode->funcode = element->funcode;
		//pNode->leftchild = NULL;
		//pNode->rightbrother = NULL;
		now_node ->leftchild = pNode;
		printf("now_node->leftchild->data:%s;\n",pNode->data);
		return pNode;
	}

	// 找到要插入的位置
	while ((!compareCmd(temp, element)) && temp->rightbrother != NULL) //(temp->data != element->data)&&(temp->data_abbreviation !=element->data_abbreviation)
	{
		temp = temp->rightbrother;
	}
	if (compareCmd(temp, element))
	{
		return temp;
	} else
	{
		pCSTree pNewNode = new CSNode;
		pNewNode->init();
		strcpy(pNewNode->data, element->data);
		strcpy(pNewNode->data_abbreviation, element->data_abbreviation);
		pNewNode->isellipsis = element->isellipsis;
		pNewNode->funcode = element->funcode;
		//pNewNode->leftchild = NULL;
		//pNewNode->rightbrother = NULL;
		temp->rightbrother = pNewNode;
		printf("rightbrother->data:%s;\n",pNewNode->data);
		return pNewNode;
	}

	return NULL;
}

//删除命令子树
char tBaseScpi::DeleteChild()
{
	char data[20];
	return data[20];
}

//@scpi_key指令
//@return 指令中':'的个数,即指令的个数
int tBaseScpi::parseStringCmd(_SCPI_keyboardCmd scpi_key)
{
	char buf[64] = { 0 };//获取':'后面对应的指令字符串
	char ch;
	int count;
	int ellipsis_count_c;
	int ellipsis_count_d;
	bool bfinish = false;
	bool isellipsis = false;
	char a[64] = { 0 };
	int b[10] = { 0 };//定义一个数组记录每个':'的位置
	int c[10] = { 0 };
	char d[64] = { 0 };

	char *q = NULL;
	int a_size = 0;
//计算整个指令的长度
	strcpy(a, scpi_key.data);
	q = a;
	a_size = strlen(q);

	int num1 = sizeof a / sizeof a[0];//64
	//记录指令字符串中":"的个数,每条SCPI以":"开头,count即表示一条指令中的指令个数
	count = 0;
	ellipsis_count_c = 0;
	ellipsis_count_d = 0;
	memset(b, 0, 10);
	memset(c, 0, 10);

	for (int j = 0; j < num1; j++)
	{
		if (a[j] == ':')
		{
			b[count] = j;//用数组记录':'出现的位置,b[0]表示第一个':'的位置;
			count++;//记录':'的个数
		} else if (a[j] == 0x0)//字符串结尾
		{
			break;
		}
	}

	//拆分指令
	int m;//读取指令中的每个字符
	int n;//以':'分隔后的每条指令的长度
//k表示一条指令中的每个指令
	for (int k = 0; k < count; k++)
	{
		memset(buf, 0, 64);

		if (k == count - 1)//最后一个':'
		{
			m = 0;
			n = 0;
			//用buf记录字符串中最后一个":"出现的位置到字符串结尾的字符串
			for (m = b[k]; m < a_size; m++)
			{
				buf[m - b[k]] = a[m];//获取':'后面对应的指令字符串
				n++;//记录每条指令的长度
			}

			while (buf[0] == ':')//去掉指令开头的':'
			{
				for (m = 1; m <= n; m++)
				{
					buf[m - 1] = buf[m];
				}
			}
		} else if (k < count - 1)//不是最后一个':'
		{
			m = 0;
			n = 0;
//从指令中上个':'开始到下个':'的位置
			for (m = b[k]; m < b[k + 1]; m++)
			{
				buf[m - b[k]] = a[m];//获取':'后面对应的指令字符串
				n++;//记录每条指令的长度
			}

			while (buf[0] == ':')//去掉指令开头的':'
			{
				for (m = 1; m <= n; m++)
				{
					buf[m - 1] = buf[m];
				}
			}
		}

	//多个':'指令组合时只对应一个功能码
		if (k == count - 1)
		{
			parseCmd[k].funcode = scpi_key.funcode; //把字符串的功能付给最后的一节点
		} else
		{
			parseCmd[k].funcode = 0;
		}

		memset(d, 0, 64);
		strcpy(d, buf);
//检查指令是否为空
		if ((strcmp(d, "") == 0))
		{
			return 0;
		}

		int t = strlen(buf);

//		
		if (d[0] == 91)//以[开头的字符串
		{
			isellipsis = true;
			//例从"[RF]a"得到"RF"
			for (int i = 1; i < t; i++)
			{
				while (d[i] == 93)//]
				{
					if (d[i + 1] == 0x0)//如果]是最后一个字符
					{
						d[i] = 0x0;//去掉]
					} else//否则用']'后面的一个字符替代']'
					{
					
						d[i] = d[i + 1];
						d[i + 1] = 0x0;
					}
				}

				d[i - 1] = d[i];//去掉[
			}
		} else
		{
			isellipsis = false;
		}

		if (isellipsis)
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
//截取指令中的大写部分
void tBaseScpi::LowertoUpper(char *element, int n)
{
	int m = strlen(element);
	int t;
	char a[20] = { 0, };
	char c[10] = { 0, };
	char b[10] = { 0, };

	char *p;

	t = 0;
	strcpy(a, element);
	
	for (int i = 0; i < m; i++)
	{
		//在A-Z之间
		if (a[i] >= 65 && a[i] <= 90)
		{
			c[i] = a[i];
			t = i;
		} else
		{
			break;
		}
	}
	//最后一个字符是否为'?'
	if (a[m - 1] == 63)
	{
		c[t + 1] = 63;
	}

	strcpy(b, c);

	for (int j = 0; j < 10; j++)
	{
		if (b[j] == 0x0)
		{
			break;
		}
	}

	strcpy(abbreviationCmd[n].data, b);
}

//比较两个节点中的SCPI指令是否相等
bool tBaseScpi::compareCmd(pCSTree& tree, pCSTree& element)
{
		return  (tree != NULL) && (strcmp(tree->data, element->data) == 0  \
			|| strcmp(tree->data_abbreviation, element->data_abbreviation) == 0);
}

char* tBaseScpi::strupr(char * element, char *p)
{
	char a[64] = { 0 };
	char b[64] = { 0 };
	strcpy(a, element);
	int m = strlen(element);

	for (int i = 0; i < m; i++)
	{
		if (a[i] >= 97 && a[i] <= 122)
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
#if 0
_SCPI_keyboardCmd tBaseScpi::StringtoCmd(char* element)
{
	_SCPI_keyboardCmd node;
	memset(node.data, 0, 64);
	node.funcode = 0;

	char a[64] = { 0 };
	int b[10] = { 0 };
	int space[10] = { 0 };
	char c[64] = { 0 };
	char d[64] = { 0 };
	char value[64] = { 0 };

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

	for (int i = b[0]; i < num; i++)
	{
		a[i - b[0]] = a[i];
	}

	for (int i = num - b[0]; i < num; i++)
	{
		a[i] = 0x0;
	}

	int space_count = 0;
	int n = strlen(a);

	for (int k = 0; k < n; k++)
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
			d[m - space[0]] = a[m];

			if (a[m] == 0x0)
			{
				break;
			}
		}
	}

	strcpy(node.data, c);
	int d_count = strlen(d);

	for (int i = 0; i < d_count; i++)
	{
		int t = 0;

		while (d[i] == 32)
		{
			d[i] = d[i + t + 1];

			if (d[i] != 32)
			{
				d[i + t + 1] = 32;
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
#endif
#if 0
CSNode* tBaseScpi::Search(pCSTree &tree, char *element)
{
	pCSTree now_node = tree;
	pCSTree temp = tree;
	pCSTree temp1 = tree;
	pCSTree temp2 = tree;
	pCSTree temp3 = tree;

	if (compareString(now_node, element))
	{
		prvNode = now_node;

		if (now_node ->leftchild == NULL)
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

			if (temp1->leftchild == NULL)
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
				temp1 = temp1->leftchild;
			}
		}
		if (compareString(temp1, element))
		{
			prvNode = temp1;

			if (temp1->leftchild == NULL)
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
				temp3 = temp3->leftchild;
			}
		}
		if (compareString(temp3, element))
		{
			prvNode = temp3;

			if (temp3->leftchild == NULL)
			{
				return temp3;
			} else
			{
				return temp3 ->leftchild;
			}
		}
	}
	return NULL;
}
#endif

//获取指令集合
void tBaseScpi::initCommandList(void)
{
	// 构造树
	root_tree = NULL;
	pCSTree root = (CSNode *) malloc(sizeof(CSNode));
	root->init();
	int t = 0;
	for (unsigned long i = 0; i < sizeof test / sizeof test[0]; i++)
	{
		initParseCmd();
		t = parseStringCmd(test[i]);
		CreatTree(root, t);
	}

	root_tree = root;
}
