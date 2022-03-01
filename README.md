# mingssager
### Requirements

Implement an instant messaging software based on TCP and UDP:

1. Include both server-side and client-side;
2. Support user registration, login, and password retrieval (based on TCP);
3. If two users are online at the same time, use p2p to communicate without being transited by the server (based on TCP);
5. Support p2p **reliable** file transfer (base on UDP);
6. The data stored on the server are strongly encrypted;
7. Support at least two groups of users to communicate and transfer files online at the same time;
8. File transfer shows good performance with full use of network bandwidth;
9. User-friendly GUI.

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
