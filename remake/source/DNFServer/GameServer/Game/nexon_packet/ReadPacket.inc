void stLoginPacket::ReadPacket(const char* packet, stLogin& out)
{
    (void)out;  // ORIG 未使用该参数（解码目标为 packet 缓冲）

    CPacketHelper helper(m_body, MaxBodySize, 0);

    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)), 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 1, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 2,
                static_cast<int>(static_cast<unsigned char>(packet[1])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x101, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x102,
                static_cast<int>(static_cast<unsigned char>(packet[0x101])));
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x201, 4);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x205, 1);
    helper.scan(reinterpret_cast<unsigned char*>(const_cast<char*>(packet)) + 0x206, 8);
