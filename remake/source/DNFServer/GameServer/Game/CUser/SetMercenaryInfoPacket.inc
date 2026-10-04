// ============================================================================
// ORIG T 0x0868e704 sz 0x2bb _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard：
//   管理等级/点数 + 雇佣兵列表逐个序列化（剩余时间、状态、名字、三状态字节）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868e704 | _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard
void CUser::SetMercenaryInfoPacket(PacketGuard& packet)
{
    // b28d-lane5 复测（判据⑩ 声明序 = 槽深序）：ORIG 0x0868e70b/0x0868e712 在 `sub $0x24`
    //   后立即两条 `movl $0x0,-0x18/-0x14`（循环变量置 0 **在函数头**）⇒ remainSec/nameLen
    //   须在函数头带初值声明（原形声明在循环内 ⇒ 少两槽、leveledUp 槽 -0xd 漂到 -0x15）。
#if !remake_verify
    cMyTrace(__PRETTY_FUNCTION__, 0x73a, 5)(
        "[DNF-DIAG MERC] SetMercenaryInfoPacket: m_characs.size()=%u manageLevel=%d managePoint=%d",
        (unsigned int)m_mercenaryMgr.m_characs.size(),
        (int)m_manageLevel, (int)m_managePoint);
#endif
    int remainSec = 0;
    int nameLen = 0;
    bool leveledUp = CalcurateManageLevel();
    if (leveledUp)
        SendManageLevelUpNoti();

    packet.put_byte(GetManageLevel());
    packet.put_int(GetManagePoint());
    packet.put_byte((signed char)m_mercenaryMgr.m_characs.size());

    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        // b28d-solo-CUser p167：ORIG 0x0868e704 每字段重调
        //   `vector<CHARAC_LOAD_MERCENARY>::operator[]`（共 9 次；我方缓存引用 merc
        //   仅 1 次，MISSING ×8）⇒ 去缓存逐字段访问（P157/P183）。
        // 剩余时间 = 到期秒 - 当前秒
        remainSec = m_mercenaryMgr.m_characs[i].m_field2c
                    - GlobalData::s_systemTime_.getCurSec();

        // 2026-09-10（P265 档③ 实测无效，已回退）：ORIG 0x0868e7b5 段该实参为
        //   `mov (%eax),%eax; mov %eax,%edx`（int 字段经异型转换的两段中转），
        //   实参位显式 `(unsigned int)` cast 实测**不产**该中转（仍单段 `mov (%eax),%edx`）
        //   ⇒ 该中转来自字段真型（enum）而非实参 cast；字段在共享头 SigTypes_C.h
        //   CHARAC_LOAD_MERCENARY::m_characNo（全仓多消费点）⇒ 按 S2 不自行改，
        //   记 N（P254 字段 enum 化旋钮待 Main 全链评估）。
        packet.put_int(m_mercenaryMgr.m_characs[i].m_characNo);

        if (&m_mercenaryMgr.m_characs[i].m_name[0] == 0)
            nameLen = 0;
        else
            nameLen = strlen(m_mercenaryMgr.m_characs[i].m_name);
        packet.put_int(nameLen);
        if (nameLen != 0)
            packet.put_str(m_mercenaryMgr.m_characs[i].m_name, nameLen);

        if (remainSec > 0)
            packet.put_byte(1);
        else if (m_mercenaryMgr.m_characs[i].m_field30 == 0xff)
            packet.put_byte(0);
        else
            packet.put_byte(2);

        packet.put_int(remainSec);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_field30);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_field31);
        packet.put_byte((signed char)m_mercenaryMgr.m_characs[i].m_avatarBonus);
    }
}