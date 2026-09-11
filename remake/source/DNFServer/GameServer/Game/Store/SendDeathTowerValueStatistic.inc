// [b28d-solo-Store] 真语义 bug 修复（ORIG 0x0860f342 实证）：field != 10 时 ORIG 不早退，
// 而是 packet.m_counterIdx = field 后照常发送并 return true（+0xcc 块 `mov 0xc(%ebp),%eax;
// mov %eax,-0x10(%ebp)` 直落共享尾 +0xd2）。旧实现 `return false` 属漏发包。
// P0：ORIG 无独立 f 局部槽（m_counterIdx 即 -0x10 槽，3/4/5/6/field 直写成员）。
bool CDeathTowerValueStatistic::SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field,
                                                              char a, char b, unsigned int c)
{
    Packet_DeathTower_Statistic_Value packet;
    packet.m_type = a;
    packet.m_level = (short)b;

    if (field == (DEATHTOWER_VALUE_STATISTIC_FIELD)10)
    {
        // ORIG 0x0860f37a：`mov 0x18(%ebp),%eax; cmp $1; je; cmp $1; jb; cmp $2;
        // je; cmp $3; je; jmp default` 无栈槽线性决策树 + 叶块按 case 升序
        // ⇒ 源形是 switch(c)（P251/P46），不是 if-else 链。
        switch (c)
        {
        case 0:
            packet.m_counterIdx = 3;
            c = 1;
            break;
        case 1:
            packet.m_counterIdx = 4;
            c = 1;
            break;
        case 2:
            packet.m_counterIdx = 5;
            c = 1;
            break;
        case 3:
            packet.m_counterIdx = 6;
            c = 1;
            break;
        default:
            // ORIG 0x0860f3db：logFormat(1, "Statistics.cpp", __PRETTY_FUNCTION__,
            // 0x5d5, "name(%d), 존재하지 않는 아이템 등급", field)
            LogManager::logFormat(
                1, "Statistics.cpp", __PRETTY_FUNCTION__, 0x5d5,
                "name(%d), \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb1\xde", field);
            return false;
        }
    }
    else
    {
        packet.m_counterIdx = (unsigned int)field;
    }

    packet.m_value = c;

    // ORIG 0x0860f41a：`lea -0x1d(%ebp),%ebx` 先物化 &packet 并跨 GetServerProxy 调用保活
    // （P52 链式 receiver+R2L 实参 callee-saved 独占形），故必须写成单条链式表达式；
    // 具名 proxy 局部会把它降级为栈槽中转（mov %eax,-0xc 回读）。无条件发送、返回 true。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&packet, 0x15);
    return true;
}