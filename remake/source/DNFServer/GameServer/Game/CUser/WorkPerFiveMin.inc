// ORIG _ZN5CUser14WorkPerFiveMinEv T 0x08652f0c：五/十分钟公会上报族
// （一般经验与今日经验两段同构）；无角色时 trace 返回。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08652f0c | _ZN5CUser14WorkPerFiveMinEv
void CUser::WorkPerFiveMin()
{
    // 2026-09-10（G3CUser-B 复验）：槽集对照 ORIG 13 槽 / 我方 13 槽、**零平移零多余**
    //   （insn 176:176）⇒ 具名局部已与 ORIG 一一对应，残余 ndiff 纯属跨版本寄存器/
    //   指令选择形（P227 链式 trace 已落地），判 N 封顶，不动。
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        // ORIG 0x08652f0c 段（P227 全表达式临时链）：NumberToString 结果先升格 %ebx
        //   跨 cMyTrace ctor 保活（`mov %eax,%ebx` 在 ctor 实参装载前排），
        //   两语句具名形会把 ctor 提到求值之前 ⇒ 必须临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x1d24, 5)(
            "CUser::WorkPerFiveMin() m_id(%s) character - m_selected is null",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_guild_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_exp();
        // 2026-09-11 L9GrpF（P211/P324 store 序）：ORIG `movb $0x0,-0x2d` 在
        //   `movzbl 0x796d9(%eax),%eax` **之前** ⇒ 常量 store 语句在字段读之前。
        packet.m_expType = 0;
        packet.m_notify = m_field796d9;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_exp();
    }
    if (get_guild_today_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_today_exp();
        // 同 #3a（ORIG 第二段 `movb $0x2,-0x49` 亦在字段读之前）。
        packet.m_expType = 2;
        packet.m_notify = m_field796d9;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_today_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_today_exp();
    }
}