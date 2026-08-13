#ifndef NSL_TCPTHREAD_H_
#define NSL_TCPTHREAD_H_

#include "Thread.h"

namespace nsl {

class TCPThread : public Thread
{
public:
    TCPThread();
    virtual ~TCPThread();
    virtual void loop(void* pParam);
    void setPort(int nPort)
    {
        nPort_ = nPort;
    }
    int getPort() const
    {
        return nPort_;
    }

    int nPort_;
};

} // namespace nsl

#endif // NSL_TCPTHREAD_H_
