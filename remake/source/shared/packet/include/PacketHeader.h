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
 * 0x06, 0x07. 0x08, 0x09(uint): reversed2
 * ...: data content
 */
class PacketHeader {
public:
    const ushort packetId;
    const ushort packetSize;
    const ushort reversed1;
    const ushort reversed2;
    const ushort reversed3;

    PacketHeader(ushort packetId, ushort packetSize);
};

TEST_CLASS_SIZE(PacketHeader, 10)

#endif  // PacketHeader_H
