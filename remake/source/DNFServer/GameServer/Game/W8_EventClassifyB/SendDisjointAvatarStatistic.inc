// SendDisjointAvatarStatistic（ORIG 0x0810f5f4）
// ORIG 用 if/else 赋值局部 m_gradeIdx（rarity==2 ? 2 : grade-1）——P47 按
// if-return/if-else 形态；GetServerProxy(s_statistic_proxy_mgr,0) 直链。
void CAvatarDisjointRandomReward::SendDisjointAvatarStatistic(
    CItem const* citem)
{
    // [b28c-L0 NEAR·N7] 补 gradeIdx=0 预清后仅剩虚调用寄存器差
    // （call *%edx vs *%eax），47=47。
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    int gradeIdx = 0;
    if (citem->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = citem->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = citem->GetItemType();
    packet.m_flagA = 1;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
}