<<c#从入门到精通>>
<数据类型>
[数据类型]
值类型:保存在栈上;
引用类型:保存在堆上;
[可空类型]
可空类型属于值类型,保存在栈上;
[类和结构]
类是引用类型,结构是值类型;
类支持继承;
[显示转换]
checked检查转换是否安全;
[运算符]
is:检查变量类型
as:在相互兼容的类型之间执行转换操作;
typeof:返回特定类型的System.Type对象;
[参数]
值类型会复制变量的值;引用类型使用变量本身;
ref:引用类型参数;
out:输出参数;
params:参数类型相同,数量可变;
命名参数:参数名:参数值传递即可;
foreach(xx in xx):遍历;
<类>
匿名类:var 类型 = new {};只读属性;
[属性成员]
格式:public 类型 成员
{set{} get{}};
[索引指示器]
格式:
访问修饰符 数据类型 this[索引类型 index]
{get{} set{}}
[字符串]
String.copy():重新生成字符串;
=:引用原来的字符串;
StringBuilder会在原来字符串上操作,不会分配新的空间;
<继承和派生>
[虚方法]
virtual声明虚方法;override重写基类的虚方法;
[抽象类]
只能被继承,不能被创建;
[密封类]
sealed定义密封类和密封方法,不能被派生类继承和重写;
<委托与事件>
[委托]
声明委托:权限修饰符 delegate 返回值 函数名(参数)
使用委托:定义委托对象,传递参数;
[比较器]
继承IComparer,重写方法Compare();

