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

        int pay = payBase;                         // -0x2c
        // ORIG 无判空防御（0x085481bc 直接解引用返回值，P140）。
        CEventBase* repeat =
            GlobalData::s_event_manager->GetRepeatEvent(0x32);
        if (repeat->IsEventing((CUser*)0) && !target.m_amp.hasAbility())
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
