// [b28d-solo-W8LT_EventTail] AE | ORIG 0x081b6f76 | P167 CLEAR（30/30）。
// 修法①（真语义 bug）：ORIG 0x081b6fb9 第二查表实参为 infinityIdx（-0x18），
// 原码误传 creatureItemId ⇒ 已改正。修法②（P0/P183）：删 mine 具名指针局部
// （结果直传 push_back）⇒ 全槽回移 4、135=135 逐条全等。
// [M1S5 2026-09-11 语义轴复核] ORIG 字段 disp 全等：m_minLevel(+4)/m_maxLevel(+8)
// < / >（0x081b6c5e 双 cmp+cc 落点体）、(*kt).m_job(+8)==jt->first(+0)、
// (*kt).m_name(+0xc) strlen/put_str(Pci,0xc(%esp)=len)；§6.5 串盲区无字面量；
// P216 不适用（EH 尾块 `mov %edx,%ebx; mov %eax,%esi; ... _Unwind_Resume` 两侧同形）。
// 本波无新落地，维持 N（our 214 vs ORIG 215：差 1 为 4.4 对齐 nop 位置）。
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
}