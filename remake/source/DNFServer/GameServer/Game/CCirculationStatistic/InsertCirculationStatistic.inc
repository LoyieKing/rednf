void CCirculationStatistic::InsertCirculationStatistic()
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0860dccc 槽组整体平移 4
    // （-0xf4/-0x1c vs -0xf8/-0x20）+ 一段顺序差（N82/N98 族）；语义等价。
    for (std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
             m_data.begin(), endIt = m_data.end();
         it != endIt; ++it)
    {
        const ENUM_SERVER_GROUP group = it->first;
        stCirculationStatistic& stat = it->second;
        for (int level = 1; level <= 0x46; ++level)
        {
            bool hasValue = false;
            for (int field = 0; field <= 0x2f; ++field)
            {
                if (stat.m_values[level][field] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            if (!hasValue)
                continue;

            Packet_Circulation_Statistic packet;
            packet.fields.m_channel =
                static_cast<unsigned short>(G_CEnvironment()->get_channel_no());
            packet.fields.m_level = level;
            memcpy(packet.m_data, stat.m_values[level],
                   0x30 * sizeof(int));
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
            proxy->SendPacket(reinterpret_cast<char*>(&packet), 0xd0);
        }
        stat.reset();
    }
