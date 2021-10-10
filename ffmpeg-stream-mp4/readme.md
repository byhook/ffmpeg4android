亲测可用，将本地视频文件，ffmeg处理程rtsp流rtmp流上传至rtmp服务器


## nginx搭建rtmp服务器（windows）

快速搭建RTMP服务器，可用于判断生产环境中哪个节点出了问题。

一、下载nginx、nginx的rtmp模块

nginx下载地址：http://nginx.org/en/download.html 下载完毕后解压到自定义目录

rtmp模块下载地址：https://github.com/arut/nginx-rtmp-module/

rtmp模块需下载到已经解压的nginx的目录，进入nginx的目录，#没有安装git客户端的话先安装git客户端，下载地址：https://git-scm.com/downloads ，安装完毕后执行以下命令

cd c:\nginx
git clone https://github.com/arut/nginx-rtmp-module/

二、修改nginx的配置文件

配置文件路径为

c:\nginx\conf\nginx-win.conf
用记事本编辑nginx-win.conf文件并保存

#user  nobody;
# multiple workers works !
worker_processes  2;

#error_log  logs/error.log;
#error_log  logs/error.log  notice;
#error_log  logs/error.log  info;

#pid        logs/nginx.pid;


events {
    worker_connections  8192;
    # max value 32768, nginx recycling connections+registry optimization = 
    #   this.value * 20 = max concurrent connections currently tested with one worker
    #   C1000K should be possible depending there is enough ram/cpu power
    # multi_accept on;
}

rtmp {
    server {
        listen 1935;#监听端口,若被占用,可以更改
        chunk_size 4000;#上传flv文件块儿的大小
        application live { #创建一个叫live的应用
             live on;#开启live的应用
             allow publish 127.0.0.1;#
             allow play all;
        }
    }
}

