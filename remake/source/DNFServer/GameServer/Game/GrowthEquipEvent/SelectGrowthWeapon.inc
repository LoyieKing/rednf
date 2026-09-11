// ============================================================================
// GrowthEquipEvent::SelectGrowthWeapon @ 0x080f73be
// 使用成长武器道具（Dispatcher_UseBoosterItem::_onBoosterItemUseResult_OnlyOne）：
// ORIG 守卫比较 CDataManager+0x4bc4 == itemId。该字段即 m_growthWeapon.m_giftBox
// （get_GiftGrowthWeaponBox 0x836561c 读同一偏移，CDataManager.h 布局 +0x4bac+0x18），
// 用具名字段替代裸偏移。
// ============================================================================
// [b23判词·N-PTR] P167 调用点差集 CLEAR（13/13）：ORIG 无 CSE，getGrowthWeaponEventdata()
// 调 3 次（已内联，不缓存 data）、serverGroup 走 [G_CEnvironment()+0x378] 直读成员（无
// get_server_group() call）。残余 gap+2 = ORIG 指针成员写用 `mov reg,DISP(base)` 直存，
// gcc 4.4 对 `*(T*)(void*+off)=v` 折成 `lea DISP(base),r; mov reg,(r)`（#2/#3 各 +1），
// 跨版本指针写形不可源码复现，语义等价。
void GrowthEquipEvent::SelectGrowthWeapon(CUser& user, int itemId, int addInfo)
{
    // ORIG +0x1b sete：仅本活动道具（== itemId）进入主体。
    if (G_CDataManager()->m_growthWeapon.m_giftBox == itemId)
    {
        // [b28c-L18] P211 注（pad 解引用⇒LHS 地址跨 call 缓存反例）：原裸偏移
        // `*(int*)(cast+0x04)=call()` 使 4.4 恒发 `lea 0x4(base),%ebx` 地址槽化；
        // 改 GrowthWeaponEvent 具名同型字段直存后复现 ORIG `mov %eax,%ebx` +
        // `mov %eax,0x4(%ebx)`（ORIG 0x080f70xx 段实证 base 指针入 ebx、disp 折叠 store）。
        // P48：ORIG 无 CSE，getGrowthWeaponEventdata() 调 3 次，每次结果寄存器直传。
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field4 =
            user.get_charac_no(-1);
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field8 = addInfo;

        // 渠道组：整合 PvP 基础渠道用用户 server group，否则直读环境
        // m_serverEnvir.m_serverGroup（ORIG [G_CEnvironment()+0x378]，无 get_server_group() call）。
        // ORIG 先置局部 0（mov [ebp-0xc],0），再 if/else 覆盖。
        int serverGroup = 0;
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            serverGroup = (int)user.GetServerGroup();
        else
            serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field12 =
            (short)serverGroup;

        DB_EventUpdateGrowthWeaponCharac::makeRequest(
            user.get_acc_id(), (unsigned int)user.get_charac_no(-1),
            addInfo, (short)serverGroup);

        SendGrowthWeaponEventInfo(user);
    }
}