<<c#高级编程>>
<核心C#>
变量声明时必须被初始化;
[引用类型]
值类型存储在堆栈上,而引用类型存储在托管堆上;
new可以创建一个新的对象;
[string]
赋值时修改其中一个字符串,会创建一个全新的string对象;
@字符串:字符串中的字符不会解释为转义字符;
[Main()方法]
/main:告诉编译器程序的入口点;
[占位符]
{0:#.00}#表示有字符就输出,没字符不输出;
.00有字符就输出,没字符输出0;
[预处理指令]
#error+字符串:提示错误信息;
#warning+字符串:提示警告信息;
#line 12 core.cs改变文件名为core.cs,行号为12;
#pragma:用于还原和抑制编译的相关警告;
<对象和类型>
[结构和类的区别]
1.类是存储在堆上的引用类型,结构是存储在栈上的值类型;
2.类支持继承;
[给方法传递参数]
引用:方法得到的是这个变量;
值:方法得到的是变量副本;
ref:强制将值传送改为引用传送;
向方法传递参数前要初始化;
out:声明传递给方法的参数不用初始化;
命名参数:命名参数可以按照任意顺序传递;
可选参数:为方法的参数定义默认值,但必须是方法定义的最后的参数;
[属性]
声明: public int Age;
set:假定带有一个与属性相同的参数为value;
get:返回属性声明的类型;
可以为set或get设置访问权限,必须有一个具备属性的访问级别;
[静态构造函数]
加载类时由.NET运行库调用;
[调用其他构造函数]
1.this调用参数最匹配的构造函数;
2.base调用基类的构造函数;
[只读字段]
初始化赋值后变成常量;
[结构]
1.不允许定义无参数的构造函数;
2.System.ValueType可以作为结构的基类;
[部分类]
partial:可以嵌套部分类;
[静态类]
只包含静态的方法和属性;
[扩展方法]
public static void method(this class obj, int param)
this关键字表示需要扩展的类型;
<继承>
c#不支持多重继承,继承时没有public等限定符;
object是System.Object的假名;
[实现继承]
override:显示声明重写一个函数;
new:隐藏同名方法;
[base]
base.方法名:可以在派生类中调用基类的方法;
[abstract]关键字声明抽象类或抽象接口;
含有抽象方法的类就是抽象类;
抽象类不能实例化,抽象函数不能直接实现;
[sealed]
1.对于类表示不能继承该类;
2.对于方法表示不能重写该方法,基类中需要使用override修饰;
[派生的接口]
继承接口必须实现接口中定义的所有方法;
<泛型>
定义:public class someclass<T>{}
[List/ArrayList]
ArrayList存储对象,参数类型为引用类型;可以往集合添加整数,字符串,和其他类型的对象;
List的参数为泛型,不需要进行装箱和拆箱操作;只能保存一种类型的对象;
[代码的扩展]
引用类型共享同一个本地类的所有相同的实现代码;
每个值类型都要实例化一个新类;
[default]
将null赋给引用类型,将0赋予值类型;
[where]
泛型约束:where T:IFoo,泛型必须实现IFoo接口;
[继承]
派生类与基类的泛型类型必须一致,或者指定基类的类型;
[静态成员]
只能在类的一个实例中共享;
[协变和抗变]
out关键字标注泛型接口为协变的,泛型只能作为返回值,可以向上转型;
in关键字标注泛型接口为逆变的,泛型只能作为输入参数,可以向下转型;
<可空类型>
Nullable<int>x1;int? x2;
x1和x2都是可空的int类型;
int y =4; int? x = y;非可空类型转换为可空类型;
<数组>
数组的大小固定,集合包含元素的个数是动态的;
[ArraySegment]
new ArraySegment<int>(array,0,3)获取array的从0开始的三个元素;
[yield]
yield return返回集合的一个元素;
yield break停止迭代;
迭代块返回IEnumerator接口;
[Tuple]
Tuple可以用来存储不用类型的元素;
Tuple.Create<int, int>(1, 2)创建Tuple;
Tuple.Item1,Tuple.Item2访问Tuple元素;
<运算符和类型强制转换>
[checked]
checked执行溢出检查;
unchecked取消检查;
[is]
intI is object:检查对象为给类型或该类型的派生类;
[as]
o1 as string:o1强制转换成string类型;只能用于引用类型;
[typeof]
返回一个表示特定类型的System.Type对象;
[装箱和拆箱]
装箱:值类型转化成引用类型;
只能对以前装箱的变量进行拆箱;
[类之间的类型强制转换]
1.具有继承关系的类不能进行强制转换;
2.强制转换只能在类的内部进行定义;
3.两个类之间的强制转换只能在一个类中定义,否则编译器无法识别需要调用哪个;
[基类到派生类的转换]
基类到派生类的转换是杜绝的,可以定义一个以基类实例为参数的派生类构造函数;
<委托,正则表达式和事件>
[定义]
private delegate string GetAString();
GetAString method = x.ToString;
x.ToString返回函数地址;x.ToString()返回字符串;
<泛型委托>
Action<in T>:定义带一个参数的方法;
Func<in T1, out T2>:定义带一个参数和一个返回值的方法;
[多播委托]
Action<double> action = method1;action += method2;
调用action会依次调用method1,method2;
[lambda表示式]
格式:Func<double, double> f= (d) => {};
=>:左边为参数,右边为参数的操作方法;
[事件]
第一步:public event EventHandler<T> EventExam相当于
public delegate void EventHandler<TEventArgs>(object sender, TEventArgs e):where e:EventArgs;
第二步:定义事件侦听器,即定义一个可以赋值给EventExam的方法;
第三步:连接事件发布程序和订阅器,即将方法赋值给委托;
[弱事件]
垃圾回收器不会清空侦听器占用的内存;可以使用弱事件来管理事件和侦听器之间的连接;
创建弱事件的步骤:
1.定义WeakEventManager的派生类;
2.重写AddListener()和RemoveListener(),调用基类中的ProtectedAddListener()和
ProtectedRemoveListener();侦听器使用这些方法连接和断开事件;
3.重写StartListening()和StopListening(),添加第一个侦听器和删除最后一个监听器时自动调用;
4.重写DeliverEvent()把事件传递给侦听器;
5.在侦听器中实现IWeakEventListener.ReceiveWeakEvent方法;
[泛型弱事件管理器]
var t1 = new class1();var t2 = new class2();
WeakEventManager<class1, class2>.AddHandler(t1, "name", t2.method)