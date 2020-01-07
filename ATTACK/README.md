SYN-ATTACK
==========
作者: Xuebi

邮箱: 2685043624@qq.com

Created Time: 2020.01.07

说明：
----
1.本程序采用多线程，模拟syn洪水进攻。
2.采用wireshack查看数据包

使用:
-----
```
$ gcc -o syn_attack syn_attack.c -lpthread
$ sudo ./syn_attack <IPaddress> <port>
```
例如
```
$ sudo ./syn_attack 0.0.0.0 80
$ sudo ./syn_attack www.pornhub.com 80
```


注意：
-----
本代码仅供交流学习，不得进行非法用途。
