//
// Created by loyieking on 24-3-23.
//

#pragma once

#include <map>
#include <sys/types.h>

#include "ReverseEngineerLib.h"
#include "PacketProc.h"

class CPacketDispatcher {
    std::map<unsigned short, DispatcherFunction> packetTable;  // 原始：ushort 键
public:
    CPacketDispatcher();  // 原始：显示定义于 PacketDispatcher.cpp（TU 首函数）

    DispatcherFunction get_dispatcher(ushort id);
    void register_packet(ushort id, DispatcherFunction dispatcher);
};