http {
    #include      /nginx/conf/naxsi_core.rules;
    include       mime.types;
    default_type  application/octet-stream;

    #log_format  main  '$remote_addr:$remote_port - $remote_user [$time_local] "$request" '
    #                  '$status $body_bytes_sent "$http_referer" '
    #                  '"$http_user_agent" "$http_x_forwarded_for"';

    #access_log  logs/access.log  main;

#     # loadbalancing PHP
#     upstream myLoadBalancer {
#         server 127.0.0.1:9001 weight=1 fail_timeout=5;
#         server 127.0.0.1:9002 weight=1 fail_timeout=5;
#         server 127.0.0.1:9003 weight=1 fail_timeout=5;
#         server 127.0.0.1:9004 weight=1 fail_timeout=5;
#         server 127.0.0.1:9005 weight=1 fail_timeout=5;
#         server 127.0.0.1:9006 weight=1 fail_timeout=5;
#         server 127.0.0.1:9007 weight=1 fail_timeout=5;
#         server 127.0.0.1:9008 weight=1 fail_timeout=5;
#         server 127.0.0.1:9009 weight=1 fail_timeout=5;
#         server 127.0.0.1:9010 weight=1 fail_timeout=5;
#         least_conn;
#     }

    sendfile        off;
    #tcp_nopush     on;

    server_names_hash_bucket_size 128;

## Start: Timeouts ##
    client_body_timeout   10;
    client_header_timeout 10;
    keepalive_timeout     30;
    send_timeout          10;
    keepalive_requests    10;
## End: Timeouts ##

    #gzip  on;

    server {
        listen       80;
        server_name  localhost;

        #charset koi8-r;

        #access_log  logs/host.access.log  main;

        ## Caching Static Files, put before first location
        #location ~* \.(jpg|jpeg|png|gif|ico|css|js)$ {
        #    expires 14d;
        #    add_header Vary Accept-Encoding;
        #}

# For Naxsi remove the single # line for learn mode, or the ## lines for full WAF mode
        location / {
            #include    /nginx/conf/mysite.rules; # see also http block naxsi include line
            ##SecRulesEnabled;
        	  ##DeniedUrl "/RequestDenied";
	          ##CheckRule "$SQL >= 8" BLOCK;
	          ##CheckRule "$RFI >= 8" BLOCK;
	          ##CheckRule "$TRAVERSAL >= 4" BLOCK;
	          ##CheckRule "$XSS >= 8" BLOCK;
            root   html;
            index  index.html index.htm;
        }

# For Naxsi remove the ## lines for full WAF mode, redirect location block used by naxsi
        ##location /RequestDenied {
        ##    return 412;
        ##}

## Lua examples !
#         location /robots.txt {
#           rewrite_by_lua '
#             if ngx.var.http_host ~= "localhost" then
#               return ngx.exec("/robots_disallow.txt");
#             end
#           ';
#         }

        #error_page  404              /404.html;

        # redirect server error pages to the static page /50x.html
        #
        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }

        # proxy the PHP scripts to Apache listening on 127.0.0.1:80
        #
        #location ~ \.php$ {
        #    proxy_pass   http://127.0.0.1;
        #}

        # pass the PHP scripts to FastCGI server listening on 127.0.0.1:9000
        #
        #location ~ \.php$ {
        #    root           html;
        #    fastcgi_pass   127.0.0.1:9000; # single backend process
        #    fastcgi_pass   myLoadBalancer; # or multiple, see example above
        #    fastcgi_index  index.php;
        #    fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
        #    include        fastcgi_params;
        #}

        # deny access to .htaccess files, if Apache's document root
        # concurs with nginx's one
        #
        #location ~ /\.ht {
        #    deny  all;
        #}
    }


    # another virtual host using mix of IP-, name-, and port-based configuration
    #
    #server {
    #    listen       8000;
    #    listen       somename:8080;
    #    server_name  somename  alias  another.alias;

    #    location / {
    #        root   html;
    #        index  index.html index.htm;
    #    }
    #}


    # HTTPS server
    #
    #server {
    #    listen       443 ssl spdy;
    #    server_name  localhost;

    #    ssl                  on;
    #    ssl_certificate      cert.pem;
    #    ssl_certificate_key  cert.key;

    #    ssl_session_timeout  5m;

    #    ssl_prefer_server_ciphers On;
    #    ssl_protocols TLSv1 TLSv1.1 TLSv1.2;
    #    ssl_ciphers ECDH+AESGCM:ECDH+AES256:ECDH+AES128:ECDH+3DES:RSA+AESGCM:RSA+AES:RSA+3DES:!aNULL:!eNULL:!MD5:!DSS:!EXP:!ADH:!LOW:!MEDIUM;

    #    location / {
    #        root   html;
    #        index  index.html index.htm;
    #    }
    #}

}
三、启动nginx服务

在nginx目录启动nginx服务

nginx,exe -c conf\nginx-win.conf
启动后保持此窗口


四、使用推流客户端推流

这里使用OBS推流软件，下载地址：https://obsproject.com/download

在OBS软件设置中添加推流地址：

rtmp://127.0.0.1:1935/live

OBS软件中添加推流视频文件或视频设备开启推流，OSB的操作参考下面的截图





五、测试推流是否正常

使用VLC软件，下载地址：https://www.videolan.org/

在VLC中打开推流地址：

rtmp://127.0.0.1:1936/live/


串流在VLC中正常播放


推流正常运行。

附Windows平台nginx命令：

#1、启动：

C:\nginx>nginx.exe -c conf\nginx-win.conf   #指定配置文件启动

#2、停止：
C:\nginx>nginx.exe -s stop   #或
C:\nginx>nginx.exe -s quit
#注：stop是快速停止nginx，可能并不保存相关信息；quit是完整有序的停止nginx，并保存相关信息。

#3、重新载入Nginx：
C:\nginx>nginx.exe -s reload
#当配置信息修改，需要重新载入这些配置时使用此命令。

#4、重新打开日志文件：
C:\nginx>nginx.exe -s reopen

#5、查看Nginx版本：
C:\nginx>nginx -v
