TowerOfDespairMgr::TowerOfDespairMgr()
{
    // [b28d-solo-CtorProducers_1 判词·P70/P253+P171] ORIG 0x08643ac4：旧「N78 memset
    // 内联封顶」判词作废。ORIG 逐条：set C1(+0x328) → vector C1(+0x340)（成员声明序
    // 自动）→ `movw $0,0x34c` → `movw $0,0x34e` → `movl $0,0x358` →
    // **`std::fill<const CDungeon**, const CDungeon*>`(this, this+0x194, &nullDungeon)** →
    // **`std::fill<unsigned*,int>`(this+0x194, this+0x328, &nullIndex)** →
    // `operator new(0x18)`+APC_Mgr C1 → store +0x354。
    // ⇒ 数组清零是具名零局部的 `std::fill` 调用（Ghidra C 段同证），非 memset；
    // 且 ORIG **不写** m_topLayer(+0x350)（P171 照抄不写，补写即多 2 条）。
    // [b28d-S435 修复·T1 ⇒ p167 CLEAR] ORIG 0x08643b33
    // `call _ZSt4fillIPPK8CDungeonS2_EvT_S4_RKT0_`（std::fill<const CDungeon**,
    // const CDungeon*>）⇒ 头 m_layerDungeons 改 `const CDungeon*[101]`（Main 代落）
    // + 本处 nullDungeon 改 const 指针；p167 由 MISSING/EXTRA 各 1 → CLEAR。
    // 残余：new APC_Mgr 的 landing pad（ORIG 亦布，故此处一致）。
    m_enterableCount = 0;
    m_minLevel = 0;
    m_field358 = 0;
    // [b28d-solo-CtorProducers_1 判词·P309] ORIG 0x08643ac4 槽序：
    // nullDungeon=-0x20（深）/ nullIndex=-0x1c（浅）。4.4 对函数头声明按
    // 「先声明浅」分配（与 4.1 相反）⇒ 全头部声明、声明序按 ORIG 槽浅→深
    // （先 nullIndex 后 nullDungeon），赋值（清零）位置保持原位。
    // [b28d-S435 修复·T1] 头 TowerOfDespairMgr.h m_layerDungeons 已改 const CDungeon*：
    // ORIG 0x08643b33 `call _ZSt4fillIPPK8CDungeonS2_EvT_S4_RKT0_`
    // （std::fill<const CDungeon**, const CDungeon*>）⇒ 零局部也须 const 指针。
    int nullIndex;
    const CDungeon* nullDungeon;
    nullDungeon = 0;
    std::fill(m_layerDungeons, m_layerDungeons + 101, nullDungeon);
    nullIndex = 0;
    std::fill(m_dungeonIndexOfLayer, m_dungeonIndexOfLayer + 101, nullIndex);
    m_apcMgr = new TowerOfDespair_APC_Mgr;
}