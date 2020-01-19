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
<字符串和正则表示式>
\b:表示字的边界;
@:\不会被解释为转义序列;
\S:表示任何不是空白字符的字符;
*:限定符,前面的字符可以重复任意次
\S*:任意个不是空白字符的字符;
[]:请求匹配包含这些字符;
ma[n|p]:表示man或map;
[a-z]:表示小写字母;
[0-9]+:表示数字,+表示至少要有一个可以包含多个;
<集合>
[List<T>]
FindIndex:
1.定义一个类,在类中实现方法bool func(T t);
2.将方法作为参数传递给FindIndex;
Find:
arrays.Find(r => r.word == "word"),在arrays中查找word单词的对象;
Sort:
1.Sort()不带参数需要在类的内部实现IComparable接口;
2.Sort(ICompare<T>):可以向Sort传递实现了ICompare接口的对象;
ConvertAll:
public sealed delegate Toutput ConvertAll<Tinput,Toutput>(Tinput from)
定义接受Tinput为参数,Toutput为返回值的委托;
[队列和栈]
Queue为FIFO的队列,Stack为LIFO的栈;
Tip:WriteLine("{0:X}",value)输出十六进制;
[Lookup]一个键可以对应多个值;
1.创建List列表;
2.ToLookup创建Lookup类对象,lambda或给委托赋值;
3.遍历;
[集]
包含不重复元素的集合;
IsSubsetof:验证子集;
IsSupersetOf:验证超集;
UnionWith:合并集合;
ExceptWith:删除集合;
[可观察的集合]
1.创建一个几个保存元素;
2.定义CollectionChanged事件
[为数组]
1.创建BitVector32对象;
2.CreateMask获取掩码,以此按位访问;
3.CreateSection获取其中某些位;
<LINQ(语言集成查询)>
[查询]
where:迭代在foreach时进行,可以使用ToList创建新集合
OfType:从集合中返回某种数据;
<动态语言扩展>
[dynamic]定义动态类型
<异步编程>
await:解除线程的阻塞,完成其他任务;
1.创建任务:定义一个方法,方法名Async后缀,Task.Run返回一个任务;
2.调用异步方法:await关键字来调用返回任务的异步方法;
await只能用在async修饰的方法中;
async只能用于返回Task或void的方法;
await只能用于返回Task的方法;
3.任务延续:ContinueWith定义任务完成后调用的代码;
[使用组合器]
获取异步任务的返回Task,使用await Task.WhenAll(WhenAny)
WhenAll所有任务完成了才返回Task;
WhenAny其中一个任务完成
[转换异步模式]
使用异步模式的方法转换为基于任务的异步模式的方法;
Factory.FromAsync进行转换;
[错误处理]
1.在try/catch块中使用await关键字;
[多个异步方法]
1.在try/catch块外定义Task,在catch中使用IsFalulted属性检查,
使用Exception.InnerException访问异常;
2.定义一个Task获取Task.WhenAll的返回结果,遍历Task的Exception.InnerException属性;
[取消]
1.创建CancellationTokenSource cts对象
2.cts.Cancel发送取消任务的申请
3.cts.Token当成接口参数检查是否取消任务或
cts.Token.ThrowIfCancellationRequested主动检查任务是否被取消;
任务被取消就会抛出异常
<内存管理和指针>
fixed:类似于const;
stackalloc:在栈上分配一定量的内存,不会初始化内存;数据类型必须为值类型;
定义一个数组对象,如果访问越界会抛出异常;定义一个数组指针就不会抛出异常;
<反射>
[System.Type]
1.使用typeof运算符;
2.使用GetType方法;
3.Type.GetType静态方法;
使用bool属性判断类型;
GetMethod返回System.Reflection.MethodInfo对象的引用;
[自定义特性]即添加可以通过反射获取的元数据;
1.[AttributeUsage(AttributeTargets.Class | AttributeTargets.Method,
  AllowMultiple = true, Inherited = false)]public class LastModifiedAttribute : Attribute{};
  用AttributeUsage关键字声明自定义特性;
  AttributeTargets枚举列出特性应用的元素为类和方法;
  AllowMultiple表示特性可以被复用;
  Inherited表示特性可以被继承,在派生类中也具有特性;
