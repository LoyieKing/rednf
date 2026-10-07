// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x08228e5e orig/ours=499/500 ndiff-blocks=47
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 6 个，4.4 libstdc++ 全内联不可复现
//   首差: sub    $0xcc,%esp | movw   $0x0,-0x36(%ebp) | lea    -0x36(%ebp),%eax
int Dispatcher_UnsealRandomOption::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08228e5e：随机选项解封（金币/卷轴双通道）。
    // [G5DsigE·语义轴] ORIG 0x08228e5e 直证无 user 判空（首 6 条即 push/push/sub/mov/
    //   mov/call，直接取 user->get_state()）；我方 `user == 0` 为发明校验，删除。
    if (user->get_state() < 3 || user->getCurCharacR() == 0)

        return 0;
    if (user->CheckInTrade())
        return 0;

    // [g01 2026-09-11] ORIG `movw $0x0,-0x36` 与 `movw $0x0,-0x38` 分落两个 get_short 之前
    //   （非合并初始化）⇒ 两声明各自紧贴其 get_short（P212 声明位置）。
    short slotA = 0;                       // 目标装备槽位（ORIG get_short(short&)）
    if (!packet.get_short(slotA))
        return LineFunc(0xe67c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned short paySlot = 0;            // 支付槽位（0xffff=金币，ORIG get_short(unsigned short&)）
    if (!packet.get_short(paySlot))
        return LineFunc(0xe681,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((int)user->CheckItemLock(1, (int)slotA) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0xd5);
        return 0;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* item = invenW->GetInvenRef(1, (int)slotA);
    if (item == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    CItem* proto = G_CDataManager()->find_item(item->m_addInfo);
    if (proto == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    const bool unlockGateFail =
        !(proto->get_rarity() < 3 && item->GetUpgrade() < 0xb);
    if (unlockGateFail)
    {
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x25);
        if (secuErr != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                     (unsigned char)(secuErr & 0xff));
            return 0;
        }
    }

    if (!item->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    int checkErr = _check_unseal_scroll_item(user, item, paySlot);
    if (checkErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                 (unsigned char)(checkErr & 0xff));
        return 0;
    }

    // key 打包：rarity@b0 | optionGrade@b1 | usableLevel@b2..3（ORIG 帧连续视图）。
    // [G1-Dispatchers] P0/P48：ORIG 调用序列（本项目逐 call 对齐）为
    //   get_rarity → getUsableLevel → G_CDataManager → map::find → G_CDataManager → map::end
    // ⇒ key 处不设 rarity/usableLevel 具名局部，且 G_CDataManager() 在 find/end 各取一次
    //   （不得用 `priceTable` 引用缓存；p167 ORIG-only G_CDataManager x1、我方 EXTRA 引用槽）。
    const unsigned char optionGrade =
        (unsigned char)proto->GetRandomOptionGrade();
    const int key = proto->get_rarity() | (optionGrade << 8) | ((int)proto->getUsableLevel() << 16);
    std::map<int, int>::const_iterator found =
        G_CDataManager()->m_randomOption.m_map178.find(key);
    if (found == G_CDataManager()->m_randomOption.m_map178.end())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }

    // [G1-Dispatchers] 分支物理序按 ORIG 调用序列定：`getCurCharacInvenR → GetInvenSlot`
    // （scroll 段）出现在 `getCurCharacInvenR → get_money`（gold 段）之前 ⇒ 源码把
    // 「非 0xffff（卷轴）」写在 if 直落侧（P47/P194 块序）。
    if (paySlot != 0xffff)
    {
        Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(1, paySlot);
        if (scroll.m_addInfo < 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
    }
    else
    {
        const int goldCost = found->second;
        if (user->getCurCharacInvenR()->get_money() < goldCost)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }

    RandomOption* opts = &item->m_random.m_random;
    // ORIG 40..42：give_option 实参位重取 getItemGroupName/getUsableLevel/get_rarity
    // （不缓存首值）⇒ 实参内联，禁止复用 key 段的值。
    const bool gave = GlobalData::g_randomOptionHandle->give_option(
        (unsigned long)item->m_addInfo, (ENUM_RARITY)proto->get_rarity(),
        (int)proto->getUsableLevel(), proto->getItemGroupName(),
        (int)proto->GetRandomOptionGrade(), opts);
    if (!gave)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    // [bughunt9] ORIG 0x0822937b..0x08229535：give_option 成功后**无条件**先记
    //   statistc_proxy::add("log_random_option","unseal_cnt",1,...)（0x082293c2），
    //   再 RandomOption(*item, 0)（0x082293d0 `movl $0x0,0x8(%esp)`，非 T_REGENERATION=1），
    //   然后才按 paySlot 分支：卷轴 use_item+SendUpdateItem / 金币 "unseal_gold"+use_money。
    //   旧写把 unseal_cnt 收进卷轴分支（金币解封漏记）且 reason 记成 1。
    statistc_proxy::add(user, "log_random_option", "unseal_cnt", 1u,
                        "%d,'%s'", user->get_charac_10_level_section(),
                        GlobalData::s_systemTime_.getCurDate());
    user->m_historyLog.RandomOption(*item, (randomOptionReason::T)0);
    if (paySlot != 0xffff)
    {
        if (user->getCurCharacInvenW()->use_item((int)paySlot, 1, 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)paySlot);
    }
    else
    {
        statistc_proxy::add(user, "log_random_option", "unseal_gold",
                            (unsigned int)found->second, "%d,'%s'",
                            user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        if (user->getCurCharacInvenW()->use_money(found->second,
                                                  (eMoneySubReason)0x2c, true) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x191);
    PacketGuard packetOut;
    packetOut.put_header(0, 0xe);
    packetOut.put_byte(0);
    packetOut.put_short(1);   // ORIG put_short(1)（NEW 缺此字段，回包格式差异）
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, (int)slotA,
                                               packetOut);
    packetOut.finalize(true);
    user->Send(packetOut);
    return 0;
}