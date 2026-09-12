// ===========================================================================
// OnRequestAssault —— ORIG T 0x082edfba (0x11bb)：袭击请求全量校验 + 创建场次。
// ===========================================================================
// [b28c-L7][SIGFIX:pvp_assault::CAssaultMgr::OnRequestAssault] int→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...(CUser*, CUser*, int*)"，tsv 第 14 行；
// mangled 不含返回类型⇒符号不变；体内 return 值仅 0/1）。
// [b28d-solo-W6_AssaultMgrDeps] 封顶 N82/N86（1156/1097，gap +59 = 4.4 帧/临时槽形态差）：
// §6.2 三件套①p167 CLEAR 202/202（0 missing/0 extra）；②逐块对齐 Ghidra C 并修
// 2 个真语义 bug（0x082ee06e attacker 的 AssaultedUserId、0x082ef0ce 错误包
// 漏 put_byte(4)）；③帧 sub $0x120 vs ORIG $0xdc + 缺 push %edi（4.4 临时槽更多）。
// [S2W6Deps 收敛记录] 1156 → **1108**（gap −59 → **−11**，共 −48 条），逐项（ORIG 地址）：
//   ① 082ee7be..082ee82c 曼哈顿距离：删 dx/distX/absDx/dy/distY/absDy 6 具名 + bool
//      tooFar 槽（-0x64/-0x60/-0x5c/-0x38/-0x34/-0x30/-0x55）→ 单条短路或 +
//      __builtin_abs（P0/P329，−20）；
//   ② 082ee88d..082ee8b5 vill/area：tooFar 三元嵌套 → 短路或（P324，−8）；
//   ③ 082ee61b..082ee64a blackCount：去具名槽 + signed→unsigned 比较（P0+类型轴；
//      ORIG `ja` 无符号，旧形 signed `jg` 在负值域运行行为不同 = 真语义修，−4）；
//   ④ 082ee1fb 删 s_villageMonsterMgr 多余 `!= 0` 判空（发明防御，−4）；
//   ⑤ 082eecb0/082eecbc authen/placeId 具名 → 实参内联（−4）；
//   ⑥ 082eeeb1 joinCount 具名 → 实参内联（−2）；⑦ 082eeec2 invenW → 直调（−2）；
//   ⑧ 082ee19a/082ee1a1/082ee1dd preventPeer 段：`!PowerWarOn()+else-if` → 真块
//      先写 + `if (flag) 0xfd else 0xfb`（P324，−6）；
//   ⑨ 082ee32b/082ee33c/082ee343 PowerWar 大块：then/else 交换 + ORIG `sete` 形
//      `if (vill == 0x7) {...} else throw 0xfc`（P324，−10）；
//   ⑩ 082ee971..082ee9b5 store 两守卫：`&&` → 嵌套 if（消 4.4 整体 bool 物化，−4）。
// 余 **11 条 = N 封顶**：push/pop %edi（ORIG 082ee369 用 %edi 保活 vill 跨
//   G_GameWorld() call，我方 %esi ⇒ N7 寄存器角色）+ 帧 0xe0 vs 0xdc（槽基址整体
//   平移 0x3c，N82/N95）+ `test %eax,%eax; setne %al` ×2（= GetAssaultPrevent
//   头签名 int，ORIG 8 位 `test %al,%al`，已报父泳道按 .h 项裁决）+ ORIG 独有的
//   movl $0 预清/物化段（我方缺，属 gcc 中端差）。
// ORIG 0x082edfba。
bool CAssaultMgr::OnRequestAssault(CUser* attacker, CUser* target, int* result)
{
    CAssaultPlace* acquiredPlace = 0;                   // -0x94(%ebp)
    unsigned int nextPlaceId = 0;                       // -0x98(%ebp)
    int chargeMoney = 0;                                // -0x9c(%ebp)

    try
    {
        if (attacker->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0xea);
        }
        if (target->isCompetitionMercenary() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x15);
        }

        if (!PowerWarOn())
        {
            // ORIG 082ee06e/082ee07b：attacker 的 AssaultedUserId vs target 的 unique_id
            // （旧实现两侧都取 target，真语义 bug）。
            if (attacker->GetCurCharacAssaultedUserId()
                != (static_cast<unsigned short>(target->get_unique_id())))
            {
                LogManager::logFormat(
                    1, "Assault.cpp",
                    __PRETTY_FUNCTION__,
                    0x1291,
                    "CAssaultMgr::OnRequestAssault ERROR - pAssaultedUser Unique ID is invalid");
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        bool preventPeerFlag = false;
        if (expert_job::CAlchemist::GetAssaultPrevent(attacker, &preventPeerFlag))
        {
            // ORIG 082ee19f..082ee1f6 / 082ee2xx 同形：`movzbl -0x9d; test; je 0xfb段`
            // 紧接 `throw 0xfd` ⇒ 原码书写序 = `if (preventPeerFlag) throw 0xfd;
            // throw 0xfb;`（P324 叶块书写序；我方旧序为 0xfb 先，两段各多 1 条 jmp）。
            // ORIG 082ee198 `test %al,%al; je 082ee198`（PowerWarOn()==0 跳到 else）
            // + then 块 `cmp $0x7,%al; setne; test; je 082ee1fb`（vill==7 跳过）
            // ⇒ 原码 = `if (PowerWarOn()) { if (vill != 0x7) {flag} } else {flag}`
            //（P324：真分支先写；我方旧形 !PowerWarOn+else-if 会多 1 条 xor + 块序反转）。
            if (PowerWarOn())
            {
                if (attacker->getCurCharacVill() != 0x7)
                {
                    if (preventPeerFlag)
                    {
                        throw static_cast<ENUM_ERROR>(0xfd);
                    }
                    throw static_cast<ENUM_ERROR>(0xfb);
                }
            }
            else
            {
                if (preventPeerFlag)
                {
                    throw static_cast<ENUM_ERROR>(0xfd);
                }
                throw static_cast<ENUM_ERROR>(0xfb);
            }
        }

        // ORIG 082ee1fb 直接把全局对象地址当 this 传入（`mov &s_villageMonsterMgr,
        // %eax; mov %eax,(%esp); call OnEvent`，无判空分支）⇒ 原码无防御。
        if (GlobalData::s_villageMonsterMgr->OnEvent())
        {
            throw static_cast<ENUM_ERROR>(0xd9);
        }

        // P0：ORIG 082ee238/082ee331 各自独立 call IsPowerWarEventOn/getCurCharacVill
        // （无槽往返 ⇒ 原码无具名 bool 局部）。
        if (PowerWarOn() && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptCount();
        }

        // ORIG 082ee298 直接解引用返回值（无判空、无 event 具名槽，P171/P0）。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x2a)->IsEventing(0))
        {
            throw static_cast<ENUM_ERROR>(0xb2);
        }

        if (attacker->getCurCharacVill() == 0x8)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        // ORIG 082ee32b `je 082ee4f4`（PowerWarOn()==0 → 尾随 else 块）+ 082ee343
        // `je 082ee4c8`（vill!=7 → 尾随 throw 0xfc 段）⇒ 原码书写序 = `if
        // (PowerWarOn()) { ...大块... } else { vill==7 → 0x13 }`（P324 叶块序）。
        if (PowerWarOn())
        {
            // ORIG 082ee33c `cmp $0x7,%al; sete %al; test %al,%al; je 082ee4c8`
            // （082ee4c8 = throw 0xfc 段）⇒ 原码 = `if (vill == 0x7) {...} else
            // throw 0xfc;`（sete 而非 setne，P324 叶块书写序）。
            if (attacker->getCurCharacVill() == 0x7)
            {
                // ORIG 082ee349/082ee35e：实参内联（area 留 %ebx 无槽，P0/P183）。
                if (G_GameWorld()->IsPowerWarSafeZone(
                        static_cast<int>(attacker->getCurCharacVill()),
                        attacker->get_area(false)))
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                if (attacker->getPowerSide() == target->getPowerSide()
                    || attacker->isJoinPowerWar() == 0
                    || target->isJoinPowerWar() == 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                CParty* party = static_cast<CParty*>(attacker->GetParty());
                if (party != 0 && party->CanDoPartyActionAssalutState() == 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
                if (target->GetCurCharacSuperState() != 0)
                {
                    throw static_cast<ENUM_ERROR>(0x13);
                }
            }
            else
            {
                throw static_cast<ENUM_ERROR>(0xfc);
            }
        }
        else
        {
            if (attacker->getCurCharacVill() == 0x7)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (expert_job::CAlchemist::GetAssaultPrevent(
                target, static_cast<bool*>(0)))
        {
            if (!PowerWarOn())
            {
                throw static_cast<ENUM_ERROR>(0x11);
            }
        }

        if (G_CEnvironment()->m_serverEnvir.m_serverGroup == 3)
        {
            // ORIG 082ee5b6/082ee5c8 两次独立 getCurCharacNo（无 targetNo 局部）。
            if (target->getCurCharacNo() == 0x1eaa0d
                || target->getCurCharacNo() == 0x2c55a8)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        if (!m_bAssaultChannel)
        {
            // ORIG 082ee62b `ja`（无符号比较）+ 值直留 %eax（无槽，P0/P197）：
            // 原码为 unsigned 比较、无具名局部。
            if (static_cast<unsigned int>(target->getCurrCharacBlackCount()) < 100
                && target->GetCurCharacChaosPoint() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x8);
            }
        }

        if (attacker->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (target->GetAssaultPlace() != 0)
        {
            throw static_cast<ENUM_ERROR>(0x12);
        }
        if (attacker->getCurCharacR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacInvenR() == 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (attacker->getCurCharacNo() == target->getCurCharacNo())
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        // ORIG 082ee7be..082ee82c：单条短路或 + abs builtin（dx/absDx/dy/absDy
        // 全无具名槽：X 段 `cmp $0x64,%eax; jg`、Y 段 `jle`，值只在寄存器里
        // 走 sar/xor/sub；P0/P329）。
        if (__builtin_abs(static_cast<int>(attacker->get_posX())
                          - static_cast<int>(target->get_posX())) > 100
            || __builtin_abs(static_cast<int>(attacker->get_posY())
                             - static_cast<int>(target->get_posY())) > 100)
        {
            throw static_cast<ENUM_ERROR>(99);          // ORIG movl $0x63
        }

        // ORIG 082ee88d..082ee8b5：同形短路或（vill 用 `jne` 到 TRUE、area 用
        // `je` 到 FALSE），无 tooFar 槽复用。
        if (attacker->getCurCharacVill() != target->getCurCharacVill()
            || attacker->get_area(false) != target->get_area(false))
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }

        if (!PowerWarOn())
        {
            if (IsStayGate(attacker) || IsStayGate(target))
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        // ORIG 082ee95a/082ee9ba 两次独立 GetInstancePrivateStoreMgr（无 storeMgr 局部）。
        private_store::CPrivateStore* store =
            private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(attacker);
        // ORIG 082ee971..082ee9b5：`cmpl $0x0,-0x38; setne %al; test %al,%al; je
        // 082ee9ba` + 第二守卫同落 082ee9ba（**无** `mov $1/jmp/mov $0/test` 整体
        // 物化）⇒ 原码为嵌套 if（P324 叶块书写序；`&&` 直写令 4.4 物化整体多 4 条）。
        if (store != 0)
        {
            if (store->isDollCreated() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x3c);
            }
        }
        store = private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(target);
        if (store != 0)
        {
            if (store->isDollCreated() == 0)
            {
                throw static_cast<ENUM_ERROR>(0x3c);
            }
        }

        CParty* attackerParty = 0;
        if (attacker->CheckInParty() != 0)
        {
            attackerParty = static_cast<CParty*>(attacker->GetParty());
        }
        if (attackerParty != 0 && attackerParty->IsExistUser(target) != 0)
        {
            throw static_cast<ENUM_ERROR>(0x13);
        }
        if (PowerWarOn()
            && target->getCurCharacVill() == 0x7)
        {
            CParty* targetParty = static_cast<CParty*>(target->GetParty());
            if (targetParty != 0 && targetParty->IsThereGhostInParty() != 0)
            {
                throw static_cast<ENUM_ERROR>(0x13);
            }
        }

        AvailableAssault(attacker, attacker);
        AvailableAssault(target, attacker);

        chargeMoney = attacker->GetCurCharacAssaultMoney();
        // ORIG 082eeb34/082eeb62 两次 getCurCharacInvenR + get_money（差额在异常路径重算）。
        if (static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money()
            < chargeMoney)
        {
            chargeMoney =
                chargeMoney
                - static_cast<const CInventory*>(attacker->getCurCharacInvenR())->get_money();
            throw static_cast<ENUM_ERROR>(0xa);
        }

        // param_3 非空＝纯校验模式（DryRun）：通过全部校验后立即成功返回。
        if (result != 0)
        {
            return true;                                 // ORIG +0xbf3 jne +0x11a9
        }

        CAssaultPlaceFactory* factory = GetInstanceAssaultPlaceFactory();
        acquiredPlace = factory->AcquireAssaultPlace();
        if (acquiredPlace == 0)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }
        acquiredPlace->Reset();
        nextPlaceId = _GetNextAssaultPlaceId();
        acquiredPlace->SetAssaultPlaceId(nextPlaceId);
        acquiredPlace->MakeTeam(attacker, target, chargeMoney);

        std::pair<std::map<unsigned int, CAssaultPlace*>::iterator, bool> inserted =
            m_places.insert(std::make_pair(nextPlaceId, acquiredPlace));
        (void)inserted;

        // ORIG 082eecb0..082eece6：两结果只在 %ebx/%eax 跨 call 保活（GetAuthenValue
        // →%ebx、GetAssaultPlaceId→%eax 直落实参，无槽往返）⇒ 原码实参内联（P0/P329）。
        TimerCompleteLoadAssaultTimeout::registNextTimer(
            0x3c, acquiredPlace->GetAssaultPlaceId(),
            acquiredPlace->GetAuthenValue());

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            chargeMoney = 0;
        }

        if (chargeMoney > 0)
        {
            Inven_Item equipSlot =
                static_cast<const CInventory*>(attacker->getCurCharacInvenR())->GetInvenSlot(0, 0xb);
            const int itemIdx = equipSlot.m_addInfo;
            // ORIG 082eed66..082eed75：G_CDataManager() 结果直供 this（无 cdm 具名槽、
            // 无判空分支）——旧版防御性 `(cdm != 0) ? : 0` 已按 ORIG 移除（§10 #17）。
            const CItem* discountItem = G_CDataManager()->find_item(itemIdx);
            if (discountItem != 0)
            {
                const float discount =
                    static_cast<const CEquipItem*>(discountItem)
                        ->getUpgradeCostDiscount();
                const float pricedF =
                    ((discount * static_cast<float>(chargeMoney)) / (-100.0f)
                     + static_cast<float>(chargeMoney));
                chargeMoney = static_cast<int>(pricedF);    // ORIG fstcw/fistpl 截断
            }
            // ORIG 082eeec2：getCurCharacInvenW 结果直留 %eax 当 this（无具名槽）。
            attacker->getCurCharacInvenW()->use_money(
                chargeMoney, static_cast<eMoneySubReason>(0x15), true);
            attacker->SetCurCharacAssaultMoney(0);
            attacker->SetCurCharacAssaultedUserId(0);
        }
        attacker->IncCurCharacAssaultCount();

        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7)
        {
            // ORIG 082eeeb1/082eeecd 两次独立 GetPowerWarLog（无 log 局部）；
            // GetAssaulterCount 结果直落 0x4(%esp)（无 joinCount 槽，P0/P329）。
            GlobalData::s_power_manager->GetPowerWarLog()->IncTotalJoinAssaultUser(
                acquiredPlace->GetAssaulterCount());
            GlobalData::s_power_manager->GetPowerWarLog()->IncAssaultAttemptSuccessCount();
        }

        _SendVictoryInfo(attacker, target);

        {
            PacketGuard packet;
            packet.put_header(0, 0x8);
            packet.put_short(
                static_cast<int>(
                    static_cast<unsigned short>(target->get_unique_id())));
            packet.put_byte(4);
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
    catch (ENUM_ERROR err)
    {
        if (PowerWarOn()
            && attacker->getCurCharacVill() == 0x7 && result == 0)
        {
            // ORIG catch 内计数用 IncAssaultAttemptFailureCount（0x82f0850）
            GlobalData::s_power_manager->GetPowerWarLog()
                ->IncAssaultAttemptFailureCount();
        }
        if (result != 0)
        {
            *result = static_cast<int>(err);
            return false;
        }
        FreeAssaultPlace(static_cast<int>(nextPlaceId), acquiredPlace);
        {
            PacketGuard packet;
            packet.put_header(1, 0xa);
            packet.put_byte(0);
            packet.put_byte(static_cast<int>(err));
            packet.put_byte(4);                     // ORIG 082ef0ce（旧实现漏写，真语义缺口）
            packet.put_int(chargeMoney);
            packet.finalize(true);
            attacker->Send(packet);
        }
        return true;
    }
}