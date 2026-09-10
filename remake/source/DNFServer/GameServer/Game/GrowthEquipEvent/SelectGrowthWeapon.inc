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
