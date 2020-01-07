# 服务器模型实现

----------------------------------------------

作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.08

----------------------------------------------

##### 说明：本文件夹主要实现各种UDP，TCP服务器-客户端模型，其中客户端向服务器发送“TIME”， 服务器向客户端返回当前时间，具体如下

1.循环UDP服务器  
* 最基本的服务器模型实现，利用循环进行数据处理

2.循环TCP服务器  
* 最基本的服务器模型实现，利用循环进行数据处理

3.UDP并发服务器_多进程  
* 利用fork()实现多进程
* ACCEPT()函数完成后，再进行fork()，利用新进程处理数据

4.TCP并发服务器_多进程  
* 利用fork()实现多进程
* ACCEPT()函数完成后，再进行fork()，利用新进程处理数据

5.TCP高级并发服务器_多进程  
* 利用fork()实现多进程
* 利用多进程处理数据，具体为fork()出多个进程后，在每个新进程中进一步实现ACCEPT()函数，最后再处理数据
6.TCP高级并发服务器_多线程  
* 利用pthread_create()实现多线程
* 利用线程池处理数据，具体为利用pthread_create()出多个线程后，进一步实现ACCEPT()函数，最后再处理数据

7.IO_复用SELECT
* 利用pthread_create()实现两个线程，线程一负责利用ACCEPT()与客户端完成3次握手，线程二利用select()监听客户端端口，并对有数据到达的端口进行数据处理


##### 使用:

1. UDP类模型 ---非线程实现
```
$ gcc -o UDP_server UDP_server.c
$ gcc -o UDP_client UDP_client.c
$ ./ UDP_server
$ ./UDP_client
```

2. TCP类模型 ---非线程实现
```
$ gcc -o TCP_server TCP_server.c
$ gcc -o TCP_client TCP_client.c
$ ./ TCP_server
$ ./TCP_client
```

3. TCP类模型 ---线程实现
```
$ gcc -o TCP_server TCP_server.c -lpthread
$ gcc -o TCP_client TCP_client.c
$ ./TCP_server
$ ./TCP_client
```

```
注意：./TCP_server.c与./TCP_client在两个命令框中运行
```

##### 注意：

本代码仅供交流学习，不得进行非法用途。
