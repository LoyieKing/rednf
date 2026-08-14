//
// Created by Loyie King on 2024/3/19.
//

#ifndef PacketHeader_H
#define PacketHeader_H

#include <sys/types.h>

#include "ReverseEngineerLib.h"

/**
 * some information about each packet:
 * minimum packet length is 10
 * 0x00,0x01(ushort): packet type
 * 0x02,0x03(ushort): packet length
 * 0x4, 0x05(ushort): reversed1
 * 0x06, 0x07. 0x08, 0x09(uint): m_connNo（连接/套接字，网络层写入）
 * ...: data content
 */
class PacketHeader {
public:
    ushort packetId;
    ushort packetSize;
    ushort reversed1;
    unsigned int m_connNo;  // +0x06 连接号/套接字（原始一次 movl 清零）

    PacketHeader(ushort packetId, ushort packetSize);
} __attribute__((packed));

TEST_CLASS_SIZE(PacketHeader, 10)

#endif  // PacketHeader_H
