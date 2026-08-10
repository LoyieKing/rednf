#include <iostream>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "udp.h"
#include "global_func.h"

Socket openPort(unsigned short port, unsigned int interfaceIp)
{
    Socket fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd == -1)
    {
        int err = getErrno();
        std::cerr << "Could not create a UDP socket:" << err << std::endl;
        return INVALID_SOCKET;
    }

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    // ORIG 对回环的判断常量是网络字节序 0x0100007f（原始工程 INADDR_LOOPBACK 值）
    if (interfaceIp != 0 && interfaceIp != 0x0100007f)
    {
        addr.sin_addr.s_addr = htonl(interfaceIp);
    }

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) != 0)
    {
        int e = getErrno();
        switch (e)
        {
        case 0:
            return -1;
        case EADDRINUSE:
            std::cerr << "Port " << port << " for receiving UDP is in use" << std::endl;
            return -1;
        case 99:
            return -1;
        default:
            std::cerr << "Could not bind UDP receive port. Error=" << e << " " << strerror(e) << std::endl;
            return -1;
        }
    }

    timeval tv_timeo;
    tv_timeo.tv_sec = 3;
    tv_timeo.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_timeo, sizeof(tv_timeo));

    if (fd == INVALID_SOCKET)
    {
        write_log("openPort() Fail, fd = INVALID_SOCKET", fd);
        assert(fd != INVALID_SOCKET);
    }

    std::clog << "Opened port " << port << " with fd " << fd << std::endl;
    return fd;
}

bool getMessage(Socket fd, char* buf, int* len, unsigned int* srcIp, unsigned short* srcPort)
{
    if (fd == INVALID_SOCKET)
    {
        write_log("getMessage() Fail, fd = INVALID_SOCKET", fd);
        assert(fd != INVALID_SOCKET);
    }

    sockaddr_in from;
    int fromLen = 16;
    int ret = recvfrom(fd, buf, *len, 0, (sockaddr*)&from, (socklen_t*)&fromLen);
    if (ret == -1)
    {
        int err = getErrno();
        switch (err)
        {
        case 0x58:
            std::cerr << "Error fd not a socket" << std::endl;
            break;
        case 0x68:
            std::cerr << "Error connection reset - host not reachable" << std::endl;
            break;
        case 0xb:
            break;
        default:
            std::cerr << "Socket Error=" << err << " " << strerror(err) << std::endl;
            break;
        }
        return false;
    }
    else if (ret < 1)
    {
        std::clog << "socket closed? " << "recv size" << ret << std::endl;
        return false;
    }
    else
    {
        *srcPort = ntohs(from.sin_port);
        *srcIp = ntohl(from.sin_addr.s_addr);
        buf[ret] = '\0';
        *len = ret;
        return true;
    }
}

bool sendMessage(Socket fd, char* buf, int l, unsigned int dstIp, unsigned short dstPort)
{
    if (fd == INVALID_SOCKET)
    {
        write_log("sendMessage() Fail, fd = INVALID_SOCKET", fd);
        assert(fd != INVALID_SOCKET);
    }

    int s;
    if (dstPort == 0)
    {
        return false;
    }
    else if (dstIp == 0)
    {
        return false;
    }
    else
    {
        sockaddr_in to;
        int toLen;

        toLen = 16;
        memset(&to, 0, toLen);
        to.sin_family = AF_INET;
        to.sin_port = htons(dstPort);
        to.sin_addr.s_addr = htonl(dstIp);

        s = sendto(fd, buf, l, 0, (sockaddr*)&to, toLen);
        if (s == -1)
        {
            int e = getErrno();
            switch (e)
            {
            case 0x61:
                std::cerr << "err EAFNOSUPPORT in send" << std::endl;
                break;
            case 0x6f:
            case 0x70:
            case 0x71:
                break;
            default:
                std::cerr << "err " << e << " " << strerror(e) << " in send" << std::endl;
                break;
            }
            return false;
        }
        else if (s == 0)
        {
            std::cerr << "no data sent in send" << std::endl;
            return false;
        }
        else
        {
            if (s != l)
            {
                std::cerr << "only " << s << " out of " << l << " bytes sent" << std::endl;
                return false;
            }
            return true;
        }
    }
}

void initNetwork()
{
}
