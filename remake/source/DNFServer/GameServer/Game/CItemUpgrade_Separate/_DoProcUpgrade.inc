// ORIG T 0x0811e468。
// [b28d-solo-CItemUpgrade_Separate] 判词 N（ORIG 0x0811e468，setcc-fold/N2 族；p167
// 6=6 CLEAR）：形 k
// （探针 /tmp/b28d_51/p9.cpp k）复现 gold@-0xc、limit@%ebx 跨 randInt 保活、range@-0x10、
// 「limit 先求值→range 内联赋值→randInt」求值序、push %ebx+sub $0x24 帧。残余 5 条：
// ORIG 的 failed 为 4.1 寄存器化分支赋值（`jae;mov $1,%eax;jmp;mov $0,%eax;test %al,%al;je`），
// 4.4 对 `<` 恒 setcc（setb+test+je）且具名 bool 恒槽化（register/无初始化/两分支赋值
// p/q/m 三形实测：setb+分支写槽或 setb %bl），跨版本后端差不可造。语义/调用集齐。
// [2026-09-09 SweepCItemUpgrade] 复核（未改）：gap=3（orig 69 / our 66）即该 setcc-fold
// 段（ORIG 0x0811e508 `cmp; jae <false岛>; mov $1,%eax; jmp; mov $0,%eax; test; je`
// 6 条 vs 4.4 `cmp; setb %al; test; je` 3 条）。判词维持 N。
bool CItemUpgrade_Separate::_DoProcUpgrade(CUser* user, Inven_Item* item,
                                           const STUpgradeInfo* info)
{
    if (user->IsGameMasterMode())
    {
        if (user->getUpgradeSeparateTrueFlag() == 1)
        {
            item->m_upgradeSep.IncUpgradeSeparate();
            return true;
        }
        if (user->getUpgradeSeparateFlaseFlag() == 1)
        {
            return false;
        }
    }

    const int gold = info->m_gold;
    if (gold != 0)
    {
        unsigned long range;
        // [round5-C05 2026-09-10 C05④ 复验未命中，已回退] ORIG 0x0811e508
        // `cmp; jae <岛>; mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`（寄存器化
        // bool 赋值）vs 4.4 `cmp; setb %al; test; je`。按 C05④ 四步法步骤 2 试
        // **P225 原形**（body 以 `return false;` 终止 ⇒ 满足「无回边」前置）：
        // `while (a < b) { return false; }` 实测读数**逐值不变**（我们 66 / ORIG 69，
        // 仍 setb）⇒ 步骤 4 封顶 N-88/P197④（勿再动控制流、勿动 callee）。
        // [LaneA P346 变体试形] 具名 bool 局部（复现 ORIG 0x0811e508 mov$1/mov$0 岛）
        bool overLimit =
            static_cast<unsigned int>(gold * 10) >=
            static_cast<unsigned int>(m_rand->randInt(range = m_randRange));
        if (!overLimit)
        {
            return false;
        }
    }
    item->m_upgradeSep.IncUpgradeSeparate();
    return true;
}