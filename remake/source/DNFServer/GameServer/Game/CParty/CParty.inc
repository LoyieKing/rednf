// ============================================================================
// CParty 构造 / 析构
// ============================================================================
// b28d-solo-CParty: N 封顶 —— N82 帧 0x20/0x4c：成员构造序与 pad 类型化（CBattle_Field/BattleData 内嵌）待 Main 裁决。
// [SweepCParty2 2026-09-09] 真 bug 修复：删 m_battleField 二次构造 ⇒ 308→287（ORIG 220，
// 剩余 gap 67 = N83 placement-new 前端内联 vs 4.4 extern _ZnwjPv（7 处 pad 区）+ 构造序差）。
CParty::CParty()
{
    // b28d-49 判词 + [SweepCParty2]：P167 的 C2/D2 "MISS" 为 C1/C2 同址别名（P93），非缺口；
    // EXTRA _ZnwjPv/_ZdlPvS_ = N83（4.1 placement-new 前端内联、4.4 恒 extern call）。
    // pad 区真实成员化需改 CParty.h 成员并同步 CPartyDeps*/OnlyOrig/Link2/DisPatcherCore 等
    // 消费 TU 的字段名 ⇒ 超本泳道写权，按 N83 封顶。@0x82a5516
    // [b28d-solo-CParty2] 7 个 pad 区已字段化为真实成员（CParty.h），
    // placement-new 手写构造全部删除：编译器在成员构造阶段按声明序自动
    // 调用各子对象 C1（与 ORIG C1 的 C2Ev 同址别名，P93）。
    for (int i = 0xb; i <= 0x2f; ++i)
        m_field148[i - 0xb] = 0;
    m_usedCoinCount = 0;
    reinterpret_cast<CBattle_Field*>(&m_battleField)->SetParty(this);
    SetAssaultState(0);
    m_routingData.Reset();
    m_teleport.init(this);
    m_randomBuffType = (QuickParty::RandomBuffType)0xb;
    m_isQuickParty = 0;
    init_quick_party_data();
    m_secu.Init(this);
    m_partyMemberCoinLimit = 0;
}