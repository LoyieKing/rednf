#ifndef NSL_IHANDLER_H_
#define NSL_IHANDLER_H_

#include <stddef.h>

namespace nsl {

class TCPSendThread;
class ITimeHandler;
class CMsgCell;
class TCPUser;
class Message;

class IHandler
{
public:
    typedef unsigned int (IHandler::*DBHandlerFunc)(CMsgCell*);

    // ORIG 的 IHandlerC2 只写 vtable（6 条指令），不初始化 sendTCP_/pTimeHandler；
    // 清零会使 IHandlerC2 多 4 条 mov（DIFF）。成员在使用前由 setSendThread 等赋值。
    IHandler()
    {
    }
    virtual ~IHandler();
    virtual void init();
    void setSendThread(TCPSendThread* pSendThread);

protected:
    TCPSendThread* sendTCP_;
    ITimeHandler* pTimeHandler;
};

class INetWorkHandler : public IHandler
{
public:
    typedef unsigned int (INetWorkHandler::*networkFuncType)(CMsgCell*, TCPUser*);

    virtual networkFuncType searchNetworkFunc(int nProtoID) = 0;
};

class IInterHandler : public IHandler
{
public:
    typedef unsigned int (IInterHandler::*interFuncType)(CMsgCell*);

    IInterHandler()
    {
    }
    virtual ~IInterHandler()
    {
    }
    virtual interFuncType searchInterFunc(int nProtoID) = 0;
};

class IDBHandler : public IHandler
{
public:
    virtual ~IDBHandler()
    {
    }
    virtual void init()
    {
    }
    virtual DBHandlerFunc SearchDBHandlerFunc(int nProtoID) = 0;
};

class ITimeHandler : public IHandler
{
public:
    virtual ~ITimeHandler()
    {
    }
};

} // namespace nsl

#endif // NSL_IHANDLER_H_
