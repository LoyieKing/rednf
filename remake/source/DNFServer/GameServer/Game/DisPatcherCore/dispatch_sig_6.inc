// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 491（ORIG 480 / ours 487），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] callee 重载修正：m_itemId 本即 unsigned long，多余的 (int) 强转错绑
// _ZN12CStreamGuardlsEi；ORIG 0x081eeb65 实为 lsEm。实测 ndiff 345→342，差集一缺一多清零。
// 残差 5 处 memset→rep stos 属 N78（4.1 内联展开、4.4 O0 恒 call memset，不可翻）。
int DisPatcher_RentAvatar::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // [H5DisPatcher 核账 2026-09-11] 本轮改动实测 ours 484→491（+7）劣化 ⇒ 已回滚留 N
    // [Q5BigTURest.DisPatcherCore 2026-09-11 落地·P312 重测正结论] ORIG
    // 0x081ee5ae/bc（unused0）与 0x081ee7d7..0x827（itemPtrs/cashPrices/winPrices/
    // goldPrices）共 5 处为**内联** `lea -0xNN,%ebx; mov $0x0,%eax; mov $0xa,%edx;
    // mov %ebx,%edi; mov %edx,%ecx; rep stos`（memset 展开），我方原 5 条
    // `memset(dst,0,sizeof)` 恒产 `call memset`（p167fix 直证 EXTRA memset×5）。
    // 本轮把「声明处的 `= {0}`」与「使用点声明」两轴一起用（声明下移到原 memset 处）：
    // rep stos ×5 全部复现（p167 EXTRA memset 5→0），本件 verdict 仍 DIFF 但
    // gap +9→+2（orig 480 / our 478），TU 四数零劣化（215/666/2/49 → 215/667/2/48
    // 中的 ae/diff 变动来自同轮 CParty.h 返回类型轴）。
    int unused0[10] = {0};              // -0xa8（ORIG rep stos 0xa 清零，后未使用）
    STRentAvatarReq rentEntries;       // ORIG -0x188(%ebp) 起载荷区（逐 entry stride 5）
    int totalCash;                      // -0x24
    int totalGold;                      // -0x20
    int totalWin;                       // -0x1c
    int i;                              // -0x30（循环游标）
    int rentDays;                       // -0x2c
    int rentSeconds;                    // -0x28
    char itemIdStr[0x14];               // -0x54
    char priceStr[0x24];                // -0x80

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

    int itemPtrs[10] = {0};             // -0xd0（find_item 结果）
    int cashPrices[10] = {0};           // -0xf8（GetCashPrice 租价）
    int goldPrices[10] = {0};           // -0x148（get_price 租价）
    int winPrices[10] = {0};            // -0x120（GetMedalPrice 租价）
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
        // [P5-DPC 2026-09-11] P251/P261 真 switch 轴：ORIG 0x081eea45..0x081eea92 为 -O0
        // switch 决策树（`movzbl 0xe(%eax),%eax; movzbl %al,%eax`；`cmp $0x1,%eax; je
        // <0x1e>`；`cmp $0x2,%eax; je <0x16d>`；`test %eax,%eax; jne <err>`；case 0 落
        // `movl $0x7,-0x2c(%ebp); jmp`）⇒ 原码为 case 0/1/2 的 switch（default=错误路），
        // 我方旧 if/else-if 产不出该树（diff at orig[296..] 段）。
        switch (rentEntries.m_entries[i].m_rentDaysType)
        {
        case 0:
            rentDays = 7;
            break;
        case 1:
            rentDays = 0x1e;
            break;
        case 2:
            rentDays = 0x16d;
            break;
        default:
            RentAvatarErrorProc(user, 1);
            return 0;
        }
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
