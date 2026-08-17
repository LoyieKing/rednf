//
// Created by Loyie King on 2024/3/19.
//

#ifndef PACKET_COLLECTITEMS_H
#define PACKET_COLLECTITEMS_H

#include "PacketHeader.h"
#include <string.h>

class Packet_CollectItems : public PacketHeader {
public:
    unsigned int m_idByChannel;  // +0xa
    unsigned int m_accId;        // +0xe（process: CUser::get_acc_id）
    int m_itemCount;             // +0x12（process: (uint)*(ushort*)(msg+0x10) = delete_item 数量）
    unsigned int m_characNo;     // +0x16（process: CUser::get_charac_no(param_1,-1)）
    unsigned char m_nameLen;     // +0x1a
    char m_name[0x1e];           // +0x1b
    Packet_CollectItems() : PacketHeader(0x27e6, 0x39),
        m_accId(0), m_itemCount(0), m_characNo(0), m_nameLen(0)
    {
    memset(m_name, 0, sizeof(m_name));
    };
} __attribute__((packed));

TEST_CLASS_SIZE(Packet_CollectItems, 0x39);

#endif  // PACKET_COLLECTITEMS_H
