#ifndef STUN_H_
#define STUN_H_

struct Address
{
    unsigned short port;
    unsigned int ip;
};

struct MSG_BINDING_RESPONSE
{
    unsigned char type;
    unsigned int ip;
    unsigned short port;
} __attribute__((packed));

struct MSG_MTU_ECHO
{
    unsigned short recv_len;
};

bool msgProcess(int socket, char* recv, int len, Address& info);
void* stunProcess1(void* param);
void* stunProcess2(void* param);
void* stunProcess3(void* param);

#endif // STUN_H_
