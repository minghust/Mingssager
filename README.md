# mingssager
**要求**：基于TCP和UDP 协议实现一个即时通讯工具，体功能要求包括：

1. 工具包括服务器端和客户端；
2. 具备用户注册、登录找回密码功能（基于TCP协议）；
3. 两个用户如果同时在线，采点到通信方式进行聊天息不需要过服务器中转，服务器也不保存（基 于 TCP 协议）； 协议）；
4. 支持离线消息（基于TCP协议）；
5.  支持点到可靠文件传输（基UDP协议）；
6. 存储在服务器端的数据需要进行强加密；
7. 支持不少于两组用户同时在线交流和传输文件；
8. 文件传输具有良好的性能，够充分利用网路带宽；
9. 人机交互友好，软件易用性强。

### Implement

based on **Qt** and **winsock**, almost use **one month** to finish this huge software

- Environment : Windows 10
- Qt : qt 5.9 

### Features

- server
  - winsock
  - I/O multiplexing: **select**
  - file Information
- client
  - QTcpSocket
  - QUdpSocket
  - QTcpServer

### Methods for reliable file transmission(based on UDP)

- Timeout
- md5
- GBN
- Qt signal and slot 
