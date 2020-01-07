PING
==========
作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.07

说明：
----
1.本程序模拟PING进行ICMP REQUEST和ICMP REPLY

使用:
-----
```
$ gcc -o packet.o -c packet.c 
$ gcc -o overall.o -c overall.c 
$ gcc -o ping.o -c ping.c 
$ gcc -o ping ping.o packet.o overall.o -lpthread
$ sudo ./ping <IPaddress> 
```

例如
```
$ sudo ./ping 0.0.0.0 
$ sudo ./ping www.pornhub.com
```


注意：
-----
本代码仅供交流学习，不得进行非法用途。
