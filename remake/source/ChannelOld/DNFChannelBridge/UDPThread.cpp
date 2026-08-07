#include "ChannelServiceApp.h"
#include "Socket.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

ChannelServiceApp::UDPThread::UDPThread()
    : Thread(), TManager<ChannelService>()
{
    pHandler_ = NULL;
}

ChannelServiceApp::UDPThread::~UDPThread()
{
}

void ChannelServiceApp::UDPThread::logError()
{
    int err = *__errno_location();
}

void ChannelServiceApp::UDPThread::loop(void* temp)
{
    puts("Start up UDPThread");
    UDPSocket sUDP;
    bool ret = sUDP.open();
    if (ret == false)
    {
        puts("failed to open UDP socket port");
    }
    else
    {
        ret = sUDP.bind((unsigned short)nPort_, true);
        if (ret == false)
        {
            printf("failed to bind UDP socket port #%d\n");
        }
        else
        {
            printf("succeeded in binding UDP socket port! #%d\n");
            while (true)
            {
                if (isTerminating())
                {
                    break;
                }
                TSystem<LinuxSystem>::usleep(10);
                char recv_buf[0x1000];
                int nRead = sUDP.recv(recv_buf, 0x1000);
                if (nRead < 0)
                {
                    printf("[ERROR] UDP Thread Recv Error(%s)", strerror(errno));
                }
                else if (nRead != 0)
                {
                    pHandler_->dispatch(recv_buf, nRead, 0);
                }
            }
            setTerminated();
        }
    }
}
