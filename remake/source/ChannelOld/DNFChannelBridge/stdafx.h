#ifndef STDAFX_H_
#define STDAFX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C" int compress2(unsigned char* dest, unsigned long* destLen, const unsigned char* source, unsigned long sourceLen, int level);

typedef char TCHAR;
typedef char CHAR;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef int BOOL;
typedef int SOCKET;
typedef int INT;
typedef unsigned int UINT;
typedef char* LPSTR;
typedef const char* LPCSTR;

#endif // STDAFX_H_
