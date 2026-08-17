#ifndef NSL_LOGSENDTHREAD_H_
#define NSL_LOGSENDTHREAD_H_

#include <deque>
#include <string.h>

#include "Thread.h"

namespace nsl {

class TCPUser;
class Message;
class ConInterface;

class LogSendThread : public Thread
{
public:
    LogSendThread();
    virtual ~LogSendThread();
    virtual void loop(void* temp);
    void CheckLogServer();
    void SendLogMsg(char* buf, int len);
    // ORIG：头内 inline（point 无 out-of-line 符号）
    void SetLogServerIp(char* ip)
    {
        strcpy(mLogServerIp, ip);
    }
    char* GetLogServerIp();
    // ORIG：头内 inline（point 无 out-of-line 符号）
    void SetLogServerPort(int port)
    {
        mLogServerPort = port;
    }
    int GetLogServerPort();

private:
    ConInterface* mLogCon;
    ThreadLock mThreadLock;
    TCPUser* mLogUser;
    char mLogServerIp[17];
    int mLogServerPort;
    std::deque<Message*> mLogMessagePool;
};

} // namespace nsl

#endif // NSL_LOGSENDTHREAD_H_
