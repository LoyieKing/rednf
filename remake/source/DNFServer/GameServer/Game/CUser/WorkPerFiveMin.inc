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
        cMyTrace("CUser::WorkPerFiveMin()", 0x1d24, 5)(
            "WorkPerFiveMin() m_id(%s) character - m_selected is null..",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_guild_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_exp();
        packet.m_notify = m_field796d9;
        packet.m_expType = 0;
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
        packet.m_notify = m_field796d9;
        packet.m_expType = 2;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_today_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_today_exp();
    }
