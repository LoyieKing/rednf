int Dispatcher_UseBoosterItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    short slot = -1;
    if (!packet.get_short(slot))
        return LineFunc(0xa436, __PRETTY_FUNCTION__, 0, 0);

    // 交易/私人商店中禁止使用（ORIG 短路或：GetInstancePrivateStoreMgr 只调一次）
    if (user->CheckInTrade() ||
        private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
        return 0;
    }

    if (user->getCurCharacR() == 0)
        return 0xa43e;

    // ORIG 0x08208e10：无判空，GetInvenSlot 的 sret 直接落 invItem（-0xe1）；
    // 其后所有 itemId 使用点都是成员直读（-0xdf），无具名 itemId 局部（P0）。
    Inven_Item invItem =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_INVEN, (int)slot);

    CStackableItem* stackable =
        static_cast<CStackableItem*>(G_CDataManager()->find_item((int)invItem.m_addInfo));

    // 物品种类必须为可叠放(0x10)或选择强化罐(0x22)；ORIG 逐次虚调 GetItemType（P48）。
    if (stackable == 0 ||
        (stackable->GetItemType() != 0x10 && stackable->GetItemType() != 0x22))
        return LineFunc(0xa450, __PRETTY_FUNCTION__, 0, 0);

    // 限制条件：罐子类型或 state==3 才允许
    if (invItem.m_addInfo != 0x28d3c1 && user->get_state() != 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
        return 0;
    }

    // 转职校验（ORIG 0x08208f20：无 grow/job 局部，实参右→左求值：先 job 后 grow）
    if (!stackable->check_change_job_type(user->getCurCharFirstGrowType(),
                                          (char)user->get_charac_job()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x17);
        return 0;
    }

    std::vector<Inven_Item> resultItems;
    char c1 = 0;
    char c2 = 0;

    if (stackable->GetItemType() == 0x22)
    {
        // ---- 选择强化罐：先读两个字节（类别/类型），再在 Booster 表中定位 ----
        if (!packet.get_byte(c1))
            return LineFunc(0xa473, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(c2))
            return LineFunc(0xa475, __PRETTY_FUNCTION__, 0, 0);

        // ORIG 0x08209036：Inven_Item 在 count 读取之前构造一次（循环外复用）。
        Inven_Item add;

        // 在 stStackableBooster_t::m_map（+0x24c）里按 (c1,c2) 取值。
        // ORIG 走 const find/end + const_iterator（_ZNKSt3map…findERS8_），
        // 且无引用局部（P183：4.4 引用/指针局部恒落槽）。

        int count = stackable->m_boosterSelect.m_field8;   // +0x26c
        if (count == 0)
        {
            std::map<std::pair<char, char>,
                     std::vector<stStackableBoosterItemInfo_t> >::const_iterator it =
                static_cast<const CStackableItem*>(stackable)->m_booster.m_map
                    .find(std::make_pair(c1, c2));
            // ORIG 0x082090ae：end() 只求值一次，empty/size 走嵌套 if。
            if (it != static_cast<const CStackableItem*>(stackable)->m_booster.m_map.end())
            {
                if (it->second.empty())
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xa480, 5)
                        ("selectionBooster empty : %d, category:%d,%d\n",
                         stackable->get_index(), (int)c1, (int)c2);
                    return LineFunc(0xa481, __PRETTY_FUNCTION__, 0, 0);
                }
                count = (int)it->second.size();
            }
        }

        int i = 0;
        while (i < count && i < 0x14)
        {
            add.reset();
            // ORIG get_int(unsigned long&) 直接写 add.m_addInfo（+0x02）。
            if (!packet.get_int(reinterpret_cast<unsigned long&>(add.m_addInfo)))
                return LineFunc(0xa48c, __PRETTY_FUNCTION__, 0, 0);

            add.m_addInfo2 = 1;
            resultItems.push_back(add);
            ++i;
        }

        user->m_historyLog.UseSelectBooster(invItem.m_addInfo, (int)c1, (int)c2, resultItems);
    }

    // ---- 主路径（两种类型汇合于此）----
    {
        // ORIG 0x08209276：inst() 结果无具名局部（P0/P183）。
        if (GlobalInstance<Arad_GiftItem_Set>::inst()->level_check(user, stackable))
        {
            unsigned int prot =
                GlobalData::s_pSecuProtectionField->CheckItem(user, (SECURITY_PROTCTION)0x2a, stackable);
            if (prot != 0)
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, (unsigned char)(prot & 0xff));
                return 0;
            }

            // ORIG 0x08209369：先求 getUsableLevel（左操作数）后 get_charac_level。
            if (stackable->getUsableLevel() > user->get_charac_level())
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
                return 0;
            }

            // ORIG 0x082093af / 0x08209452：getCurCharacInvenW 两处独立调用。
            if (!user->getCurCharacInvenW()->check_empty_count(
                    Inven_Item::ITEM_TYPE_CREATURE_EQUIP, 1))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0xcb);
                return 0;
            }

            if (user->CheckCoolTimeItem((unsigned long)invItem.m_addInfo))
            {
                user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
                return 0;
            }

            if (stackable->IsDeletableItem())
            {
                // ORIG CInventory::delete_item(invW, 1, slot, 1, eItemDelReason_3, 1)
                if (!user->getCurCharacInvenW()->delete_item(
                        INVEN_TYPE_INVEN, (int)slot, 1, eItemDelReason_3, true))
                    return LineFunc(0xa4d3, __PRETTY_FUNCTION__, 0, 0);
            }
            user->_processContinuousItem(invItem.m_addInfo, stackable);
            user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, (int)slot);

            int nItemIdx  = 0;      // local_2c（作 count 传入，调用后 = 限制道具 index）
            int nProcess  = 0;      // local_78（ProcessSpecialItem 的 restrictCode& 输出）
            long curSec   = GlobalData::s_systemTime_.getCurSec();

            WongWork::CCeraShop::stCeraShopItemParam_t ceraParam;
            WongWork::CCeraShop* cera = GlobalData::s_pCeraShop;
            bool rt = cera->ProcessSpecialItem(
                user, (int)invItem.m_addInfo, nItemIdx, nProcess, curSec, true,
                resultItems, ceraParam, c1, c2, false, 1);

            if (!rt)
            {
                return LineFunc(0xa4f6, __PRETTY_FUNCTION__, 0, 0);
            }
            else if (nProcess == 0)
            {
                if (nItemIdx == 0 || user->IsRestrictedGoods(nItemIdx))
                {
                    // ok
                }
                else
                {
                    user->SetRestrictedGoods(nItemIdx);
                    cera->SaveFeaturedIdx(user, nItemIdx, (int)curSec, 0);
                    user->SendRestrictedGoods(true, nItemIdx);
                }


                _onBoosterItemUseResult(user, resultItems, (int)invItem.m_addInfo);
                _onBoosterItemUseResult_OnlyOne(user, resultItems, (int)invItem.m_addInfo);

                if (invItem.m_addInfo == 0x28d1b3)
                    user->SendEventJarOutputItemStatistics(resultItems);

                // ---- 结果包（header 0xa3）----
                // ORIG 0x082096d3：同一 invItem 重新赋值（sret 临时 -0x168 → 复制回 -0xe1）。
                invItem = const_cast<CInventory*>(user->getCurCharacInvenR())
                              ->GetInvenSlot(INVEN_TYPE_INVEN, (int)slot);

                PacketGuard pg;
                pg.put_header(1, 0xa3);
                pg.put_byte(1);
                pg.put_short((int)slot);
                pg.put_int(invItem.m_addInfo2);
                pg.put_short((int)resultItems.size());
                // ORIG 0x08209850..0x08209892：两个具名 const_iterator（begin/end
                // 各自独立语句，转换构造 ×2），比较走 const 版 operator!=。
                std::vector<Inven_Item>::const_iterator it = resultItems.begin();
                std::vector<Inven_Item>::const_iterator itEnd = resultItems.end();
                while (it != itEnd)
                {
                    pg.put_int(it->m_addInfo);
                    pg.put_int(it->m_addInfo2);
                    ++it;
                }
                pg.finalize(true);
                user->Send(pg);
            }
        }
        else
        {
            // 物品等级校验失败（ORIG 0x082092b3：cMyTrace 临时链，实参右→左求值）
            cMyTrace(__PRETTY_FUNCTION__, 0xa49b, 0)
                ("stackable item level check charac_no[%d], item_id(%d)",
                 user->get_charac_no(-1), (int)invItem.m_addInfo);
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa3, 0x13);
            return 0;
        }
    }

    return 0;
