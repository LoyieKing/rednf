// df_relay_r — UDPThread（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <string.h>
#include "UDPThread.h"
#include "RelayService.h"
#include "UDPHandler.h"
#include "Helper.h"
#include "ScriptData.h"

namespace RelayServiceApp
{
UDPThread::UDPThread()
{
    m_port = 0;
    m_handler = 0;
    m_udpSocket = 0;
    m_tick = get_ms_tick();
}

UDPThread::~UDPThread()
{
}

void UDPThread::loop(void* pParam)
{
    int epoll_fd = -1;
    int max_events = 1;
    epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        printf("In %s : Can't create epoll device", "UDPThread::loop");
    }
    UDPSocket udp;
    if (udp.open() == 1)
    {
        if (udp.bind((unsigned short)m_port, true) == 1)
        {
            if (udp.setOptNonBlock() == 1)
            {
                printf("succeeded in binding UDP socket port #%d\n", m_port);
                epoll_event ev;
                ev.events = 1;
                ev.data.ptr = (void*)udp.getHandle();
                if (epoll_ctl(epoll_fd, 1, udp.getHandle(), &ev) < 0)
                {
                    printf("In %s : epoll_ctl error\n", "UDPThread::loop");
                }
                setUDPSocket(&udp);
                char buf[0x1000];
                while (!isTerminating())
                {
                    int n = epoll_wait(epoll_fd, &ev, max_events, 100);
                    if (n != 0)
                    {
                        if (G_ScriptData()->mFlag)
                        {
                            long long now = get_ms_tick();
                            long long diff = now - m_tick;
                            if (500 < diff)
                            {
                                getUDPSocket()->popMonitorAuthPacket();
                                m_tick = now;
                            }
                        }
                        int r = udp.recv(buf, 0x1000);
                        if (r < 0)
                        {
                            printf("[ERROR] UDP Thread Recv Error(%s)", strerror(*__errno_location()));
                        }
                        else if (r != 0)
                        {
                            m_handler->dispatch(buf, r, 0);
                        }
                    }
                }
                setTerminated();
            }
            else
            {
                puts("failed to set Option Non Blocking");
            }
        }
        else
        {
            printf("failed to bind UDP socket port #%d\n", m_port);
        }
    }
    else
    {
        puts("failed to open UDP socket port");
    }
}

void UDPThread::logError()
{
    int err = errno;
}


} // namespace RelayServiceApp
