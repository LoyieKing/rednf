//
// Created by loyieking on 24-3-23.
//
#include "PacketHeader.h"

PacketHeader::PacketHeader(ushort packetId, ushort packetSize) : packetId(packetId), packetSize(packetSize), reversed1(0), reversed2(0), reversed3(0) {
}
