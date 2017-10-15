#include "clientstart.h"

void ClientStart(void)
{
    WSADATA wsaData;
    int nRC;
    sockaddr_in srvAddr, clientAddr;
    SOCKET clientSock;
    char sendBuf[BUFLEN], recvBuf[BUFLEN];
    FD_SET rfds, wfds;
    u_long uNonBlock;
    HANDLE hThread;
    unsigned dwThreadID;

    vector<string>cli;

    InitializeCriticalSection(&gCriticalSection);
    //��ʼ�� winsock
    nRC = WSAStartup(0x0101, &wsaData);
    if (nRC)
    {
        printf("Client initialize winsock error!\n");
        return;
    }
    if (wsaData.wVersion != 0x0101)
    {
        printf("Client's winsock version error!\n");
        WSACleanup();
        return;
    }
    printf("Client's winsock initialized !\n");

    //���� client socket
    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock == INVALID_SOCKET)
    {
        printf("Client create socket error!\n");
        WSACleanup();
        return;
    }
    printf("Client  socket create OK!\n");

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0);
    clientAddr.sin_addr.S_un.S_addr = inet_addr(argv[1]);
    nRC = bind(clientSock,(LPSOCKADDR)&clientAddr, sizeof(clientAddr));
    if (nRC == SOCKET_ERROR)
    {
        printf("Client socket bind error!\n");
        closesocket(clientSock);
        WSACleanup();
        return;
    }
    printf("Client socket bind OK!\n");


    //׼������������Ϣ��������Ҫָ���������ĵ�ַ
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(5050);
    srvAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    //���ӷ�����
    nRC = connect(clientSock,
        (LPSOCKADDR)&srvAddr, sizeof(srvAddr));
    if (nRC == SOCKET_ERROR)
    {
        printf("���ӷ�����ʧ��!\n");
        closesocket(clientSock);
        WSACleanup();
        return;
    }

    ////����һ�����̣߳���ȡ�ͻ��Ӽ����������Ϣ
    //hThread = (HANDLE)_beginthreadex(NULL,
    //	0,
    //	GetInputs,
    //	sendBuf,
    //	0,
    //	&dwThreadID);

    int i = 0;
    //��������������ݺʹӷ�������������
    while (1)
    {
        //��շ��ͺͽ��ջ�����
        memset(sendBuf, '\0', BUFLEN);
        memset(recvBuf, '\0', BUFLEN);

        //��SOCKET��Ϊ������ģʽ��
        //���ҵȴ������ݵ������߿��Է�������
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(clientSock, &rfds);
        FD_SET(clientSock, &wfds);
        uNonBlock = 1;
        ioctlsocket(clientSock, FIONBIO, &uNonBlock);
        select(0, &rfds, &wfds, NULL, NULL);

        //��������ݵ���
        if (FD_ISSET(clientSock, &rfds))
        {

            //���ܷ��������������ݲ�����ʾ
            nRC = recv(clientSock, recvBuf, BUFLEN, 0);
            if (nRC == SOCKET_ERROR)
            {
                printf("��������ʧ��!\n");
                DeleteCriticalSection(&gCriticalSection);
                closesocket(clientSock);
                WSACleanup();
                return;
            }
            else if (nRC > 0)
            {
                recvBuf[nRC] = '\0';
                if (strcmp(recvBuf, argv[1]) != 0)	// don't equal
                {
                    string s(&recvBuf[0], &recvBuf[strlen(recvBuf)]);
                    cli.push_back(s);
                    cout << "recvbuf:" << recvBuf << endl;
                }
                cout << i << "vector: ";
                for (auto it = cli.begin(); it != cli.end(); it++)
                    cout << *it << ' ';
                cout << endl << endl;
            }
        }
        //������Է�������
        if (FD_ISSET(clientSock, &wfds))
        {
            //����û��ڼ�����������Ϣ������
            if (strlen(sendBuf) > 0)
            {
                nRC = send(clientSock, sendBuf, strlen(sendBuf), 0);
                if (nRC == SOCKET_ERROR)
                {
                    printf("��������ʧ��!\n");
                    DeleteCriticalSection(&gCriticalSection);
                    closesocket(clientSock);
                    WSACleanup();
                    return;
                }
                else
                {
                    EnterCriticalSection(&gCriticalSection);
                    sendBuf[0] = '\0';
                    LeaveCriticalSection(&gCriticalSection);
                }
            }
        }
        if (strcmp(sendBuf, "exit") == 0)
        {
            break;
        }
    }
    DeleteCriticalSection(&gCriticalSection);
    closesocket(clientSock);
    WSACleanup();
}
//���߳����еĺ�������ȡ�ͻ��Ӽ����������Ϣ
unsigned __stdcall GetInputs(void *arg)
{
    char *inputs = (char *)arg;
    while (1)
    {
        printf("\n��Ҫ����:");
        EnterCriticalSection(&gCriticalSection);
        gets(inputs);
        LeaveCriticalSection(&gCriticalSection);
        if (strcmp(inputs, "exit") == 0)
            return EXIT_SUCCESS;
    }
}
