int CUser::exchange_interspace(int fromSpace, int fromSlot, unsigned long fromItemIdx,
                               int addInfo, int toSpace, int toSlot,
                               unsigned long toItemIdx, int* outSlot)
{
    // b28d-solo-CUserStatC 判词：N 封顶（N82 帧/槽 + 4.1 shr $0x1f 符号位 lowering）。
    // P167 CLEAR（265/265）；gap 18/2637；真语义 bug 1 处已修。@0x865f7b0
    PacketGuard packetGuard;      // ORIG 栈上 RAII 局部（无其它使用）
    Inven_Item item;              // 主工作副本（ORIG -0x131）

    if (!_checkItemMovable(this, (ENUM_ITEMSPACE)fromSpace, fromSlot, fromItemIdx,
                           (ENUM_ITEMSPACE)toSpace, toSlot, toItemIdx))
    {
        *outSlot = -1;
        return 0x15;
    }

    switch (fromSpace)
    {
    // ---------------------------------------------------------------- 背包源
    case 0:
        switch (toSpace)
        {
        case 0:  // 背包→背包
        {
            int ret = getCurCharacInvenW()->move_item(INVEN_TYPE_EQUIP, fromSlot,
                                                      INVEN_TYPE_EQUIP, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x0865f8a0 为 `cmpl $0x0,槽; setne %al;
            // test %al,%al; je <成功>`（4.1 bool 物化）⇒ P225 形 while+return 复现。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            return 0;
        }
        case 2:  // 背包→角色仓库
        {
            if (CheckInTrade())
                return 0x13;

            item = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, fromSlot);

            CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
            if (pItem == 0)
                return 0x17;
            if (pItem->IsImpossibleCharacCargo())  // ORIG vtable+0x40
                return 0x17;
            if (item.m_addInfo > 0x1963 && item.m_addInfo < 7000)
                return 0x17;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                {
                    toSlot = getCurCharacCargoR()->get_empty_slot();
                    if ((unsigned char)(((unsigned int)toSlot) >> 31))
                        return 4;
                }
            }
            else
            {
                if (item.m_field1 == 4)
                    return 0x17;
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int exist = getCurCharacCargoR()->check_item_exist(item.m_addInfo);
                if (exist >= 0)
                    toSlot = exist;
                else
                {
                    if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                    {
                        toSlot = getCurCharacCargoR()->get_empty_slot();
                        if ((unsigned char)(((unsigned int)toSlot) >> 31))
                            return 4;
                    }
                }
            }

            toSlot = getCurCharacCargoR()->tryInsertItemSpecialSlot(item, toSlot);
            if ((unsigned char)(((unsigned int)toSlot) >> 31))
                return 4;

            bool delOk = getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, fromSlot, addInfo, (eItemDelReason)7, true);
            // b28d-solo-CUserStatC：ORIG 0x0865fbdf 为 `mov %al,槽; cmpb $0x0,槽;
            // je <失败 trace 块>`（4.1 bool 局部直测）；4.4 对 `if (!delOk)` 恒加
            // `movzbl; xor $1`（N122）⇒ 反写 `if (delOk)` 复现。
            if (delOk)
            {
                *outSlot =
                    getCurCharacCargoW()->insert_item_special_slot(item, toSlot);

                const Inven_Item slotIt =
                    getCurCharacCargoR()->get_cargo_slot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.ItemAddCargo(item.m_addInfo, addInfo, 1);
                else
                    m_historyLog.ItemAddCargo(item.m_addInfo, addInfo,
                                              slotIt.get_add_info());
            }
            else
            {
                // ORIG：删除失败仍返回 0
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3b21, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(), fromSlot,
                      addInfo);
            }
            break;
        }
        case 3:  // 背包→装备栏
        {
            if (!CheckInTrade())
            {
                int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
                if (ret == 0)
                {
                    *outSlot = toSlot;
                    void* party = GetParty();
                    if (party != 0)
                        ((CParty*)party)->Reset_party_overlapped_drop_ratio();
                    return 0;
                }
                return ret;
            }
            return 0x13;
        }
        case 4:  // 背包→交易窗口
        {
            CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x0865fdfd 为 `xor $1; test %al,%al;
            // je <then>`（bool callee 取反段）⇒ 源码是反极性 if(!f()){else 体}
            // else{then 体}（P272/P292）；写 ==1 时 4.4 恒产 cmp $1。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 0, fromSlot, addInfo);
                if (ret < 0)
                {
                    int characNo = getCurCharacNo();
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3b57, 5);
                    trace("(regist_item(): %d)(m_id: %s) (CharacNo: %d) "
                          "(from_slot: %d) (item_count: %d)",
                          ret, accStr, characNo, fromSlot, addInfo);
                    if (ret == -4)
                        return 0x5e;
                    return 0x11;
                }
                *outSlot = ret;
            }
            break;
        }
        case 0xc:  // 背包→账号金库
        {
            if (CheckInTrade())
                return 0x13;
            if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    this, (RestrictCategory::Enum)1, 0x1b))
                return 0xd1;
            int secuRet =
                GlobalData::s_pSecuProtectionField->Check(this,
                                                          (SECURITY_PROTCTION)1);
            if (secuRet != 0)
                return secuRet;
            if (!IsExistAccountCargo())
                return 0x13;

            CAccountCargo* cargo = GetAccountCargo();
            item = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, fromSlot);

            if (IsTradeLimitAttachTypeItem(item))
                return 0x17;
            if (!cargo->CheckInsertCondition(item))
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargo->CheckSlotEmpty(toSlot))
                {
                    toSlot = cargo->GetEmptySlot();
                    if ((unsigned char)(((unsigned int)toSlot) >> 31))
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (!cargo->CheckStackLimit(specific, item.m_addInfo, addInfo))
                        return 4;
                    toSlot = specific;
                }
                else
                {
                    if (!cargo->CheckSlotEmpty(toSlot))
                    {
                        toSlot = cargo->GetEmptySlot();
                        if ((unsigned char)(((unsigned int)toSlot) >> 31))
                            return 4;
                    }
                }
            }

            bool delOk = getCurCharacInvenW()->delete_item(
                INVEN_TYPE_EQUIP, fromSlot, addInfo, (eItemDelReason)0x25, true);
            if (delOk)
            {
                *outSlot = cargo->InsertItem(item, toSlot);
                const Inven_Item slotIt = cargo->GetSlot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo, 1, 1,
                                                     (eItemAddReason)0x2d);
                else
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo,
                                                     slotIt.m_addInfo2, addInfo,
                                                     (eItemAddReason)0x2d);
            }
            break;
        }
        default:
            return 1;
        }
        break;

    // ------------------------------------------------------------ 角色仓库源
    case 2:
        switch (toSpace)
        {
        case 0:  // 仓库→背包
        {
            if (CheckInTrade())
                return 0x13;

            item = getCurCharacCargoR()->get_cargo_slot(fromSlot);
            Inven_Item copy = item;
            if (copy.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
                copy.set_add_info(addInfo);

            if (getCurCharacInvenW()->tryInsertItemIntoInventory(copy) == -1)
                return 0x11;

            bool delOk =
                getCurCharacCargoW()->delete_item(fromSlot, addInfo,
                                                  (eItemDelReason)7);
            // b28d-solo-CUserStatC：ORIG 0x08660624 为 `mov %al,槽; cmpb $0x0,槽;
            // je <失败 trace 块>`；4.4 对 `if (!delOk)` 恒加 `movzbl; xor $1`（N122）
            // ⇒ 反写 `if (delOk)` 复现。
            if (delOk)
            {
                *outSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    copy, (eItemAddReason)7, true, false);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3be5, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), copy.m_addInfo, addInfo);
                }
            }
            else
            {
                cMyTrace trace("int CUser::exchange_interspace(int, int, "
                               "itemIndexOnlyServer_t, int, int, int, "
                               "itemIndexOnlyServer_t, int*)",
                               0x3bea, 5);
                trace("delete_item failed %d %d %d", getCurCharacNo(),
                      item.m_addInfo, addInfo);
            }
            return 0;
        }
        case 2:  // 仓库→仓库
            // b28d-solo-CUserStatC：ORIG 0x0866080d 为 `call; xor $0x1,%eax;
            // test %al,%al; je <then>` ⇒ 反极性早返形（P292/P272）。
            if (!getCurCharacCargoW()->move_item(fromSlot, toSlot))
                return 0x11;
            *outSlot = toSlot;
            return 0;
        case 3:
            return CheckInTrade() ? 0x13 : 1;
        case 7:
            return 1;
        case 0xc:  // 仓库→账号金库
        {
            if (CheckInTrade())
                return 0x13;
            int secuRet =
                GlobalData::s_pSecuProtectionField->Check(this,
                                                          (SECURITY_PROTCTION)1);
            if (secuRet != 0)
                return secuRet;
            if (!IsExistAccountCargo())
                return 0x13;

            CAccountCargo* cargo = GetAccountCargo();
            item = getCurCharacCargoR()->get_cargo_slot(fromSlot);

            if (!cargo->CheckInsertCondition(item))
                return 0x11;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!cargo->CheckSlotEmpty(toSlot))
                {
                    toSlot = cargo->GetEmptySlot();
                    if ((unsigned char)(((unsigned int)toSlot) >> 31))
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int specific = cargo->GetSpecificItemSlot(item.m_addInfo);
                if (specific >= 0)
                {
                    if (!cargo->CheckStackLimit(specific, item.m_addInfo, addInfo))
                        return 4;
                    toSlot = specific;
                }
                else
                {
                    if (!cargo->CheckSlotEmpty(toSlot))
                    {
                        toSlot = cargo->GetEmptySlot();
                        if ((unsigned char)(((unsigned int)toSlot) >> 31))
                            return 4;
                    }
                }
            }

            bool delOk =
                getCurCharacCargoW()->delete_item(fromSlot, addInfo,
                                                  (eItemDelReason)0x25);
            if (delOk)
            {
                *outSlot = cargo->InsertItem(item, toSlot);
                const Inven_Item slotIt = cargo->GetSlot(*outSlot);
                if (slotIt.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo, 1, 1,
                                                     (eItemAddReason)0x2e);
                else
                    m_historyLog.AccountCargoAddItem(slotIt.m_addInfo,
                                                     slotIt.m_addInfo2, addInfo,
                                                     (eItemAddReason)0x2e);
            }
            return 0;
        }
        default:
            return 1;
        }

    // -------------------------------------------------------------- 装备栏源
    case 3:
        switch (toSpace)
        {
        case 0:  // 装备→背包
        {
            if (!CheckInTrade())
            {
                int ret = change_equip(toSpace, (short)toSlot, fromSlot);
                if (ret == 0)
                {
                    *outSlot = fromSlot;
                    return 0;
                }
                return ret;
            }
            return 0x13;
        }
        case 1:  // 装备→时装
        {
            Inven_Item equipSlot =
                getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, fromSlot);
            Inven_Item avatarSlot =
                getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, toSlot);
            if (WongWork::CAvatarItemMgr::IsTempKey((int)equipSlot.m_addInfo2)
                || WongWork::CAvatarItemMgr::IsTempKey((int)avatarSlot.m_addInfo2))
                return 0x13;
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x0866168a 为 `cmpl $0x0,槽; jle <then>`
            // （then 为跳转目标、else 直落返回 ret）⇒ 必须写成「正守卫早返」形，
            // `if (ret < 1) {then}` 与 `if (ret <= 0) {then}` 均使 4.4 反产 `jg`。
            if (ret > 0)
                return ret;
            *outSlot = toSlot;
            return 0;
        }
        case 2:
            return CheckInTrade() ? 0x13 : 1;
        case 4:  // 装备→交易窗口
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x0866103e 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 3, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
                send_equip(fromSlot);
            }
            return 0;
        }
        case 7:  // 装备→宠物
        {
            if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
            {
                // b28d-solo-CUserStatC：真语义 bug 修正——ORIG 0x08660ec4 实参为
                // INVEN_TYPE_CREATURE(3)（目标为宠物槽），原写 INVEN_TYPE_INVEN(0) 取错域。
                const Inven_Item* ref = getCurCharacInvenR()->GetInvenRef(
                    INVEN_TYPE_CREATURE, toSlot);
                if (!online_preliminary::COnlinePreliminary::
                        CheckUnusableEquipment(ref))
                {
                    SendNotiPacketMessage(
                        g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                          (bool*)0),
                        (ENUM_MESSAGE_TYPE)0);
                    return 1;
                }
            }
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_INVEN, fromSlot, INVEN_TYPE_CREATURE, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x08660f6d 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            send_equip(toSlot);
            return 0;
        }
        default:
            return 1;
        }

    // ------------------------------------------------------------ 交易窗口源
    case 4:
    {
        // 允许目的域位掩码 {0,1,3,7}
        if ((unsigned int)toSpace > 7u || ((1u << toSpace) & 0x8bu) == 0)
            return 1;

        CTradeSpace* trade = G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
        if (trade == 0)
            return 1;
        if (trade->is_empty())
        {
            m_tradeSpaceIdx = -1;
            return 0x15;
        }
        // b28d-solo-CUserStatC：ORIG 0x086619f6 同为 bool 取反段 ⇒ 反极性 if。
        if (!trade->check_trade_possibility())
        {
            trade->cancel_trade();
            G_CGameManager()->PutTradeSpace(trade);
        }
        else
        {
            if (trade->IsLocked())
                return 0x11;
            int ret = trade->remove_item(this, fromSlot, toSpace, toSlot, addInfo);
            if (ret < 0)
                return 0x11;
            *outSlot = ret;
            if (toSpace == 3)
                send_equip(toSlot);
        }
        break;
    }

    // --------------------------------------------------------------- 时装源
    // [BigUserLogic] ORIG fromSpace 跳表(0x8cef0f4)体物理序 = 0,2,3,4,1,7,12
    // （case1 体 @+0x195e 在 case4 @+0x17ff 后、case7 @+0x2066 前）；4.4 -O0 的
    // switch 体序 = case 书写序 ⇒ 整块移到 case4 之后，语义零改动。
    case 1:
    {
        // ORIG 在分支入口构造两个临时项并按目的域选择性填充。
        Inven_Item itemA;
        Inven_Item itemB;  // toSpace==4 时仅保持构造初值（ORIG 同）
        // b28d-solo-CUserStatC：ORIG 0x0866112a/0x08661641 为「形参载入寄存器 + cmp 链」
        // （mov 0x1c(%ebp),%eax; cmp $3/$4/$1; je），4.4 对 if-else 链恒折内存直比；
        // 3-case switch 按书写序复现该链（P286/P208 推广，探针 /tmp/b28d_cstatc_probe4.cpp）。
        switch (toSpace)
        {
        case 3:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            // ORIG：toSpace==3 各子分支独立读两个槽（itemA/itemB 不合并条件）。
            itemB = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, toSlot);
            break;
        case 4:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            break;
        case 1:
            itemA = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, fromSlot);
            itemB = getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_AVATAR, toSlot);
            break;
        default:
            break;
        }

        // ORIG:IsTempKey 为静态语义（值符号位判定），直接静态调用，不经 mgr 访问器。
        if (WongWork::CAvatarItemMgr::IsTempKey((int)itemA.m_addInfo2)
            || WongWork::CAvatarItemMgr::IsTempKey((int)itemB.m_addInfo2))
            return 0x13;

        switch (toSpace)
        {
        case 3:
        {
            int ret = change_equip(fromSpace, (short)fromSlot, toSlot);
            if (ret > 0)
                return ret;
            *outSlot = toSlot;
            break;
        }
        case 4:
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x08660de9 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 1, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            break;
        }
        case 1:
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_AVATAR, fromSlot, INVEN_TYPE_AVATAR, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x086616e3 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            break;
        }
        default:
            return 1;
        }
        break;
    }

    // -------------------------------------------------------------- 宠物源
    case 7:
        // b28d-solo-CUserStatC：ORIG 0x08661816 为「形参载入寄存器 + cmp 链 4/7/3」
        // （cmp 序 = case 书写序），if-else 链 4.4 恒折内存直比 ⇒ 改 switch。
        switch (toSpace)
        {
        case 4:
        {
            CTradeSpace* trade =
                G_CGameManager()->GetTradeSpace((int)m_tradeSpaceIdx);
            if (trade == 0)
                return 1;
            if (trade->is_empty())
            {
                m_tradeSpaceIdx = -1;
                return 0x15;
            }
            // b28d-solo-CUserStatC：ORIG 0x08661767 同为 bool 取反段 ⇒ 反极性 if。
            if (!trade->check_trade_possibility())
            {
                trade->cancel_trade();
                G_CGameManager()->PutTradeSpace(trade);
            }
            else
            {
                if (trade->IsLocked())
                    return 0x11;
                int ret = trade->regist_item(this, 7, fromSlot, addInfo);
                if (ret < 0)
                    return 0x11;
                *outSlot = ret;
            }
            break;
        }
        case 7:
        {
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_CREATURE, fromSlot, INVEN_TYPE_CREATURE, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x08661900 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            break;
        }
        case 3:
        {
            if (G_GameWorld()->IsCheckUnusableEquipmentChannel())
            {
                const Inven_Item* ref = getCurCharacInvenR()->GetInvenRef(
                    INVEN_TYPE_CREATURE, fromSlot);
                if (!online_preliminary::COnlinePreliminary::
                        CheckUnusableEquipment(ref))
                {
                    SendNotiPacketMessage(
                        g_scriptStringManager_.findString(4, "game_server_msg_146",
                                                          (bool*)0),
                        (ENUM_MESSAGE_TYPE)0);
                    return 1;
                }
            }
            int ret = getCurCharacInvenW()->move_item(
                INVEN_TYPE_CREATURE, fromSlot, INVEN_TYPE_INVEN, toSlot);
            // b28d-solo-CUserStatC：ORIG 0x08661972 同 setne 物化 ⇒ P225 形。
            while (ret != 0)
            {
                return ret;
            }
            *outSlot = toSlot;
            send_equip(toSlot);
            return 0;
        }
        default:
            return 1;
        }
        break;

    // ----------------------------------------------------------- 账号金库源
    case 0xc:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                this, (RestrictCategory::Enum)1, 0x1b))
            return 0xd1;
        int secuRet =
            GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)1);
        if (secuRet != 0)
            return secuRet;

        switch (toSpace)
        {
        case 0:  // 金库→背包
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();

            item = cargo->GetSlot(fromSlot);
            Inven_Item copy = item;
            if (copy.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
                copy.set_add_info(addInfo);

            if (getCurCharacInvenW()->tryInsertItemIntoInventory(copy) == -1)
                return 0x11;

            bool delOk = cargo->DeleteItem(fromSlot, addInfo);
            if (delOk)
            {
                *outSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    copy, (eItemAddReason)7, true, false);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3ed3, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), copy.m_addInfo, addInfo);
                }
                // ORIG：删除后重读槽位供历史日志计数（第二临时副本）。
                const Inven_Item after = cargo->GetSlot(fromSlot);
                if (after.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoDelItem(copy.m_addInfo, 0, 1,
                                                     (eItemDelReason)0x26);
                else
                    m_historyLog.AccountCargoDelItem(copy.m_addInfo,
                                                     after.m_addInfo2, addInfo,
                                                     (eItemDelReason)0x26);
            }
            break;
        }
        case 2:  // 金库→角色仓库
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();

            item = cargo->GetSlot(fromSlot);

            if (item.m_addInfo > 0x1963 && item.m_addInfo < 7000)
                return 0x17;
            if (item.m_field1 == 4)
                return 0x17;

            if (item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                {
                    toSlot = getCurCharacCargoR()->get_empty_slot();
                    if ((unsigned char)(((unsigned int)toSlot) >> 31))
                        return 4;
                }
            }
            else
            {
                if (item.get_add_info() < addInfo)
                    return 0x11;
                item.set_add_info(addInfo);
                int exist = getCurCharacCargoR()->check_item_exist(item.m_addInfo);
                if (exist >= 0)
                    toSlot = exist;
                else
                {
                    if (!getCurCharacCargoR()->check_slot_empty(toSlot))
                    {
                        toSlot = getCurCharacCargoR()->get_empty_slot();
                        if ((unsigned char)(((unsigned int)toSlot) >> 31))
                            return 4;
                    }
                }
            }

            toSlot = getCurCharacCargoR()->tryInsertItemSpecialSlot(item, toSlot);
            if ((unsigned char)(((unsigned int)toSlot) >> 31))
                return 4;

            bool delOk = cargo->DeleteItem(fromSlot, addInfo);
            if (delOk)
            {
                *outSlot =
                    getCurCharacCargoW()->insert_item_special_slot(item, toSlot);
                if (*outSlot == -1)
                {
                    char* accStr = NumberToString(get_acc_id(), 0);
                    cMyTrace trace("int CUser::exchange_interspace(int, int, "
                                   "itemIndexOnlyServer_t, int, int, int, "
                                   "itemIndexOnlyServer_t, int*)",
                                   0x3f17, 5);
                    trace("insertItemIntoInventory failed %s %d %d %d", accStr,
                          getCurCharacNo(), item.m_addInfo, addInfo);
                }
                const Inven_Item after = cargo->GetSlot(fromSlot);
                if (after.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                    m_historyLog.AccountCargoDelItem(item.m_addInfo, 0, 1,
                                                     (eItemDelReason)0x27);
                else
                    m_historyLog.AccountCargoDelItem(item.m_addInfo,
                                                     after.m_addInfo2, addInfo,
                                                     (eItemDelReason)0x27);
            }
            break;
        }
        case 3:
            return CheckInTrade() ? 0x13 : 1;
        case 7:
            return 1;
        case 0xc:  // 金库→金库
        {
            if (CheckInTrade())
                return 0x13;
            if (!IsExistAccountCargo())
                return 0x13;
            CAccountCargo* cargo = GetAccountCargo();
            if (!cargo->MoveItem(fromSlot, toSlot))
                return 0x11;
            *outSlot = toSlot;
            break;
        }
        default:
            return 1;
        }
        break;
    }
    default:
        return 1;
    }
    return 0;
