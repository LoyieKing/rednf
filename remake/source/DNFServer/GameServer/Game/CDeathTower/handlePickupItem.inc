// [b28d-solo-CDeathTower] verdict=DIFF（orig 400 / our 418，NDIFF=145），P167 CLEAR（44/44）：残差 = 4.4 帧
// 0x… 槽号整体平移 + _pickupItem/appendPickupItem 实参跨 call 落槽 vs ORIG
// callee-saved（N82/N95/N7 族）。语义（金钱/物品/avatar 三路 + 统计）全等。
// [J10 2026-09-11 复核] orig 400 / our 404（gap −4，前值 418）；p167 44/44 CLEAR
// （CPacketHandler/map_item 的 C1↔C2、D1↔D2 同址别名）。ORIG 帧 sub $0x110 vs 我方
// sub $0x120；槽基 ORIG memberCnt -0x1c / 形参截断 -0xbc vs 我方 -0x24 / -0xcc
// ⇒ N126（4.1 首槽 -0x4 vs 4.4 -0xc）+ N115 outgoing 预留，源形不可择。
// [K9GameRestA-S322 · 定义侧同步] 返回类型 int→bool（声明 CDeathTower.h:265 已同步）：
// ORIG 调用点 0x081c3b93 `call; test %al,%al; je 0x081c3cd2`（P292 档① 单字节直判）；
// 返回类型不入 mangling ⇒ 符号/vtable 槽序/sizeof 零变化。
bool CDeathTower::handlePickupItem(CUser* user, int idx, bool a3, char a4)
{
    map_item item;
    CPacketHandler packet;

    if (!m_stage.peekItem(idx, item))
    {
        if (a4 > 3)
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x51);
        else
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    int memberCnt = m_party->get_member_count();
    if (memberCnt < 1)
    {
        packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
        user->Send(packet);
        return 0;
    }

    // ORIG：map_item+0x12 = Inven_Item.m_addInfo（item idx）判定金钱物品。
    if (item.m_item.m_addInfo == 0)
    {
        // Money item
        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }
        int moneyA[4] = {0};
        int moneyB[4] = {0};
        _pickupItemMoney(user, memberCnt, item, moneyA, moneyB);
        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupMoney(moneyA, moneyB, m_party);
    }
    else
    {
        // [判据①] ORIG 0x0846425e（else 分支首条指令）`movl $0x0,-0x4c(%ebp)` 即
        // nameBuf 预清，早于 find_item/itemType（跳转落点 0x08464265 直证）；
        // ipgNo 在 genIPGNo 前无任何清零（P171 照抄原码，勿加 ={0}）。
        char ipgNo[0x18];
        int nameBuf = 0;              // ORIG -0x4c：_pickupItem/appendPickupItem 的 char*
        CItem* citem = G_CDataManager()->find_item(item.m_item.m_addInfo);
        if (!citem)
        {
            return 0;
        }

        int itemType = 0;
        if (citem->is_stackable())
        {
            if (citem->GetItemType() == 9)
            {
                itemType = 7;
            }
        }
        else
        {
            if (citem->IsCreatureItem())
            {
                itemType = 7;
            }
            else
            {
                if (citem->IsAvatarItem())
                {
                    itemType = 1;
                }
            }
        }

        CUser* targetUser = _pickupItem(
            user, (char*)&nameBuf, memberCnt, item, a3, a4, citem);
        if (!targetUser)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x16);
            user->Send(packet);
            return 0;
        }

        int slot = -1;
        // ORIG 0x08464396..0x084643a7：&item.m_item 每使用点重发 lea+add（无具名局部，P0）。
        if (item.m_item.isAvatarItemType())
        {
            int characNo = targetUser->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5, characNo, ipgNo);
            slot = targetUser->addAvatarItem(
                item.m_item.m_addInfo, 0, (char)item.m_item.m_endurance,
                ipgNo, (eAvatarItemAddReason)0x10);
        }
        else
        {
            CInventory* inven = (CInventory*)targetUser->getCurCharacInvenW();
            slot = inven->insertItemIntoInventory(item.m_item, (eItemAddReason)4, true, true);
        }

        if (slot == -1)
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)4);
            user->Send(packet);
            return 0;
        }

        if (citem->IsRandomOption())
        {
            statistc_proxy::add(
                targetUser, "log_random_option", "pick_cnt", 1, "%d,'%s'",
                targetUser->get_charac_10_level_section(),
                GlobalData::s_systemTime_.getCurDate());
        }

        if (!m_stage.pickupItem(idx, item))
        {
            packet.makeCommonError((ENUM_CMDPACKET)0x2e, (ENUM_ERROR)0x15);
            user->Send(packet);
            return 0;
        }

        packet.makePickupItemHeader(user->get_unique_id(), (unsigned short)idx);
        packet.appendPickupItem((char*)&nameBuf, targetUser->get_unique_id(),
                                slot, itemType);
    }

    m_party->send_to_party(packet);
    return 1;
}