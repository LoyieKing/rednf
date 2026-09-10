void CDisjointer::OnStartMission(CUser* targetUser, CUser* requestUser)
{
    // b28：清 register asm 绑定（N96/P0 全域禁），ORIG 物化岛形为 4.1 CFG 产物，
    // 先按最短语义（&& 短路 + 直接 throw）复测。
    if (targetUser != requestUser &&
        requestUser->getCurCharacMoney() < m_machine.get_cost())
        throw static_cast<ENUM_ERROR>(0x15);

    if (!targetUser->CheckMoney(m_machine.get_cost()))   // ORIG CheckMoney 返 1 为足额
    {
        send_error(targetUser, 10);
        // ORIG vptr+0x4 虚槽 = slot1 OnCloseMission(机主, 0)
        OnCloseMission(m_machine.get_user(), ENUM_CLOSE_MISSION_TYPE_0);
        throw static_cast<ENUM_ERROR>(10);
    }

    if (!check_disjoint_endurance())
    {
        send_error(targetUser, 0xbd);
        throw static_cast<ENUM_ERROR>(0xbd);
    }
