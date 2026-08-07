#include <iostream>
#include <stdio.h>
#include <string.h>

#include "stun.h"
#include "udp.h"

extern int socketRealIp_;
extern int socketNat_;
extern int socketPortResctric_;

void* stunProcess1(void* param)
{
    char recv_buf[8];
    int recv_byte;
    Address source;

    printf("Create Thread1 RealIP Socket: %d\n", socketRealIp_);

    while (true)
    {
        recv_byte = 8;
        memset(&source, 0, sizeof(source));
        memset(recv_buf, 0, sizeof(recv_buf));

        if (getMessage(socketRealIp_, recv_buf, &recv_byte, &source.ip, &source.port))
        {
            msgProcess(socketRealIp_, recv_buf, recv_byte, source);
        }
    }
}

void* stunProcess2(void* param)
{
    char recv_buf[8];
    int recv_byte;
    Address source;

    printf("Create Thread2 NAT Socket: %d\n", socketNat_);

    while (true)
    {
        recv_byte = 8;
        memset(&source, 0, sizeof(source));
        memset(recv_buf, 0, sizeof(recv_buf));

        if (getMessage(socketNat_, recv_buf, &recv_byte, &source.ip, &source.port))
        {
            msgProcess(socketNat_, recv_buf, recv_byte, source);
        }
    }
}

void* stunProcess3(void* param)
{
    char recv_buf[1500];
    int recv_byte;
    Address source;

    printf("Create Thread3 MTU_CHECKER Socket: %d\n", socketPortResctric_);

    while (true)
    {
        recv_byte = 1500;
        memset(&source, 0, sizeof(source));
        memset(recv_buf, 0, 1500);

        if (getMessage(socketPortResctric_, recv_buf, &recv_byte, &source.ip, &source.port))
        {
            MSG_MTU_ECHO pck;
            pck.recv_len = recv_byte;
            sendMessage(socketPortResctric_, (char*)&pck, 2, source.ip, source.port);
        }
    }
}

bool msgProcess(int socket, char* recv, int len, Address& info)
{
    switch (recv[0])
    {
    case 1:
        {
            MSG_BINDING_RESPONSE pk_send;
            pk_send.type = 2;
            pk_send.ip = info.ip;
            pk_send.port = info.port;
            if (!sendMessage(socket, (char*)&pk_send, 7, info.ip, info.port))
            {
                return false;
            }
        }
        break;
    case 5:
        {
            MSG_BINDING_RESPONSE pk_send;
            pk_send.type = 2;
            pk_send.ip = info.ip;
            pk_send.port = info.port;
            if (!sendMessage(socketPortResctric_, (char*)&pk_send, 7, info.ip, info.port))
            {
                return false;
            }
        }
        break;
    case 0:
        break;
    default:
        return false;
    }
    return true;
}
