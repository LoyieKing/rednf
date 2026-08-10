//
// Created by loyieking on 24-3-23.
//

#include "PacketDispatcher.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"

CPacketDispatcher::CPacketDispatcher() {}

DispatcherFunction CPacketDispatcher::get_dispatcher(ushort id) {
    std::map<unsigned short, DispatcherFunction>::iterator found = packetTable.find(id);
    if (found != packetTable.end()) {
        return found->second;
    }
    return NULL;
}
void CPacketDispatcher::register_packet(ushort id, DispatcherFunction dispatcher) {
    // 来自反编译/DWARF 的推断：原始代码使用 insert(make_pair(...))，而非 operator[]
    // insert 语义为：key已存在时不覆盖旧值（operator[] 会覆盖）
    packetTable.insert(std::make_pair(id, dispatcher));
}
