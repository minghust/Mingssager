#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <winsock2.h>
#include <list>
#include <algorithm>
#include <string.h>
#include <windows.h>
#include <map>
#include <sstream>
#include "login.h"

using namespace std;
const int MAXCONN = 5;
const int BUFLEN = 255;
const string LOGIN = "login";
const string REGISTER = "register";
const string GETPASSWDBACK = "getpasswdback";
const string GETUSERLIST = "getuserlist";
const string OFFLINE = "offline";
const string UPDATE = "update";

typedef list<SOCKET> ListCONN;
typedef list<SOCKET> ListConErr;

void main(int argc, char* argv[])
{
	WSADATA wsaData;
	int nRC;
	sockaddr_in srvAddr, clientAddr;
	SOCKET srvSock;
	int nAddrLen = sizeof(sockaddr);
	char sendBuf[BUFLEN], recvBuf[BUFLEN];
	ListCONN conList;		//����������Ч�ĻỰSOCKET
	ListCONN::iterator itor;
	ListConErr conErrList;	//��������ʧЧ�ĻỰSOCKET
	ListConErr::iterator itor1;
	FD_SET rfds, wfds;
	u_long uNonBlock;

	vector<UserList>userList;
	map<string, string>offlineBuf; // offline message
	//��ʼ�� winsock
	nRC = WSAStartup(0x0101, &wsaData);
	if (nRC)
	{
		printf("Server initialize winsock error!\n");
		return;
	}
	if (wsaData.wVersion != 0x0101)
	{
		printf("Server's winsock version error!\n");
		WSACleanup();
		return;
	}
	printf("Server's winsock initialized !\n");

	//���� TCP socket
	srvSock = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSock == INVALID_SOCKET)
	{
		printf("Server create socket error!\n");
		WSACleanup();
		return;
	}
	printf("Server TCP socket create OK!\n");

	//�� socket to Server's IP and port 5050
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(5050);
	srvAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	nRC = bind(srvSock, (LPSOCKADDR)&srvAddr, sizeof(srvAddr));
	if (nRC == SOCKET_ERROR)
	{
		printf("Server socket bind error!\n");
		closesocket(srvSock);
		WSACleanup();
		return;
	}
	printf("Server socket bind OK!\n");

	//��ʼ�������̣��ȴ��ͻ�������
	nRC = listen(srvSock, MAXCONN);
	if (nRC == SOCKET_ERROR)
	{
		printf("Server socket listen error!\n");
		closesocket(srvSock);
		WSACleanup();
		return;
	}

	//��srvSock��Ϊ������ģʽ�Լ����ͻ���������
	uNonBlock = 1;
	ioctlsocket(srvSock, FIONBIO, &uNonBlock);

	while (1)
	{
		//��conList��ɾ���Ѿ���������ĻỰSOCKET
		for (itor1 = conErrList.begin(); itor1 != conErrList.end(); itor1++)
		{
			itor = find(conList.begin(), conList.end(), *itor1);
			if (itor != conList.end()) conList.erase(itor);
		}

		//���read,write�׽��ּ���
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//���õȴ��ͻ���������
		FD_SET(srvSock, &rfds);

		for (itor = conList.begin(); itor != conList.end(); itor++)
		{
			//�����лỰSOCKET��Ϊ������ģʽ
			uNonBlock = 1;
			ioctlsocket(*itor, FIONBIO, &uNonBlock);
			//���õȴ��ỰSOKCET�ɽ������ݻ�ɷ�������
			FD_SET(*itor, &rfds);
			FD_SET(*itor, &wfds);
		}
		//��ʼ�ȴ�
		int nTotal = select(0, &rfds, &wfds, NULL, NULL);

		//���srvSock�յ��������󣬽��ܿͻ���������
		if (FD_ISSET(srvSock, &rfds))
		{
			nTotal--;
			//�����ỰSOCKET
			SOCKET connSock = accept(srvSock, (LPSOCKADDR)&clientAddr, &nAddrLen);
			if (connSock == INVALID_SOCKET)
			{
				printf("Server accept connection request error!\n");
				closesocket(srvSock);
				WSACleanup();
				return;
			}
			sprintf(sendBuf, "%s",inet_ntoa(clientAddr.sin_addr));
			printf("%s: ", sendBuf);
			printf("%u\n", clientAddr.sin_port);

			//�������ĻỰSOCKET������conList��
			conList.insert(conList.end(), connSock);
		}
		if (nTotal > 0)
		{
			//���������Ч�ĻỰSOCKET�Ƿ������ݵ���
			//���Ƿ���Է�������
			for (itor = conList.begin(); itor != conList.end(); itor++)
			{
				//����ỰSOCKET���Է������ݣ�
				//����ͻ�������Ϣ
				//if (FD_ISSET(*itor, &wfds))
				//{
				//	//������ͻ����������ݣ�����
				//	if (strlen(sendBuf) > 0)
				//	{
				//		nRC = send(*itor, sendBuf, strlen(sendBuf), 0);
				//		if (nRC == SOCKET_ERROR)
				//		{
				//			//�������ݴ���
				//			//��¼�²�������ĻỰSOCKET
				//			conErrList.insert(conErrList.end(), *itor);
				//		}
				//		else//�������ݳɹ�����շ��ͻ�����
				//			memset(sendBuf, '\0', BUFLEN);
				//	}
				//}

				//����ỰSOCKET�����ݵ���������ܿͻ�������
				if (FD_ISSET(*itor, &rfds))
				{
					nRC = recv(*itor, recvBuf, BUFLEN, 0);
					if (nRC == SOCKET_ERROR)
					{
						//�������ݴ���
						//��¼�²�������ĻỰSOCKET
						conErrList.insert(conErrList.end(), *itor);
					}
					else
					{
						vector<string>op;
						string str(recvBuf);
						SplitString(str, op, "!");
						// �������ݳɹ��������ڷ��ͻ������У�
						// �Է��͵����пͻ�ȥ
						if (op[0] == LOGIN)
						{
							cout << "recvBuf" << op[1] << endl;
							vector<string> v;
							SplitString(op[1], v, "+");
							int re = VerifyUserLogin(v[0], v[1]);
							if (re == LOGIN_SUCCESS)
							{
								sprintf(sendBuf, "%s", "LOGIN_SUCCESS");
								send(*itor, sendBuf, strlen(sendBuf), 0);
								
								// send to client userList
								UpdateUserList(userList);
								string s = "";
								for (int i = 0; i < userList.size(); i++)
								{
									if (userList[i].name == v[0])
										userList[i].isOnline = true;
									s += userList[i].name + "+" + v[2] + "+";
									s += userList[i].isOnline ? "true" : "false";
									s += "!";
								}
								sprintf(sendBuf, "%s", s.c_str());
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
							else if (re == PSW_ERROR)
							{
								sprintf(sendBuf, "%s", "PSW_ERROR");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
							else if (re == INVALID_USERNAME)
							{
								sprintf(sendBuf, "%s", "INVALID_USERNAME");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
							else
							{
								sprintf(sendBuf, "%s", "FILE_ERROR");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
						}
						else if (op[0] == REGISTER)
						{
							vector<string> v;
							cout << "recvBuf:" << op[1] << endl;
							SplitString(op[1], v, "+");
							bool re = SaveInfo(v[0], v[1], v[2]);
							if (re)
							{
								sprintf(sendBuf, "%s", "REGISTER_SUCCESS");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
							else
							{
								sprintf(sendBuf, "%s", "REGISTER_FAILURE");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
						}
						else if (op[0] == GETPASSWDBACK)
						{
							cout << "recvBuf" << op[0] << endl;
							vector<string> v;
							SplitString(op[1], v, "+");
							bool re = GetbackPasswd(v[0], v[1]);
							if (re)
							{
								sprintf(sendBuf, "%s", "GETPASSWDBACK_SUCCESS");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
							else
							{
								sprintf(sendBuf, "%s", "GETPASSWDBACK_FAILURE");
								send(*itor, sendBuf, strlen(sendBuf), 0);
							}
						}
						else if (op[0] == OFFLINE)
						{
							for (int i = 0; i < userList.size(); i++)
							{
								if (userList[i].name == op[1])
								{
									userList[i].isOnline = false;
								}
							}
						}
						else if (op[0] == UPDATE)
						{
							UpdateUserList(userList);
							string s = "";
							vector<string>v;
							SplitString(op[1], v, "+");
							for (int i = 0; i < userList.size(); i++)
							{
								if (userList[i].name == v[0])
									userList[i].isOnline = true;
								s += userList[i].name + "+" + v[1] + "+";
								s += userList[i].isOnline ? "true" : "false";
								s += "!";
							}
							sprintf(sendBuf, "%s", s.c_str());
							send(*itor, sendBuf, strlen(sendBuf), 0);
						}
					}
				}
			}
		}
	}
	closesocket(srvSock);
	WSACleanup();
}

void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}