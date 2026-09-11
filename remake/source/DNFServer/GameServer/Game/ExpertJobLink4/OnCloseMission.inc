// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x085d2b74 sz 0x17e）：
// 广播 0xd9 下线包 → 注销村庄对象 → 清专家职业 → 归还池。
void CDisjointer::OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type)
{
    (void)type;
    PacketGuard packet;
    packet.put_header(0, 0xd9);
    packet.put_short(static_cast<int>(user->get_unique_id() & 0xffff));
    packet.finalize(true);

    // b28d-61 [P200 短整直传实参] + [b28d-solo-ExpertJobLink4 P23/P134]：
    // ORIG 0x085d2be0 序 = get_area→%ebx（无 area 具名槽）→ getCurCharacVill
    // →movsbl %al,%esi→G_GameWorld()；具名 area 局部会把值落槽并改装载序。
    G_GameWorld()->send_to_area((int)user->getCurCharacVill(),
                                user->get_area(false), packet);

    // [b28d-solo-ExpertJobLink4 P212/N93]：ORIG Zone 槽 -0xa（浅于 PacketGuard
    // -0x18）+ &m_machine 提前入 %esi；具名 Zone 局部在 4.4 下占更深槽（-0x16）。
    // 改匿名临时实参（Zone 无默认 ctor，不可头部声明后赋值）。
    if (!GlobalData::s_villageObjectMgr->unregister_object(
            village_object::Zone(user->getCurCharacVill(),
                                 (unsigned char)user->get_area(false)),
            &m_machine))
    {
        LogManager::logFormat(
            1, "ProfessionDisjointer.cpp",
            // P139：ORIG 0x085d2c7f 传 _ZZ…E19__PRETTY_FUNCTION__ 局部静态，
            // 手写同内容字符串只产匿名 .rodata（符号差集残留）。
            __PRETTY_FUNCTION__,
            0x15b, "unregister_object error");
    }

    user->SetCurCharacExpertJob(0);
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}