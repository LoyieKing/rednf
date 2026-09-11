Packet_PvPChannelInfo::Packet_PvPChannelInfo()
    : PacketHeader(0xfa2, 0x17f8)   // ORIG C1 @0x0822bd3c：0x4(%esp)=0xfa2(packetId)、
                                    //   0x8(%esp)=0x17f8(packetSize)；原参数序写反
{
}