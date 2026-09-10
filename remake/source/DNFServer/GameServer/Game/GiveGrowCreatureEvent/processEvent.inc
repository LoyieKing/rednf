int GiveGrowCreatureEvent::processEvent(CUser& user)    // 0x081b6f76
{
    // ORIG：链式直调（无具名中间局部，P0 右值内联）；G_CDataManager() 每用
    // 点重取（×4，P48 无 CSE）；sigInfo/info 两段各自重取 serverGroup/no/job。
    int creatureItemId = user.getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItemId();

    int infinityIdx = G_CDataManager()->getInfinityChangeCreatureIndex(creatureItemId);
    // ORIG 0x081b6fb9：第二查表实参取 infinityIdx（-0x18），非 creatureItemId（真语义 bug）。
    int clientIdx = G_CDataManager()->getClientChangeCreatureIndex(infinityIdx);
    if (infinityIdx == -1 || clientIdx == -1)
        return 0x16;

    CItem* item = G_CDataManager()->find_item(infinityIdx);
    if (item == 0)
        return 0x16;

    int changeAbleLev = G_CDataManager()->getChangeAbleCreatureLev();
    if (item->GetItemType() == 0x16)
    {
        // 可成长生物：进化到客户端版本后继续走登记流程。
        if (!user.getCurCharacInvenW()->GetCreatureMgrW()->EventEvoluteCreature(
                infinityIdx, changeAbleLev, clientIdx))
            return 0x16;
    }

    // DB 登记当前角色（SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo 0x2c）。
    SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo sigInfo;
    sigInfo.m_serverGroup = static_cast<unsigned short>(user.GetServerGroupToPvP());
    sigInfo.m_characNo = user.getCurCharacNo();
    sigInfo.m_job = user.get_charac_job();
    ss_strcpy(sigInfo.m_name, user.getCurCharacName());
    DB_EventSaveGiveGrowCreature::makeRequest(user.get_acc_id(), sigInfo);

    // 内存登记（GrowthCreatureEvent::eventCharacInfo 同布局 0x2c）。
    GrowthCreatureEvent::eventCharacInfo info;
    info.m_serverGroup = static_cast<unsigned short>(user.GetServerGroupToPvP());
    info.m_characNo = user.getCurCharacNo();
    info.m_job = user.get_charac_job();
    ss_strcpy(info.m_name, user.getCurCharacName());

    // P0/P183：ORIG 无 mine 具名局部（getGrowthCreatureEventdata 结果直传
    // push_back 实参），具名指针多 2 条 mov 且全槽深 4。
    static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
        user.getGrowthCreatureEventdata())->push_back(info);
    return 0;
