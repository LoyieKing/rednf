//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_WEB_NOTICE_SINGLE_H
#define PACKET_WEB_NOTICE_SINGLE_H

#include "PacketHeader.h"

class Packet_Web_Notice_Single : public PacketHeader {
public:
    Packet_Web_Notice_Single() : PacketHeader(0x9e0, 0x10a){};
    // ORIG 布局：0xa 头 + 0x100 载荷 = 0x10a（manager/dbmw 的
    // OnWebNoticeBroadcast 以 +0xb memset 0xff + strncpy len + SendToTcpServer 0x10a；
    // 缺载荷会越界写栈）。
    char data[0x100];
} __attribute__((packed));

#endif  // PACKET_WEB_NOTICE_SINGLE_H
