int Dispatcher_SetSlot::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140020] get_binary(&req, 0x24); return 0
    char* volatile pBuf = reinterpret_cast<char*>(&req);   // ORIG 局部槽 + volatile 回读交错
    packet.get_binary(pBuf, 0x24);
    return 0;
}