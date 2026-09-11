// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 344（ORIG 316 / ours 372），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_Exit::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [b28e-L5] P214③/N78：ORIG 0x081ed948 函数头为 `movl $0x0,-0x44/-0x40/-0x3c/-0x38`
   // 4×dword 常量清零（16 字节数组）⇒ 原码是 `char x[0x10] = {0};`；逐字节 16×movb
    // 是早期 hack 写法，多 12 条指令且与 ORIG 形态不符。
    char local_48[16] = {0};
    // [b28e-L5] P167 真缺口补修：ORIG 0x081ed96c `mov 0xc(%ebp),%eax; lea 0xe0(%eax),%edx`
    // + 0x081ed987 `call _ZN8CNetworkILi4096ELi450000EE10GetPeerIP2EPci`（len 0x10）
    // —— 原实现漏调用；该缓冲尾段（0x081edd84/0x081eddbd）作为
    // DB_SecuServiceUpdateValidityTime::makeRequest 的第 4 实参（len 8 / 0x10）。
    user->m_network.GetPeerIP2(local_48, 0x10);
    unsigned char local_49 = 0;
    if (packet.get_byte(local_49) != true)
    {
        local_49 = 0;
    }
    if (user != 0)
    {
        user->SetLogInOutState(local_49);
    }
    // [b28d-solo-DisPatcherCore] P167 真 bug 修正：ORIG 0x081... 调
    // `_ZN8WongWork12CMCAPManager19IsSaveCleanPadPointEv`（非 const，WongWorkMisc_A.cpp
    // 定义），原写 `isSaveCleanPadPoint()`（CMCAPManager.h:29 旧 const 变体）为错符号。
    if (user->m_mcap.IsSaveCleanPadPoint())
    {
        // [G5-DisPatcherCore] P329/P0：ORIG 0x081ed9a6 段实参右→左求值——
        // `call get_acc_id`（→%ebx 保活）先于 `call CMCAPManager::getPoint`（→%eax），
        // 即第二实参先算、第一实参后算；具名局部把 getPoint 提前到语句边界 ⇒ 序反 + 多 -0x5c 槽。
        DB_SaveCleanpadPoint::makeRequest(user->m_mcap.getPoint(),
                                          static_cast<int>(user->get_acc_id()));
        user->m_mcap.disableCleanPadPoint();
    }
    if (user->get_state() > 2)
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel() != false)
        {
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                CExpandEquipslot* local_20 = user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
                // [G5-DisPatcherCore] P0：ORIG 0x081edbxx 段 movsbl 直入 %ebx/%eax（无槽往返）
                // ⇒ 原码无具名 char 局部，实参右→左（先第 4 实参 getCurExpand → %ebx，
                // 后第 3 实参 getCurChannel → %eax）；具名局部会各占一字节槽并 reload。
                local_20->EquipslotSwitch(user, '\0',
                                          static_cast<char>(user->getCurChannelEquipslotSwitch()),
                                          static_cast<char>(user->getCurExpandEquipslotSwitch()));
            }
        }
    }
    {
        // [Lane38 2026-09-10] P0/P219：ORIG 0x081edc9x 段无具名 string 局部——
        // 字面量直接喂按值形参（ORIG 全函数仅 1 次 `_ZNSsC2EPKcRKSaIcE` 构造、
        // **无** `_ZNSsC1ERKSs`；P167 实测我方 EXTRA `_ZNSsC1ERKSs` +1、
        // `_ZNSsC1EPKcRKSaIcE`/`_ZNSaIcEC1Ev` 为 C2 变体别名）。原「具名局部 +
        // 按值传参」多出 1 次拷贝构造与整段临时副本码。
        // P183/P0：ORIG 0x081edbxx 为单语句链式——`call Singleton::Get` 结果直接作
        // findGameScript 的 receiver，返回值直接作 delUserRewardData 的 receiver，
        // 无 this_01/this_02 两个具名指针槽（我方多发 2 组 store/load 且帧 +8）。
        static_cast<ARAD::Arad_EventPeriodDataManager*>(
            ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                "Arad_EventPeriodDataManager"))
            ->delUserRewardData(user);
    }
    if (user->get_state() != 0)
    {
        user->WorkPerFiveMin();
        if (user->get_charac_memberkey() != 0)
        {
            int local_14 = user->get_member_pay_tex_money_to_upper();
            int local_10 = user->get_member_pay_tex_fatigue_to_upper();
            // [G5-DisPatcherCore] P0/P52：ORIG 仅 money/fatigue 有具名槽（-0x10/-0xc）；
            // characNo→%ebx、is_connect→%esi、GetServerGroup 结果直作 receiver 实参、
            // GetServerProxy 结果直作 this（全无槽）。我方原 4 个具名量（uVar5/bVar3/uVar6/
            // this_03）正是本件帧 +0x10 的来源；实参序仍按 cdecl 右→左（characNo 先于
            // is_connect），与 ORIG 0x081edbxx 段一致。
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
                ->SendMemberPayTax(user->is_connect_upper_member(),
                                   static_cast<unsigned int>(user->getCurCharacNo()),
                                   local_14, local_10);
            if (0 < local_14)
            {
                user->set_member_pay_tex_money_to_upper(0);
            }
            if (0 < local_10)
            {
                user->set_member_pay_tex_fatigue_to_upper(0);
            }
        }
        // [G5-DisPatcherCore] P227/P329：ORIG 0x081edcb0 段是 cMyTrace **全表达式临时链**——
        // `call get_acc_id` 结果直入 NumberToString（无 -0x18 槽），NumberToString 结果
        // 升格 %ebx 跨 ctor 保活；trace 之后才 `call get_acc_id; mov %eax,-0x18`（第二次取值）。
        // 我方原「具名 local_1c/local_18/uVar6 + 具名 cMyTrace」多 3 个槽并改变求值序。
        unsigned int local_1c;    // ORIG -0x18（第二次 get_acc_id 落此）
        unsigned int local_18;    // ORIG -0x14（GetUID）
        cMyTrace(__PRETTY_FUNCTION__, 0x706d, 0)(
            "[USER LOGOUT (Normal)] DB ID : %s\n", NumberToString(user->get_acc_id(), 0));
        // 原为手写虚调（裸 vptr 解引用，违 §4.3）；ORIG 0x081edc7x 的
        // `call GetInstancePrivateStoreMgr; mov (%eax),%edx; mov (%edx),%ecx; call *%ecx`
        // 即 IUserPos 槽 0 = CPrivateStoreMgr::Exit（头文件 CPrivateStoreMgr.h:357 已声明）
        // 的真实虚成员调用，且无 puVar7 具名指针槽。
        private_store::GetInstancePrivateStoreMgr()->Exit(user);
        pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(user, true);
        GlobalData::s_ExpertJobMgr->OnLeaveUser(user);
        GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);
        user->checkLogOutCorrectly();
        // ORIG 0x081edcd6 `call _ZNK5CUser10get_acc_idEv` → -0x18(%ebp)，紧接
        // `call GetUID` → -0x14（0x081edd68/0x081edda1 读 -0x18/-0x14）——此处的取值
        // 才是具名槽的来源；上一条 trace 处那次 get_acc_id 无槽（已内联）。
        local_1c = user->get_acc_id();
        local_18 = user->GetUID();
        // P0/P329：ORIG 该处 `call GetCharacPlayTick`→%ebx（保活）、`call get_charac_no`→%eax
        // 直传 makeRequest，无具名 lVar8/iVar4 槽。
        DB_UpdateCharacPlayTime::makeRequest(user->get_charac_no(-1),
                                            user->GetCharacPlayTick());
        G_CGameManager()->user_exit(user);
        if (local_49 == '\0')
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, true, local_18);
        }
        else
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, false, local_18);
            // P183：ORIG `call getSecurityCard; mov %eax,(%esp); call isCertified` 为单语句
            // 内联链（无 this_06/this_07 具名指针槽）。
            if (user->getSecurityCard()->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 8, local_48);
            }
            if (user->getPad()->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 0x10, local_48);
            }
        }
    }
    return 0;
}
