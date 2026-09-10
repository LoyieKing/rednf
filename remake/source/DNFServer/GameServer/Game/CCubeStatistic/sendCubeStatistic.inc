int CCubeStatistic::sendCubeStatistic(int itemIndex, int count, CUser* user,
                                      CUBE_STATISTIC_FIELD field)
{
    // ORIG：Packet_Cube_Statistic(channel_no, get_charac_level, itemIndex,
    //   count, field) 通过 CStatisticServerProxy::SendPacket(pkt, 0x17=23) 上报。
    // 与 CValueStatistic::InsertValueStatistic 同型（GetServerProxy→SendPacket）。
    Packet_Cube_Statistic pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_level = (unsigned short)user->get_charac_level();
    pkt.m_itemIndex = (unsigned int)itemIndex;
    pkt.m_itemCount = count;
    pkt.m_type = (unsigned char)field;

    // ORIG 0x861002e 区：GetServerProxy 返回直传 0x4(%esp) 无判空无槽
    // （P140/P52 链式单语句；&pkt 跨 GetServerProxy 调用缓存 %ebx）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket((char*)&pkt, 0x17);
    return 1;
