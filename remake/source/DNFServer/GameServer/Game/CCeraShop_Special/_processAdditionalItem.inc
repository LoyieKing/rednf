// ============================================================================
// _processAdditionalItem —— ORIG 0x083267a2 (T)
// 结构（报告 /tmp/pai_report.md 全量逐块对照）：
//   getCurCharacR()==0 → return；itemIdx==1 → _processCoin + Inven_Item + push_back。
//   否则 errIdx 局部恒 -1 → ORIG -O0 下 `if (errIdx != -1)` 为死块（P184 照抄：
//   两枚 PacketGuard 发包 + Inven_Item 推入），else 主路径：
//   find_item；可叠特殊物品（type 0x10/0x22）→ insertItemIntoInventory/邮件兜底+push_back；
//   一般路径：agency 清零 + genIPGNo(头像) + Inven_Item 组装 + SetUpgrade(flag/五值) +
//   autoUnsealItem + 计数循环（头像 AddAvatarItem/邮件；造物 _processCreature；
//   否则 _processItem（0xc1 时前置双 find + 嵌套发放 0x10 count=0xc352））。
// 【§6.2 验收标注】orig=765 our=732；P167 调用点差集归零（仅余 N78 两处
// rep stos→memset call：nameBuf 0x200 / agency 0x18）；其余 |gap| 为 N82 帧
// 槽号 + P184 死块 rep 段形态残余 ⇒ 语义完成、byte 层 N 封顶。
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x083267a2 orig/ours=763/730 ndiff=702。
// 巨件帧槽（N82）+ N78 rep stos→memset call 两处（nameBuf 0x200/agency 0x18）；
// P167 调用点差集已归零。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82] 763/765；P167 CLEAR 75/75（memset→{0} 消 N78 两处），余帧 0x47c/0x46c
void CCeraShop::_processAdditionalItem(CUser* user, unsigned long itemIdx,
                                       unsigned short slot, unsigned char a,
                                       unsigned char b, std::vector<Inven_Item>& items,
                                       stCeraShopItemParam_t& param, unsigned char flag)
{
    if (user->getCurCharacR() == 0)
        return;

    if (itemIdx == 1)
    {
        _processCoin(user, slot, false);
        Inven_Item coin;
        coin.m_addInfo = 1;
        coin.m_addInfo2 = (int)slot;
        items.push_back(coin);
        return;
    }

    int notifySize = 0;      // ORIG -0x5c
    int notifyCount = 0;     // ORIG -0x58
    int errIdx = -1;         // ORIG -0x54；-O0 下恒 -1 → 下方 if 为死块（P184）
    // P214③：ORIG 对两处缓冲用 rep stos 内联清零（N78 突破口）⇒ 显式 {0} 零初始化。
    char nameBuf[0x200] = {0};     // ORIG -0x3ed
    int nameLen = 0;         // ORIG -0x50

    if (errIdx != -1)
    {
        // —— -O0 静态不可达块：照抄 ORIG 0x134..0x303 发射序 ——
        PacketGuard notify;
        notify.put_header(1, notifySize);
        notify.put_byte(1);
        notify.put_int(notifyCount);
        notify.finalize(true);
        user->Send(notify);

        Inven_Item errItem;
        errItem.m_addInfo = errIdx;
        errItem.m_addInfo2 = (int)slot;
        items.push_back(errItem);

        PacketGuard errorMsg;
        errorMsg.put_header(0, 0xc);
        errorMsg.put_byte(0);
        errorMsg.put_short(0);
        errorMsg.put_byte(0);
        errorMsg.put_int(nameLen);
        errorMsg.put_str(nameBuf, nameLen);
        errorMsg.finalize(true);
        user->Send(errorMsg);
    }
    else
    {
        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return;

        // 可叠特殊物品：is_stackable && GetItemType() ∈ {0x10, 0x22}
        // （ORIG 短路形：非 stackable 直落 false 岛；两次 GetItemType 独立求值 P48）
        if (item->is_stackable() &&
            (item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        {
            int resSlot = -1;                // ORIG 先写 -0x44=-1 再组装
            Inven_Item invItem;
            invItem.m_addInfo = (int)itemIdx;
            invItem.m_addInfo2 = (int)slot;
            item->make_item(invItem);
            resSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
                invItem, (eItemAddReason)0xf, true, false);
            if (resSlot != -1)
                user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, resSlot);
            else
                CMailBoxHelper::ReqDBSendNewMailCashShop(
                    user, invItem, 0, user->getCurCharacNo(), true, 0, 0);
            items.push_back(invItem);
            return;
        }

        // 一般路径
        unsigned int count = slot;
        if (item->is_stackable())
            count = 1;

        char agency[0x18] = {0};
        if (item->IsAvatarItem())
        {
            unsigned int characNo = (unsigned int)user->getCurCharacNo();
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)3, characNo, agency);
        }

        Inven_Item invItem;
        invItem.m_addInfo = (int)itemIdx;
        invItem.m_addInfo2 = (int)slot;
        item->make_item(invItem);

        if (flag != 0)
            invItem.SetUpgrade(flag);
        if (invItem.m_addInfo == 0x71db || invItem.m_addInfo == 0x72d0 ||
            invItem.m_addInfo == 0x742d || invItem.m_addInfo == 0x7556 ||
            invItem.m_addInfo == 0x7661)
            invItem.SetUpgrade(0xc);

        autoUnsealItem(item, invItem);

        int accCera = 0;  // ORIG -0x78 累积出参（_processItem/_processCreature 共用）

        for (int i = 0; i < (int)count; ++i)
        {
            if (!item->IsAvatarItem())
            {
                if (!item->IsCreatureItem())
                {
                    long curSec = GlobalData::s_systemTime_.getCurSec();
                    _processItem(user, invItem, 0, accCera, curSec, true, false,
                                 items, param);
                    // ORIG 双 find_item 先于条件（0xa5e..0xa76），0xc1 判定在后。
                    CItem* found = G_CDataManager()->find_item(invItem.m_addInfo);
                    CItem* item10 = G_CDataManager()->find_item(0x10);  // ORIG 无空判
                    if (invItem.m_addInfo == 0xc1 && found != 0)
                    {
                        std::vector<Inven_Item> nestedItems;
                        stCeraShopItemParam_t nestedParam;
                        int nestedRestrict = 0;
                        Inven_Item nestedItem;
                        item10->make_item(nestedItem);
                        nestedItem.m_addInfo = 0x10;
                        nestedItem.m_addInfo2 = 1;
                        _processItem(user, nestedItem, 0xc352, nestedRestrict, curSec,
                                     false, true, nestedItems, nestedParam);
                    }
                }
                else
                {
                    int creatureSlot = 0;  // ORIG -0x94 出参
                    _processCreature(user, invItem, creatureSlot, accCera, false,
                                     item->getUsablePeriod(),
                                     item->getExpirationDate());
                }
            }
            else
            {
                int avatarPeriod = ((CEquipItem*)item)->getAvatarPeriod(b);
                int avSlot = user->getCurCharacInvenW()->AddAvatarItem(
                    (int)itemIdx, avatarPeriod, false, (char)a, (char)b, agency,
                    eAvatarItemAddReason_0, false, 0);
                if (avSlot >= 0)
                    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1,
                                             avSlot);
                else
                {
                    int expireDate = 0;
                    if (avatarPeriod != 0 && avatarPeriod != 0x16d)
                    {
                        // ORIG 0x08326f13..0x08326f2a：单表达式（call getCurSec →
                        // `mov -0x38,%edx; imul $0x15180,%edx,%edx; add %edx,%eax` →
                        // 存槽）；「赋值再 +=」会多发 mov+内存 add。
                        expireDate = GlobalData::s_systemTime_.getCurSec() +
                                     avatarPeriod * 0x15180;
                    }
                    const char* msg = g_scriptStringManager_.findString(
                        4, "game_server_msg_104", (bool*)0);
                    int msgLen = std::strlen(msg);
                    int characNo = user->getCurCharacNo();
                    CMailBoxHelper::ReqDBSendNewAvatarMail(
                        user, characNo, (int)itemIdx, expireDate, (char)a, (char)b,
                        agency, msg, msgLen);
                }
                user->SendAradEventItemLog(itemIdx, 1);
            }
            if (!item->is_stackable())
            {
                items.push_back(invItem);
            }
            else
            {
                if (!_isPackageItem(item))
                    items.push_back(invItem);
            }
        }
    }
}