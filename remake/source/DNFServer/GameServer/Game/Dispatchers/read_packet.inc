int Dispatcher_Pause::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140b04] get_binary(&req, 0xe); return 0
    char* volatile pBuf = reinterpret_cast<char*>(&req);  // ORIG 局部槽 -0xc；volatile 复现
    packet.get_binary(pBuf, 0xe);                           // 「先 load 回读、后存参」交错（实验实证）
    return 0;
}