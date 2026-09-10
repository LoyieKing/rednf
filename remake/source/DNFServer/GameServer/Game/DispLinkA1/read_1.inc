int Dispatcher_Client_Spec_Statistic::read(PacketBuf& packet, MSG_BASE& msg)
{
    unsigned char* const p = reinterpret_cast<unsigned char*>(&msg);
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xd));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xe));
    packet.get_byte(*reinterpret_cast<unsigned char*>(p + 0xf));
    for (int i = 0; i < (int)reinterpret_cast<unsigned char (&)[0x10]>(*p)[0xf]; ++i) {  // ORIG movzbl 0xf(%eax)
        packet.get_byte(*reinterpret_cast<unsigned char*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x12));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x16));
        packet.get_int(*reinterpret_cast<unsigned int*>(
            reinterpret_cast<unsigned long>(p + i * 0xc) + 0x1a));
    }
    return 0;
