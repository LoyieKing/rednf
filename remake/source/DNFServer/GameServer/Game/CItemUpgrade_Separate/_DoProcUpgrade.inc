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
    // [g14 试形 W3 2026-09-11] ORIG 0x0811e4ce/0x0811e508 为「短路 && 正极性物化」：
    //   `cmpl $0,-0xc; je <mov $0 岛>` … `cmp %eax,%ebx; jae <mov $0 岛>; mov $1; jmp join;
    //    <岛> mov $0; join: test %al; je <体>; mov $0(ret false)`。故写单表达式正极性
    //   短路 `(gold != 0) && (gold*10 < randInt())` 作 `if` 条件（P214① 正向形）。
    //   ⇒ 一次命中：DIFF(69/70, gap −1) → **AE(69/69, gap 0)**；旧判词
    //     「N-88/P197④ setcc-fold 不可造」（基于具名 bool/register/P225 while 三形）
    //     被本形推翻——原判遗漏了「无具名局部 ⇒ 短路直接作 if 条件」这一源形。
    unsigned long range;
    if (gold != 0 &&
        static_cast<unsigned int>(gold * 10) <
            static_cast<unsigned int>(m_rand->randInt(range = m_randRange)))
    {
        return false;
    }
    item->m_upgradeSep.IncUpgradeSeparate();
    return true;
}