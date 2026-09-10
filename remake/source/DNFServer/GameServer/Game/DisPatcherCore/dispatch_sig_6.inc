int DisPatcher_RentAvatar::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [H5DisPatcher 核账 2026-09-11] 本轮改动实测 ours 484→491（+7）劣化 ⇒ 已回滚留 N
    // [DPC_SigA 已排除] P312/P78：ORIG 0x081ee5ae/bc 与 0x081ee7d7..827 共 5 处内联
    // `rep stos`，本 TU 4.4.7 路由下 `= {0}` 改写实测劣化（帧/对齐副作用 > rep stos 收益）
    int unused0[10];                    // -0xa8（ORIG rep stos 0xa 清零，后未使用）
    STRentAvatarReq rentEntries;       // ORIG -0x188(%ebp) 起载荷区（逐 entry stride 5）
    int itemPtrs[10];                   // -0xd0（find_item 结果）
    int cashPrices[10];                 // -0xf8（GetCashPrice 租价）
    int goldPrices[10];                 // -0x148（get_price 租价）
    int winPrices[10];                  // -0x120（GetMedalPrice 租价）
    int totalCash;                      // -0x24
    int totalGold;                      // -0x20
    int totalWin;                       // -0x1c
    int i;                              // -0x30（循环游标）
    int rentDays;                       // -0x2c
    int rentSeconds;                    // -0x28
    char itemIdStr[0x14];               // -0x54
    char priceStr[0x24];                // -0x80

    memset(unused0, 0, sizeof(unused0));
    totalCash = 0;
    rentDays = 0;
    rentSeconds = 0;

    PacketGuard packetGuard;            // -0x40
    if (packet.get_byte(rentEntries.m_count) != 1)
        return LineFunc(0x71ee, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x081ee628/0x081ee633：计数无独立局部，两处判定各自 movzbl -0x17b(%ebp)
    if (rentEntries.m_count == 0 || 9 < rentEntries.m_count)
        return LineFunc(0x71f0, __PRETTY_FUNCTION__, 0, 0);

    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        if (packet.get_byte(rentEntries.m_entries[i].m_rentDaysType) != 1)
            return LineFunc(0x71f4, __PRETTY_FUNCTION__, 0, 0);
        if (packet.get_int(rentEntries.m_entries[i].m_itemId) != 1)
            return LineFunc(0x71f5, __PRETTY_FUNCTION__, 0, 0);
    }

    if (user->getCurCharacR() == 0)
        return LineFunc(0x71fa, __PRETTY_FUNCTION__, 0, 0);

    if (user->getCurCharacInvenR()->GetRemainCapacity(
            (INVEN_TYPE)2, Inven_Item::ITEM_TYPE_0) <
        (int)(unsigned int)rentEntries.m_count)
    {
        RentAvatarErrorProc(user, 4);
        return 0;
    }

    memset(itemPtrs, 0, sizeof(itemPtrs));
    memset(cashPrices, 0, sizeof(cashPrices));
    memset(goldPrices, 0, sizeof(goldPrices));
    memset(winPrices, 0, sizeof(winPrices));
    totalCash = 0;
    totalGold = 0;
    totalWin = 0;
    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        itemPtrs[i] = (int)(long)G_CDataManager()->find_item(
            (int)rentEntries.m_entries[i].m_itemId);
        if (itemPtrs[i] == 0)
        {
            RentAvatarErrorProc(user, 0x11);
            return 0;
        }
        cashPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetCashPrice(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
        goldPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->get_price(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
        winPrices[i] = getAvatarRentPrice(
            ((CItem*)itemPtrs[i])->GetMedalPrice(),
            (unsigned int)rentEntries.m_entries[i].m_rentDaysType);
        totalCash += cashPrices[i];
        totalGold += goldPrices[i];
        totalWin += winPrices[i];
    }

    if ((unsigned int)user->GetCera() < (unsigned int)totalCash ||
        user->getCurCharacInvenR()->get_money() < totalGold ||
        (unsigned int)user->GetWinPoint() < (unsigned int)totalWin)
    {
        RentAvatarErrorProc(user, 10);
        return 0;
    }

    for (i = 0; i < (int)(unsigned int)rentEntries.m_count; ++i)
    {
        if (rentEntries.m_entries[i].m_rentDaysType == 1)
            rentDays = 0x1e;
        else if (rentEntries.m_entries[i].m_rentDaysType == 2)
            rentDays = 0x16d;
        else if (rentEntries.m_entries[i].m_rentDaysType != 0)
        {
            RentAvatarErrorProc(user, 1);
            return 0;
        }
        else
            rentDays = 7;
        rentSeconds = rentDays * 0x15180;

        CStreamGuard streamGuard(
            GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp",
                                               0x7239),
            false);                     // -0x5c
        streamGuard.operator*()->operator<<(cashPrices[i]);
        streamGuard.operator*()->operator<<(winPrices[i]);
        streamGuard.operator*()->operator<<(goldPrices[i]);
        // ORIG 0x081eeb65 call _ZN12CStreamGuardlsEm（unsigned long 重载）；
        // m_itemId 本即 unsigned long，多余的 (int) 强转会错选 lsEi。
        streamGuard.operator*()->operator<<(rentEntries.m_entries[i].m_itemId);
        streamGuard.operator*()->operator<<(rentDays);
        streamGuard.operator*()->operator<<(rentSeconds);
        streamGuard.operator*()->operator<<((int)(unsigned int)rentEntries.m_count);
        streamGuard.operator*()->operator<<(i + 1);
        OS_API::snprintf(itemIdStr, 0x14, "%d",
                         (int)rentEntries.m_entries[i].m_itemId);
        OS_API::snprintf(priceStr, 0x24, "%d", cashPrices[i]);
        GlobalData::s_pIPGHelper->IPGOutput(
            user, "", priceStr, itemIdStr, (char const*)0, (char const*)0,
            streamGuard.Get());
    }

    return 0;
}
