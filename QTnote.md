[1]隐式共享：引用隐式共享只需要一个指针访问数据，修改共享数据时需要复制。
[2]eclipse配置c++环境：
a.安装cdt插件;
b.安装c/c++编译器MINGW;
c.配置环境变量path;
[3]git上传代码:
1.新建Git仓库;
2.git config --user.name(email),配置用户名和邮箱
3.ssh-keygen -T rsa生成公匙，添加到GitHub上，ssh -T git@github.com检查
4.git add origin master添加远程仓库
5.git pull origin master --allow-unrelated-histories合并代码，
然后 git push origin master提交代码
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