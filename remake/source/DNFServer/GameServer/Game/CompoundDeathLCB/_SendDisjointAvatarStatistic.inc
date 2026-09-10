void CCompoundAvatar::_SendDisjointAvatarStatistic(tagCompoundInfo& info)
{
    Packet_Avater_Disjoint_Statistic packet;

    const CItem* scriptA = info.m_scriptA;
    const CItem* scriptB = info.m_scriptB;

    packet.m_field0a = 2;                                      // -0x36/local_36
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333ef6/0x08333f52 两组分级**复用同一
    // -0xc 槽**（各以 `movl $0,-0xc` 重新初始化）⇒ 源码必须是同一个具名局部
    // 两次使用（P160 复用槽；两个独立局部会各占一槽，实测 +4 帧）。
    int gradeIdx = 0;
    if (scriptA->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptA->get_grade() - 1;
    }
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = CI_SubType(scriptA);
    packet.m_flagA = 0;
    gradeIdx = 0;
    if (scriptB->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptB->get_grade() - 1;
    }
    packet.m_gradeIdxB = gradeIdx;
    packet.m_subTypeB = CI_SubType(scriptA);                   // 【ORIG 怪癖】保留
    packet.m_flagB = 0;

    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333fa7：`lea -0x3c(%ebp),%ebx` 先把
    // &packet 升格 callee-saved，再求 receiver（GetServerProxy）⇒ 单语句链式
    // 表达式（P52）；具名 proxy 局部会多一个槽且 &packet 退化为实参处现算。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet),
                      static_cast<int>(sizeof(packet)));       // 0x28
