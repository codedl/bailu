[按钮监听事件]
为按钮设置监听事件，重写onClick函数
[文件]
内存文件:通过openFileOutput和openFileInput获取FileOutputStream和
FileInputStream对象，调用接口进行读写关闭。
SD文件:
1.添加读写sd卡的权限，动态请求权限;
2.检查sd卡是否可用
3.获取sd卡在系统的路径
4.在路径下创建文件进行读写
[Tcp/Ip]
1.服务器创建指定端口号的socket，调用accept等待客户端连接
2.客户端创建指定ip和端口号的socket连接服务器
3.使用建立连接后的socket获取输入输出对象,使用对象进行读写操作
4.bufferdwriter需要flush
[线程]
a.Runnable
1.创建runnable对象
2.通过以runnable对象为参数的构造函数创建Thread对象
3.start运行
b.join等待另一个线程执行完毕，interrupted中断线程执行
c.synchronized同步资源和方法，同一时间只允许一个线程访问资源
[传统蓝牙]
1.扫描：在广播里获取蓝牙设备信息；
2.连接：蓝牙的连接需要和设备的uuid相同
[android studio打开项目]
下载gradle,删除原先项目的配置文件,重新构建项目,添加库
[蓝牙录音]
1.连接蓝牙耳机;
2.创建MediaRecorder对象，设置格式
3.创建AudioManager对象，跟蓝牙耳机建立Sco连接，开始录音
[Java:反射]
a.访问变量:
1.获取对象在类中定义的变量field;
2.通过field取值;判断field的类型,进行赋值
b.访问方法：
1.获取对象中定义的方法，根据方法的参数，返回值调用该方法
[约束布局]
1.layout_constraintTop_toBottomOf：
控件的top在某个控件的bottom下方
[tomcat服务器]
1.startup.bat启动服务器;
2.在server.xml中配置<Context docBase="D:\web" path="/web"/>
[android studio导入jar包]
1.找到libs目录没有就新建
2.sync
[android界面]
UI线程与非UI线程通信：
1.handle:在ui线程定义static的Handle子类，接收来自非ui线程的消息
2.Intent:定义Intent广播机制，在广播接收者处理intent消息

[visio裁剪]
文件->选项->自定义功能区->开发工具;
选中形状->开发工具的操作;

[对话框]
1.生成对话框
2.设置控件，标题，按钮的名称和点击事件
3.创建，显示

[过滤文件]
1.list(listFiles)添加参数
2.重写accept方法，根据accept创建文件
3.判断文件是否符合，符合返回true

[tableLayout]
1.创建布局
2.TableRow加入行
3.在行中定义宽度

[Fragment]
1.在界面为Fragment定义个布局作容器
2.创建Fragment的布局
3.定义操作Fragment的子类
4.在界面里定义子类的对象
5.FragmentManager获取beginTransaction返回的FragmentTransaction
6.hide;show;add等控制Fragment，最后commit