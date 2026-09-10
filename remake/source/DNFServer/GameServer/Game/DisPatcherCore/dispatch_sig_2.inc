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
        unsigned int uVar5 = user->m_mcap.getPoint();
        DB_SaveCleanpadPoint::makeRequest(uVar5, static_cast<int>(user->get_acc_id()));
        user->m_mcap.disableCleanPadPoint();
    }
    if (user->get_state() > 2)
    {
        if (G_GameWorld()->IsEquipSlotSwitchChannel() != false)
        {
            if (user->getCurChannelEquipslotSwitch() == 1)
            {
                CExpandEquipslot* local_20 = user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
                char cVar1 = static_cast<char>(user->getCurExpandEquipslotSwitch());
                char cVar2 = static_cast<char>(user->getCurChannelEquipslotSwitch());
                local_20->EquipslotSwitch(user, '\0', cVar2, cVar1);
            }
        }
    }
    {
        // [Lane38 2026-09-10] P0/P219：ORIG 0x081edc9x 段无具名 string 局部——
        // 字面量直接喂按值形参（ORIG 全函数仅 1 次 `_ZNSsC2EPKcRKSaIcE` 构造、
        // **无** `_ZNSsC1ERKSs`；P167 实测我方 EXTRA `_ZNSsC1ERKSs` +1、
        // `_ZNSsC1EPKcRKSaIcE`/`_ZNSaIcEC1Ev` 为 C2 变体别名）。原「具名局部 +
        // 按值传参」多出 1 次拷贝构造与整段临时副本码。
        ARAD::Arad_DataManager* this_01 = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* this_02 =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(
                this_01->findGameScript("Arad_EventPeriodDataManager"));
        this_02->delUserRewardData(user);
    }
    if (user->get_state() != 0)
    {
        user->WorkPerFiveMin();
        if (user->get_charac_memberkey() != 0)
        {
            int local_14 = user->get_member_pay_tex_money_to_upper();
            int local_10 = user->get_member_pay_tex_fatigue_to_upper();
            unsigned int uVar5 = static_cast<unsigned int>(user->getCurCharacNo());
            bool bVar3 = user->is_connect_upper_member();
            ENUM_SERVER_GROUP uVar6 = user->GetServerGroup();
            CMonitorServerProxy* this_03 =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(uVar6);
            this_03->SendMemberPayTax(bVar3, uVar5, local_14, local_10);
            if (0 < local_14)
            {
                user->set_member_pay_tex_money_to_upper(0);
            }
            if (0 < local_10)
            {
                user->set_member_pay_tex_fatigue_to_upper(0);
            }
        }
        unsigned int local_1c = user->get_acc_id();
        unsigned int local_18 = user->GetUID();
        char* uVar6 = NumberToString(local_1c, 0);
        cMyTrace local_30(__PRETTY_FUNCTION__,
                          0x706d, 0);
        local_30("[USER LOGOUT (Normal)] DB ID : %s\n", uVar6);
        private_store::CPrivateStoreMgr* puVar7 = private_store::GetInstancePrivateStoreMgr();
        (*reinterpret_cast<void (**)(private_store::CPrivateStoreMgr*, CUser*)>(
            *reinterpret_cast<void**>(puVar7)))(puVar7, user);
        pvp_assault::CAssaultMgr* this_04 = pvp_assault::GetInstanceAssaultMgr();
        this_04->OnLeaveAssaultPlace(user, true);
        GlobalData::s_ExpertJobMgr->OnLeaveUser(user);
        GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(user);
        user->checkLogOutCorrectly();
        // ORIG 0x081edcd6 `call _ZNK5CUser10get_acc_idEv` → -0x18(%ebp)：user_exit 之后
        // **重新取** acc_id 供 DB_ArrangeUserLoginInfo / DB_SecuServiceUpdateValidityTime
        // （0x081edd68/0x081edda1 读 -0x18），不复用 trace 处那次。我方原先只 2 次
        // （ORIG 3 次）⇒ P183 局部缓存族。getter 无副作用，重复取值行为完全等价。
        local_1c = user->get_acc_id();
        long lVar8 = user->GetCharacPlayTick();
        int iVar4 = user->get_charac_no(-1);
        DB_UpdateCharacPlayTime::makeRequest(iVar4, lVar8);
        CGameManager* this_05 = G_CGameManager();
        this_05->user_exit(user);
        if (local_49 == '\0')
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, true, local_18);
        }
        else
        {
            DB_ArrangeUserLoginInfo::makeRequest(local_1c, false, local_18);
            WongWork::CSecurityCard* this_06 = user->getSecurityCard();
            if (this_06->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 8, local_48);
            }
            Sanicova::CPad* this_07 = user->getPad();
            if (this_07->isCertified() != false)
            {
                DB_SecuServiceUpdateValidityTime::makeRequest(local_1c, 0xb4, 0x10, local_48);
            }
        }
    }
    return 0;
