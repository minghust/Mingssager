#ifndef CLIENTSTART_H
#define CLIENTSTART_H

#pragma comment(lib, "ws2_32.lib")

#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <winbase.h>
#include <iostream>
#include <vector>
#include <string>
#define BUFLEN 255

//全局的临界区保护变量，以保护主线程和子线程都要访问的sendBuf
CRITICAL_SECTION gCriticalSection;

void ClientStart(void);

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;

#endif // CLIENTSTART_H
