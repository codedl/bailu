[隐式共享]
引用隐式共享只需要一个指针访问数据，修改共享数据时需要复制。
[eclipse配置c++环境]
a.安装cdt插件;
b.安装c/c++编译器MINGW;
c.配置环境变量path;
[git上传代码]
1.新建Git仓库;
2.git config --user.name(email),配置用户名和邮箱
3.ssh-keygen -T rsa生成公匙，添加到GitHub上，ssh -T git@github.com检查
4.git add origin master添加远程仓库
5.git pull origin master --allow-unrelated-histories合并代码，
然后 git push origin master提交代码
+:git rm --cached . -r 删除缓冲区中文件

[QT:socket]
1.udp编程
a.获取本地主机信息；
b.服务通过socket指定端口发送数据
c.客户端bind端口号，接收服务器数据

=======
[QT:Udpsocket]
a.获取本地主机信息；
b.服务通过socket指定端口发送数据
c.客户端bind端口号，接收服务器数据
[QT:TcpSocket]
a.服务器：
1.创建socket句柄，listen端口号和IP，等待连接
2.获取客户端socket句柄；
3.根据信号与槽机制读写数据
b.客户端
1.创建socket对象，连接指定地址和端口的服务器；
2.利用信号与槽机制添加connected，disconned，readyRead的槽函数
3.通过句柄读写数据
[QT:File]
a.QFile:
1.创建QFile对象跟文件关联；
2.调用QFile的接口操作文件，以包含写的权限打开会创建文件；
b.QTextStream:
1.创建QTextStream对象关联QFile对象；
2.使用流操作符和格式化函数输入输出；
c.QFileinfo:
创建QFileinfo对象获取文件信息
d.xml文件解析：
1.QFile关联xml文件，将文件写入QDomDocument
2.从第一个主节点firstChildElement开始，获取节点的attributes，读取每个
attributes.item的nodeName和nodeValue;
3.获取节点的firstChildElement，获取节点的attributes，读取每个
attributes.item的nodeName和nodeValue;
4.获取节点的childNodes子节点列表，读取每个子节点列表的attributes.item的nodeName和nodeValue;


