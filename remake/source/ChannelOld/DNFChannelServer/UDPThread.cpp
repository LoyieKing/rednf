#include "ChannelServiceApp.h"
#include "Socket.h"
#include <unistd.h>

ChannelServiceApp::UDPThread::UDPThread()
{
    pHandler_ = NULL;
}

ChannelServiceApp::UDPThread::~UDPThread()
{
}

void ChannelServiceApp::UDPThread::logError()
{
    __errno_location();
}

void ChannelServiceApp::UDPThread::loop(void* temp)
{
    puts("Start up UDPThread");
    UDPSocket sUDP;
    if (sUDP.open())
    {
        if (sUDP.bind((unsigned short)nPort_, true))
        {
            printf("succeeded in binding UDP socket port! #%d\n", nPort_);
            while (true)
            {
                if (isTerminating())
                {
                    break;
                }
                usleep(10);
                char recv_buf[0x1000];
                int nRead = sUDP.recv(recv_buf, 0x1000);
                if (nRead < 0)
                {
                    printf("[ERROR] UDP Thread Recv Error(%s)", strerror(*__errno_location()));
                }
                else if (nRead != 0)
                {
                    gFileLogWarn << "UDP read=" << nRead << endl;
                    pHandler_->dispatch(recv_buf, nRead, 0);
                }
            }
            setTerminated();
        }
        else
        {
            printf("failed to bind UDP socket port #%d\n", nPort_);
        }
    }
    else
    {
        puts("failed to open UDP socket port");
    }
}
