// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08635xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N4/N7 封顶：94/97（原 94/100）。
// 修法：①P183 删 `CBattle_Field* field = &party->m_battleField` 缓存（ORIG 每站点
// 重发 `mov -0x10(%ebp),%eax; add $0xb24,%eax`）；②get_blood_dungeon_type() 外联
// call 改直读 m_bloodDungeonType（ORIG `movzbl 0x89f(%eax),%eax`）。
// 残差 = sendUltimateRandomDifficulty 的 `add $0xb24` vs 4.4 `lea 0xb24` + 实参
// 装载序（P30/N4）+ 条件物化 ⇒ N4/N7。
bool TimerUltimateNextRoundStart::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
    {
        return 0;
    }
    // 真语义 bug 修正（Ghidra C：body 条件 = m_pDungeon==0 || type==2）⇒ 早退条件
    // 必须写成 De Morgan 正形 `m_pDungeon != 0 && type != 2`（旧码 `==0 || !=2` 极性错）。
    if (party->m_battleField.m_pDungeon != 0 &&
        party->m_battleField.m_pDungeon->m_bloodDungeonType != 2)
    {
        return 0;
    }
    // P183 地址重物化铁律：ORIG 每个站点重发 `mov -0x10(%ebp),%eax; add $0xb24,%eax`
    // （08638dfb/08638e2f/08638e5c 等），禁缓存 `CBattle_Field* field = &party->m_battleField`。
    if (party->m_battleField.getUltimateDifficylty() != 0)
    {
        return 0;
    }
    const int diff = party->m_battleField.getNextRoundRandomDifficulty();
    party->m_battleField.setUltimateDifficulty(static_cast<char>(diff));
    // P48 no-CSE：ORIG 两次 fresh getBloodRoundOrder()（比较用 + send 实参用）
    if (static_cast<short>(party->m_battleField.m_bloodRound.getBloodRoundOrder()) !=
        party->m_battleField.getBloodMaxRound() - 1)
    {
        party->m_battleField.sendUltimateRandomDifficulty(
            diff, static_cast<short>(party->m_battleField.m_bloodRound.getBloodRoundOrder()));
    }
    party->m_battleField.startUltimateRound();
    return 1;
}