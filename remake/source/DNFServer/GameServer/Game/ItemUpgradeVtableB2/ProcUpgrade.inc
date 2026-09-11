// ============================================================================
// ProcUpgrade @ 0x0854779c —— 强化主流程（vtable 槽 0）。
// 返回 0 成功；其余为错误码（4/8/9/0xd/0x10/0x11/0x13/0x16/0x17/0xea/0x3f4…）。
//
// 【batch-11 验收标注（§6.2 超大函数口径）】orig 1242 / ours 1246（gap 4）。
//   ① 调用点差集（P167）：ORIG direct 115 + indirect 5 ↔ ours 115 + 5，**归零**
//      （仅 Inven_Item C2×4 / upgrade_info_t C2×1 ↔ 4.4 发射 C1×5 的同体符号名
//      对称差，函数体等价，属跨 GCC 版本发形）；
//   ② 逐块比对补齐：4 处 Inven_Item「默认构造+赋值」物化块、map operator-> ×3、
//      GetRepeatEvent ×2、use_money 双分支各自重取 invenW、减费装备改走
//      getCurCharacInvenR(0,0xb)、回包 add_info 改取材料槽且重取 invenW、
//      randInt 上界改字面 99、去掉 ORIG 不存在的 GetRepeatEvent 判空防御
//      （gap 156 → 4）；
//   ③ 残余 4 insn 集中在 ticket 级 bool 汇聚岛（0x08547c51）与
//      /100 magic 除法展开（0x08548a98）⇒ 判**语义完成、byte 层
//      N76/N82/N18b 封顶**，勿再耗轮次。
// [b28d-solo-ItemUpgradeVtableB2 复核] DIFF 1242/1244 封顶（N82 + P292）：
//   ①P167 CLEAR 115/115（本轮删匿名 namespace helper×2，EXTRA 归零）；
//   ②gap -2（我方多 2 条，含 1 nop）；③帧 0x32c 同、残余为槽号整体重排
//   （N82）与两处 bool 物化（`_GetUpgradeInfo` int 返回的 `test/sete` vs
//   ORIG `xor $0x1`；callee 定义在 CItemUpgrade.cpp 属主 TU，P292 跨 TU
//   返回类型冲突不改）⇒ 语义完成、byte 层封顶。
//   MSG_ITEM_UPGRADE +0x1a/+0x1b/+0x1d/+0x21/+0x23 字段化已由 Main 代落
//   PacketTypes.h:807-812；本函数无裸偏移读（CEnvironment+0x378 已按 §4.3
//   具名化为 m_serverEnvir.m_serverGroup）。
// [2026-09-10 SweepCEquipItem2] 本轮复核（未改）：gap 1（fresh .o orig 1242 /
//   our 1241），p167 115=115 CLEAR（仅 Inven_Item/upgrade_info_t 的 C1/C2 同址
//   别名伪差）。虚槽 5/5 与 ORIG 逐处一致：0x44 / 0x30 / 0x34 / 0x34 / 0x4c
//   （ORIG 0x08547a63 / 0x08547e12 / 0x08547e4d / 0x085481be / 0x08548aae）。
//   非槽差异为 setcc-fold（setne/setge 岛）与浮点书写序（P243）；帧差 4 来自
//   ORIG 多一条 `push %edi`（N7 寄存器分配）。按 §6.2 三件套判语义完成。
//
// 物化口径（P183/P167 实证）：ORIG 对材料/保护券槽各发
//   Inven_Item C2（0x080cb854）+ GetInvenSlot 临时槽（-0x2c8）+ 15×movl+1×movb
//   逐字段拷贝，共四处独立物化块（-0x15e/-0x19b/-0x1d8/-0x215）；重建端一律
//   「默认构造 + 赋值」两步形，禁 `T x = f()` 直构（会折叠 sret 目标，丢整块）。
// [L3EHeroItemUpgrade 2026-09-11] 本轮 2 处落地（our 1241→1237、editdist
//   570→527、ratio .659→.690、TU 四数不变）：
//   ① 0x085481a0 `movl $0x0,-0x2c(%ebp)`（Ghidra local_30=0）⇒ 源码是
//      `pay = 0;` 显式初值（死写入，两分支随后覆盖），原写 `int pay = payBase`；
//   ② 0x085481b7..0x085481cb：GetRepeatEvent 返回值**无具名局部槽**
//      （`mov (%eax),%edx; add $0x34,%edx; ...; mov %eax,(%esp)`）⇒ 删
//      `CEventBase* repeat` 中间槽（editdist -42 主因）。
//   负结论：0x085482c2..0x085482ce 的 `(float)pay` 两具名 float 临时形实测
//   our 1237→1240 但 editdist 527→**621** 大幅劣化 ⇒ 回滚（P243 书写序）。
// ============================================================================
int CItemUpgrade::ProcUpgrade(CUser* user, MSG_ITEM_UPGRADE* msg,
                              PacketGuard* pg)
{
    CInventory* invenW = ((CUserCharacInfo*)user)->getCurCharacInvenW();

    Inven_Item target = invenW->GetInvenSlot(INVEN_TYPE_EQUIP,
                                             msg->m_targetSlot);
    if (target.isEmpty())
    {
        return 4;
    }
    if (target.m_addInfo != msg->m_targetItemIdx)
    {
        return 4;
    }

    Inven_Item material;
    material = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot);
    if (material.isEmpty())
    {
        return 0x16;
    }

    Event_Upgrade_Jar* jar = GlobalData::s_pEvent_UpGrade_Jar;
    if (!jar->CheckMaxLuckyLevel(target, material.m_addInfo))
    {
        return 0x3f4;
    }
    jar->CheckMaterialIndex(material.m_addInfo);

    ENUM_ERROR err = ENUM_ERROR_NONE;              // -0xdc
    msg->m_25 = _getUpgradeCountByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }
    msg->m_27 = _getUpgradeRateByUpgradeItem(target, material, err);
    if (err != ENUM_ERROR_NONE)
    {
        return (int)err;
    }

    CItem* script = G_CDataManager()->find_item(target.m_addInfo);
    if (script == 0)
    {
        return 4;
    }

    Inven_Item ticket;                             // -0x19b
    char ticketFlag = 0;                           // -0x45
    if (msg->m_ticketSlot != 0xffff)
    {
        if (!script->IsPossibleKiriProtection())   // CItem vtable+0x44
        {
            return 0xea;
        }
        ticket = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticket.isEmpty())
        {
            return 4;
        }
        std::map<unsigned int, stUpgradeItemTicketParam_t>& tickets =
            G_CDataManager()->m_serverParameter.m_ticket468;
        std::map<unsigned int, stUpgradeItemTicketParam_t>::iterator it =
            tickets.find(ticket.m_addInfo);
        if (it == tickets.end())
        {
            return 9;
        }
        // ORIG 每次 it->second 都重调 _Rb_tree_iterator::operator->（3 次：
        // 0x08547c15/0x08547c3b/0x08547c6b）、GetUpgrade 重调 2 次 ⇒ 表达式内
        // 逐点写，禁 &it->second 缓存（P183）。
        if (target.GetUpgrade() < it->second.m_view.m_minUpgrade ||
            it->second.m_view.m_maxUpgrade < target.GetUpgrade())
        {
            return 8;
        }
        ticketFlag = (char)it->second.m_view.m_ticketFlag;
    }

    // 升级表：增幅物品用 CItemAmplifier 表，否则表区子对象的 getUpgradeInfoTable。
    const upgrade_table_t* table;
    if (!target.m_amp.hasAbility())
    {
        // [b28d-solo-ItemUpgradeVtableB2] P49/P167：删匿名 namespace inline
        // helper（helper 在 -O0 是真实 call ⇒ ORIG 无的 EXTRA 符号）；基类
        // CItemUpgradeTable 成员直调，this 自动 +0x04 调整（ORIG 0x08547ca4
        // `add $0x4,%eax` + call getUpgradeInfoTable）。
        table = getUpgradeInfoTable();
    }
    else
    {
        table = GlobalData::s_itemAmplifier_->getUpgradeInfoTable();
    }

    upgrade_info_t info;                           // -0x284（ctor 自清零）
    const int level = ((const CUserCharacInfo*)user)->get_charac_level();
    if (!_GetUpgradeInfo(&target, script, *table, &info, level))
    {
        return 0xd;
    }

    // 【ORIG 怪癖】payBase/matCount 仅 upCount!=0 时赋值，其余路径沿用旧栈值。
    int payBase;                // -0x250 金钱基数
    int matCount;               // -0x254 材料消耗数量
    int ticketMatIdx;           // -0x258（ORIG 写入后无读者：死写入照抄，P171）
    if (msg->m_25 != 0)
    {
        payBase = 0;
        matCount = 1;
        ticketMatIdx = (int)material.m_addInfo;
    }
    (void)ticketMatIdx;

    if (!_CheckTargetUsableLv(material, target))
    {
        return 0x13;
    }
    const int portable = _ProcPortableUpgradeItem(material, target, info);
    if (portable != 0)
    {
        return portable;
    }

    if (ticket.m_addInfo == 0x28d30e || ticket.m_addInfo == 0x28d30f)
    {
        payBase = 0;
    }

    jar->SettingRecipeNUpConunt(material.m_addInfo, info,
                                target.GetUpgrade());

    if (msg->m_upgradeType == 0 && script->IsImpossibleUpgrade())
    {   // CItem vtable+0x30
        return 0x13;
    }
    if (msg->m_upgradeType == 1 && script->IsImpossibleAmplify())
    {   // CItem vtable+0x34
        return 0x13;
    }

    // 特殊券覆盖材料消耗数（第一处 @0x08547e89，删除前第二次见 @0x085483b4）。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketAgain;                    // -0x1d8
        ticketAgain =
            invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        // ORIG 各分支直发 movl $imm,-0x254（0x08547f9e/0x08547faa/0x08547fce/
        // 0x08547fda）⇒ if/else 独立赋值，禁三目（三目物化「先备值再 store」
        // 两条，本函数 4 处共 +4 insn）。
        if (ticketAgain.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketAgain.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
    }

    int cond = 0;                                  // -0x3c
    cond = _CheckCondition(user, target, script, material, *table, info);
    if (cond != 0)
    {
        return cond;
    }
    if (msg->m_upgradeType == 0 && target.m_amp.hasAbility())
    {
        return 0x17;
    }
    if (msg->m_upgradeType == 1 && !target.m_amp.hasAbility())
    {
        return 0x17;
    }

    pg->put_byte(1);

    const int oldUp = (int)target.GetUpgrade();    // -0x38（int 槽）
    bool upgraded = false;                         // -0x32
    char winFlag = 0;                              // -0x31

    if (msg->m_25 != 0)
    {
        upgraded = true;
        // ORIG 上界是字面量 99（movl $0x63,-0xd0），非 _DoProcUpgrade 的 +0x4ec。
        unsigned long bound = 99;
        // [b28d-solo-ItemUpgradeVtableB2] P49/P167：同上，直读 m_rand 字段
        // （+0x4e8）消 helper call。
        if (m_rand->randInt(bound) < msg->m_27)
        {
            target.SetUpgrade((unsigned char)msg->m_25);
            winFlag = 1;
        }
        else
        {
            target.SetUpgrade((unsigned char)oldUp);
            winFlag = 0;
        }
    }
    else
    {
        upgraded = _DoProcUpgrade(user, &target, &info);

        // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x085481a0 `movl $0x0,-0x2c(%ebp)`
        // （Ghidra local_30 = 0）位于 GetRepeatEvent 之前 ⇒ 源码是 `pay = 0`
        // 显式初值（死写入，两分支随后覆盖），非 `int pay = payBase`。
        int pay = 0;                               // -0x2c
        // ORIG 无判空防御（0x085481bc 直接解引用返回值，P140）。
        // [L3EHeroItemUpgrade 2026-09-11] ORIG 0x085481b7..0x085481cb：
        // `call GetRepeatEvent; mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx;
        //  movl $0x0,0x4(%esp); mov %eax,(%esp); call *%edx` ⇒ 返回值**无具名
        // 局部槽**（%eax 直用，P183/P48）；我们有 `repeat` 槽多 2 次 mov 中转。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x32)
                ->IsEventing((CUser*)0) &&
            !target.m_amp.hasAbility())
        {
            // 第二次 GetRepeatEvent（0x0854820c）：ORIG 不缓存返回值（P48）。
            const int rate =
                ((CReduceUpgradeItemPay*)
                    GlobalData::s_event_manager->GetRepeatEvent(0x32))
                    ->GetReduceRate();
            if (rate > 0)
            {
                pay = payBase - (payBase * rate) / 100;
                if (pay < 0)
                {
                    pay = 0;
                }
            }
        }
        else
        {
            pay = payBase;
        }

        // 减费装备来自**右槽**（0x08548ad3 getCurCharacInvenR，type=0/slot=0xb）。
        Inven_Item worn =
            ((const CUserCharacInfo*)user)
                ->getCurCharacInvenR()
                ->GetInvenSlot((INVEN_TYPE)0, 0xb);
        const int wornIdx = (int)worn.m_addInfo;   // -0x28
        CItem* discScript = G_CDataManager()->find_item(wornIdx);
        if (discScript != 0)
        {
            const float discount =
                ((CEquipItem*)discScript)->getUpgradeCostDiscount();
            // _DAT_08c9d304 = -100.0f（objdump flds 立即数直读，P185）。
            // [L3EHeroItemUpgrade 2026-09-11 负结论] ORIG 0x085482c2..0x085482ce
            // 把 `(float)pay` 两次各物化独立 float 槽（-0x2dc 供 fadds、-0x2d8
            // 供 fmuls）。实测改写为两具名 float 临时（payForAdd/payForMul）后
            // our 1237 → 1240，但 editdist **527 → 621 大幅劣化**（新槽打乱后续
            // 全部槽号，4.4 亦可能未按声明序分配）⇒ 回滚，P243 书写序封顶。
            pay = (int)((discount * (float)pay) / -100.0f + (float)pay);
        }

        if (target.m_amp.hasAbility())
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0x2e, true) != 1)
            {
                return 10;
            }
        }
        else
        {
            if (((CUserCharacInfo*)user)->getCurCharacInvenW()->use_money(
                    pay, (eMoneySubReason)0xd, true) != 1)
            {
                return 10;
            }
        }
    }

    // 特殊券覆盖材料消耗数（第二处 @0x085483b4）+ 删券。
    if (msg->m_ticketSlot != 0xffff)
    {
        Inven_Item ticketDel;                      // -0x215
        ticketDel = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_ticketSlot);
        if (ticketDel.m_addInfo == 0x28d30f)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 0x32;
            }
            else
            {
                matCount = 10;
            }
        }
        else if (ticketDel.m_addInfo == 0x28d30e)
        {
            if (msg->m_upgradeType == 0)
            {
                matCount = 100;
            }
            else
            {
                matCount = 20;
            }
        }
        if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, msg->m_ticketSlot, 1, (eItemDelReason)3,
                true) != 1)
        {
            return 0x11;
        }
        user->m_historyLog.apply_Item(ticket.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)0);
    }

    if (((CUserCharacInfo*)user)->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, msg->m_materialSlot, matCount,
            (eItemDelReason)0xc, true) != 1)
    {
        return 0x11;
    }
    if (msg->m_25 != 0)
    {
        user->m_historyLog.apply_Item(material.m_addInfo, target.m_addInfo,
                                      (eApplyItemReason)1);
    }

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0xe, user, (unsigned int)payBase);

    pg->put_byte((int)msg->m_upgradeType);
    pg->put_short((int)msg->m_materialSlot);
    // 回包的 add_info 取自**材料槽**当前物品，且 ORIG 在此重取 invenW
    // （0x08548ee7 call getCurCharacInvenW → 0x08548f0b GetInvenSlot(1, +0x23)）。
    pg->put_int(((CUserCharacInfo*)user)
                    ->getCurCharacInvenW()
                    ->GetInvenSlot(INVEN_TYPE_EQUIP, msg->m_materialSlot)
                    .get_add_info());
    pg->put_short((int)msg->m_ticketSlot);

    if (msg->m_25 == 0)
    {
        if (jar->isNewNPC())
        {
            pg->put_byte(2);
        }
        else if (jar->isFortune())
        {
            pg->put_byte(3);
        }
        else
        {
            pg->put_byte(0);
        }
    }
    else
    {
        pg->put_byte(1);
    }
    pg->put_byte(oldUp);

    if (table->m_bodyView.m_named.m_noticeUpgradeLevel <=
            (int)target.GetUpgrade() &&
        (msg->m_25 == 0 || winFlag != 0))
    {
        _NoticeUpgrade(user, target, upgraded);
    }

    if (target.GetUpgrade() > 12)
    {
        const unsigned int logUpgraded = (unsigned int)upgraded;  // -0x2d4
        const unsigned int logUpgrade =
            (unsigned int)target.GetUpgrade();                    // -0x2d0
        // [b28d-solo-ItemUpgradeVtableB2] §4.3 裸偏移具名化：ORIG 0x0854898c
        // `call G_CEnvironment; mov 0x378(%eax),%eax` —— +0x378 即
        // CEnvironment::m_serverEnvir（+0x198）+ Server_Envir::m_serverGroup
        // （+0x1e0），同一 4 字节 int 装载（CEnvironment.h:290 / :186 已具名）。
        const unsigned int envField =
            (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        DB_ReqItemUpgradeLog::makeRequest(
            envField, target.m_addInfo,
            ((const CUserCharacInfo*)user)->getCurCharacNo(),
            ((const CUserCharacInfo*)user)->getCurCharacName(), logUpgrade,
            logUpgraded != 0);
    }

    if (upgraded)
    {
        if (msg->m_25 == 0)
        {
            pg->put_byte(0);
        }
        else if (winFlag == 0)
        {
            pg->put_byte(1);
        }
        else
        {
            pg->put_byte(0);
        }
        pg->put_byte((int)target.GetUpgrade());
        pg->put_short((int)msg->m_targetSlot);
        if (msg->m_25 == 0)
        {
            APSystem::CUserProc::ClearActionAndSendtoUser(
                user, (APSystem::_ActionGroupIndex)0xc, 0,
                (ENUM_PACKETCLASS)0);
        }
    }
    else
    {
        int sellPrice = 0;                         // -0xe4
        G_Store()->GetSellItemPrice(target, script,
                                    (short)target.m_addInfo2, false,
                                    sellPrice);
        GetInstanceValueStatistic()->AddValueStatistic(
            (VALUE_STATISTIC_FIELD)0xf, user, (unsigned int)sellPrice);

        const int penalty =
            _ProcPenalty(user, &target, script, msg->m_targetSlot, *table,
                         &info, pg, &ticket, ticketFlag);
        if (penalty != 0)
        {
            if (penalty != 0xd8)
            {
                return penalty;
            }
            user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 (int)msg->m_materialSlot);
            user->SendUpdateItemList((CUser::eSendTarget)1,
                                     (ENUM_ITEMSPACE)0, 0);
            return penalty;
        }
    }

    user->m_historyLog.UpgradeItem(script->get_index(), oldUp,
                                   (int)target.GetUpgrade(), upgraded);

    if (script->IsRandomOption())                  // CItem vtable+0x4c
    {
        user->makeGuildItemUpgradMessage(msg->m_28, oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }
    else
    {
        user->makeGuildItemUpgradMessage(script->GetItemName(), oldUp,
                                         (int)target.GetUpgrade(), upgraded);
    }

    if (target.m_addInfo != 0)
    {
        invenW->update_item(INVEN_TYPE_EQUIP, msg->m_targetSlot, target);
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    return 0;
}