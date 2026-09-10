void Dispatcher_Client_Spec_Statistic::send(CUser* user, ParamBase& param)
{
    if (user == 0)
    {
        return;
    }
    char* hwSpec = user->getClientHWSpec();
    const unsigned char specType = *(unsigned char*)((char*)&param + 4);
    const bool special = (specType == 1 || specType == 0x81);
    if (hwSpec[0] != 0 && !special)
    {
        return;
    }
    // ORIG 栈帧对象 0xc06：PacketHeader(0x9ce,0xc06)+flag@0xa+count@0xb+HWSpec[254]@0x12。
    static const unsigned int kSpecStatSize = 0xc06;
    unsigned char stat[kSpecStatSize];
    new (stat) Packet_Client_Spec_Statistic();   // NEEDS_DEP: ORIG W 0x0822bc18
    const unsigned char count = (unsigned char)hwSpec[1];
    stat[0x0a] = special ? 2 : 0;
    stat[0x0b] = count;
    const unsigned short len = (unsigned short)(count * 0xc + 0x12);
    *(unsigned short*)(stat + 2) = len;
    memcpy(stat + 0x12, hwSpec + 4, (unsigned int)count * 0xc);
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    if (proxy != 0)
    {
        proxy->SendPacket(reinterpret_cast<char*>(stat), (int)len);
    }
