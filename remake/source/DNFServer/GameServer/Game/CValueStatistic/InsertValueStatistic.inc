int CValueStatistic::InsertValueStatistic()
{
    // [b28d-59] AE（ORIG 0x0860d8a4）：修掉 group 局部缺失（恢复具名 group 槽 -0x1c）、
    // 删 proxy 判空（P171）、条件改 `if (hasValue == false) continue;`（P168a② 取反形）
    // 后 105/105 逐条一致。
    for (std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        // [b28d-59] ORIG 0x0860d8d3/d8e6：group 槽 -0x1c、stat 引用槽 -0x18
        // （均具名局部，P0 槽往返实证）。
        ENUM_SERVER_GROUP group = it->first;
        stValueStatistic& stat = it->second;
        for (int level = 1; level < 0x47; ++level)
        {
            bool hasValue = false;
            for (int f = 0; f < 0x1e; ++f)
            {
                if (stat.m_values[level][f] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            // [b28d-59] ORIG 0x0860d933 `movzbl hasValue; xor $1; test; jne` ⇒
            // 原码取反形（P168a②直分支形）。
            if (hasValue == false)
            {
                continue;
            }
            {
                // ORIG 0x0860d8a4：ctor（PacketHeader(0x17ca,0x86)+m_level=0）构造，
                // 随后 m_level=level 与 memcpy m_values；不再整体 memset。
                Packet_Value_Statistic packet;
                packet.m_level = level;
                memcpy(packet.m_values, stat.m_values[level], 0x78);
                // [b28d-59] ORIG 0x0860d999..d9ad 无 proxy 判空（P171：原码不写即删；
                // GetServerProxy 返回值经 %eax 直传 SendPacket），旧判空多 1 槽 +3 条。
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group)->SendPacket(
                    (char*)&packet, 0x86);
            }
        }
        stat.reset();
    }
    return 1;
