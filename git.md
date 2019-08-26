////dingle19930328
[git上传代码]
1.新建Git仓库;
2.git config --global user.name(email),配置用户名和邮箱
3.ssh-keygen -t rsa生成公匙，添加到GitHub上，ssh -T git@github.com检查
4.添加远程仓库:git remote add 主机名 主机地址
5.git pull origin master --allow-unrelated-histories合并代码，
然后 git push origin master提交代码

[git更新单个文件]
1.git fetch 主机名  分支(:本地分支)
2.git checkout 主机名/分支  文件
[git管理多个远程仓库]
1.git remote add 主机名 主机地址：
如:git remote add qtc git@github.com:codedl/bailu;
就可以通过qtc引用git@github.com:codedl/bailu主机;
2.git pull/push  主机名 分支(:本地分支)
[git删除主机和分支]
git branch -d 本地分支名
git push origin --delete 远程分支名
git remote remove 主机名
[git删除]
git rm --cached path -r 删除缓冲区中文件
git rm -f  path 删除缓冲区和物理文件s