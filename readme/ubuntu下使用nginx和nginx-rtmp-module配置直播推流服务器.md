
本来准备在`centos服务器`上搭建的，因为笔者工作系统是`ubuntu`，因此直接在本机上搭建，更方便快捷，配置过程比较简单，记录一下。

## 目录

- [配置环境](#配置环境)
- [安装`obs-studio`开始第一次推流](#安装`obs-studio`开始第一次推流)
- [安装vlc播放器开始拉流](#安装vlc播放器开始拉流)
- [配置环境](#配置环境)
- [配置环境](#配置环境)


#### 配置环境

`操作系统：ubuntu 16.05`

[nginx-1.15.5版本](http://nginx.org/download/nginx-1.15.5.tar.gz)

同步`nginx-rtmp-module`代码
```
git clone https://github.com/arut/nginx-rtmp-module.git
```
最好事先安装好以下依赖
```java
sudo apt-get update
sudo apt-get install openssl libssl-dev
sudo apt-get install libpcre3 libpcre3-dev
```

编写`build.sh脚本`编译`nginx和nginx-rtmp-module`模块

```java
cd nginx-1.15.5

./configure \
--prefix=/usr/local/nginx \
--add-module=../nginx-rtmp-module \
--with-http_ssl_module

sudo make
sudo make install

cd ..
```

目录结构如图所示：

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181031082316874.png)

开始编辑`nginx.conf`文件

```java
sudo vim /usr/local/nginx/conf/nginx.conf
```

在`http`节点下面添加配置

```java
http {
  ......
}

rtmp {
    server {
        listen 1935;
        application onzhou {
            live on;
            record off;
        }
    }
}
```

`常用的nginx命令：`
```java
ngnix -s reload：表示重新加载配置文件
ngnix -s reopen: 再次打开日志文件
ngnix -s stop : 停止服务器
ngnix -s quit : 退出服务器
```

`启动nginx服务`
```java
sudo /usr/local/nginx/sbin/nginx
```

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181031082330363.png)

#### 安装`obs-studio`开始第一次推流
```java
sudo add-apt-repository ppa:obsproject/obs-studio
sudo apt-get update && sudo apt-get install obs-studio
```
配置流地址如下
```java
rtmp://192.168.1.102:1935/onzhou/live
```

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181031082338649.png)

`点击开始推流`

#### 安装vlc播放器开始拉流
```java
sudo apt install vlc
```

使用vlc播放网络串流

![](https://github.com/byhook/ffmpeg4android/blob/master/readme/images/20181031082353133.png)
