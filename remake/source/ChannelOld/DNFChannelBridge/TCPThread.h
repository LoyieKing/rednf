#ifndef TCPTHREAD_H_
#define TCPTHREAD_H_

#include "ChannelServiceApp.h"

namespace ChannelServiceApp
{
    class TCPThread : public Thread, public TManager<ChannelService>
    {
    public:
        TCPThread();
        ~TCPThread();
        virtual void loop(void* temp);
        void setHandler(TCPHandler* pHandler)
        {
            pHandler_ = pHandler;
        }

    private:
        TCPHandler* pHandler_;
    };
}

#endif // TCPTHREAD_H_