2.定义Attribute的派生类,通过在派生类中的字段说明特性的用途;
3.[assembly: SupportsWhatsNew]用特性标记程序集;
[派生类对象(通过实现Attribute的派生类对象应用特性,可以手动为派生类对象的字段赋值)]class ClassName{}
将自定义特性应用到ClassName类;
如果自定义特性支持方法,在方法前[派生类对象(通过实现Attribute的派生类对象应用特性,可以手动为派生类对象的字段赋值)];
4.获取自定义特性:
a.Assembly.Load加载程序集;
b.Attribute.GetCustomAttribute(Assembly e, Type a)从程序集中获取自定义特性;
c.Assembly.GetTypes从程序集中获取定义的类型;
d.Type.IsClass类似的接口判断特性应用的元素;
e.Attribute.GetCustomAttributes(Type)从元素中获取自定义特性集合;
f.Attribute as LastModifiedAttribute得到自定义特性;
g.访问自定义特性的字段;
==>由c#的基本语法到实现功能的框架
==>c#的框架在章节和内容上比较紧凑,学习新的框架要前后结合
<错误和异常>
.Net运行库有个默认的try/catch块;
定义Exception的派生类可以自定义异常;
[调用者信息]
public void Log(
[CallerLineNumber] int line = -1,
[CallerFilePath] string path = null,
[CallerMemberName] string name = null){}
可以使用这些特性获取文件的行号,文件路径,调用者
<部署>
xcopy/Web部署/Windows Store
<程序集>
扩展名为exe或dll的.net可执行程序为程序集;
卸载程序集只能通过终止应用程序域来进行;
[加载程序集]
1.AppDomain.CurrentDomain.FriendlyName查看应用程序域名称;
2.AppDomain.CreateDomain创建应用程序域;
3.AppDomain.ExecuteAssembly加载程序集;
AppDomain.CreateInstance加载类生成的程序集;
运行自定义代码:
4.AppDomain.CreateInstanceAndUnwrap创建实例;
5. new CompilerParameters().GenerateInMemory定义编译选项;
new CSharpCodeProvider()CompileAssemblyFromSource编译代码;
CompilerResults results获取编译结果;
results.Errors.HasErrors判断是否出错;
[强名]程序集唯一的名称;
<任务/线程和同步>
[数据和任务并行性]
Parallel.For(Foreach)使用多个线程完成循环,实现数据的并行;
Parallel.Invoke实现任务的并行性;
[使用线程池的任务]
new TaskFactory().StartNew;
Task.Factory.StartNew;
new Task().Run;
Task.Run(lambda表示式);
使用的是线程池中的线程;
[同步任务]
new Task().RunSynchronously在主线程中创建任务;
使用主线程;
new Task(t, "s", TaskCreationOptions.LongRunning).Start
可以创建一个新线程;
[任务的结果]
Task的Result属性表示任务的结果;
[连续的任务]
ContinueWith(Task)创建一个在参数Task完成后才执行的任务;
[取消架构]
1.Parallel.For的取消:
a.创建发出取消请求的实例,new CancellationTokenSource();
b.注册取消时的操作,Token.Register;
c.发出取消请求,CancelAfter/Cancel;
d.定义接收请求的参数,Parallel.For重载方法的第三个参数传递new ParallelOptions()
{CancellationToken = cts.Token}
2.任务的取消:
a.创建发出取消请求的实例,new CancellationTokenSource();
b.注册取消时的操作,Token.Register;
c.发出取消请求,CancelAfter/Cancel;
d.在任务创建取消标记,CancellationToken.Token,使用标记的IsCancellationRequested
属性检查是否发出取消申请;
[线程池]
ThreadPool.QueueUserWorkItem(WaitCallback):
向线程池请求线程调度,执行WaitCallback方法;
[thread类]
new Thread(ThreadStart).Start启动一个线程;
new Thread(ParameterizedThreadStart).Start;
定义一个类,在类中定义传递给Thread构造函数的参数的方法;
IsBackground指定线程是否为前台;
Priority定义线程优先级;
[线程同步]
1.lock:
应用于引用对象;
i++:
a.从内存中获取一个值;
b.该值递增1;
c.存储回内存;
2.Interlocked.Increment:以原子方式读写变量;
3.Monitor.TryEnter(obj, 500, ref lockTaken):
等待500ms根据lockTaken的状态判断是否获得锁;
Monitor.Exit释放锁;
4.Mutex:
new Mutex(false, "name", bool isNew);
isNew判断名称为name的互斥是否存在;
进程根据name判断互斥是否存在;
Mutex.OpenExisting()打开已经存在的Mutex;
Mutex.Watione获得互斥;
Mutex.ReleaseMutex释放互斥;
5.SemaphoreSlim:
a.创建new SemaphoreSlim(init, max)实例,
init指定初始化信号量请求数,max指定最大信号量请求数;
b.semaphore.Wait(t)等待t时间后获取信号量;
c.Release释放信号量;
6.ManualResetEventSlim:
a.创建ManualResetEventSlim、WaitHandle实例,
b.类中定义ManualResetEventSlim对象,使用创建ManualResetEventSlim的WaitHandle
属性给WaitHandle实例赋值;
c.任务中使用ManualResetEventSlim的set发送信号;
d.WaitHandle.WaitAny()等待ManualResetEventSlim的set发送的信号;
e.ManualResetEventSlim的Reset复位到不发送信号的状态;
7.ReaderWriterLockSlim:
a.创建ReaderWriterLockSlim实例;
b.EnterReadLock获取读锁,ExitReadLock释放读锁;
c.TryEnterWriteLock(t)在t的时间里尝试获取锁;
ExitWriteLock释放锁;
[Timer类]
System.Threading.Timer定义一个定时任务;
System.Timers.Timer:
a.创建定时器;
b.AutoReset属性定义是否重复;
c.Elapsed定义方法;
e.Start/Stop开始/停止;
<文件和注册表操作>
[读写文件]
File.ReadAllText(),ReadAllLines(),ReadAllBytes()
读取文件内容;
File.WriteAllText()....
写文件;
[ReadByte]
从文件中读取一个字节,把结果转化成0-255之间的整数;
<网络>
<ADO.NET Entity Framework>
<核心WPF>
<用WPF编写业务应用程序>
<WCF>