#ifndef DBMW_DNFTCPHANDLER_H_
#define DBMW_DNFTCPHANDLER_H_

#include "DBMWCommon.h"


class CTcpHandler
{
public:
    CTcpHandler();
    ~CTcpHandler();
    int WaitForEvent();
    int ResetEpoll(int flag);
    int SetPeer(void* peer, int fd, bool flag);
    void* GetEventPtr(int idx);
    char IsSetInEvent(int idx);
    char IsSetOutEvent(int idx);
    char IsSetErrEvent(int idx);
    CProtocol* m_epoll;     // +0（ORIG：基类指针赋值产生 mov ebx,eax;mov eax,edx 形态）
};

#endif  // DBMW_DNFTCPHANDLER_H_
