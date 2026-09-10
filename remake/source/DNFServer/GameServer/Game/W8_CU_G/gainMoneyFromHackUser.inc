void CUser::gainMoneyFromHackUser(unsigned int who, unsigned long long gold,
                                  unsigned int cnt, unsigned long long totalGold)
{
    if (!isLoadingHackGold())
    {
        setLoadHackGold(true);
        DB_AutoPunishedSecondUserData::makeRequest(get_acc_id(), gold, GetUID(), who);
        return;
    }

    // b28d-solo-W8_CU_G：ORIG 0x0867fc3f 先载 totalGold(-0x18/-0x14) 入 eax:edx、
    // 后载 gold 入 ecx:ebx ⇒ 4.4 先求右操作数，写 `gold + totalGold`（加法交换律）。
    incTotalHackUserTradeGold(gold + totalGold);
    incTotalHackUserTradeCnt(cnt);

    // b28d-solo-W8_CU_G：ORIG 0x0867fc7f/0x0867fc8e 两 getter 结果全程寄存器
    //（ebx:esi / eax，无槽往返，P0）⇒ 内联实参，勿声明 totalNow/cntNow 具名局部。
    if (GlobalData::g_autoPunishRuleHackTypeMgr->checkTotalTradeRuleOver(
            static_cast<unsigned int>(getTotalHackUserTradeCnt()),
            getTotalHackUserTradeGold()))
    {
        SetTradePunishType(8);
    }
    // b28d-solo-W8_CU_G 真语义修正：ORIG 0x0867fce6 末尾 makeRequest 第 3 参读
    // -0x10/-0xc（形参 gold 槽），**非** getTotalHackUserTradeGold() 结果
    //（该值仅用于 checkTotalTradeRuleOver，ebx:esi 之后即弃）。原码传 totalNow
    // 属参数传错，已改正为 gold。
    DB_InsertAutoPunishSecondUser::makeRequest(
        get_acc_id(), who, gold, isTradePunishType(8));
