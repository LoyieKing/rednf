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
        if (static_cast<unsigned int>(gold * 10) <
            static_cast<unsigned int>(m_rand->randInt(range = m_randRange)))
        {
            return false;
        }
    }
    item->m_upgradeSep.IncUpgradeSeparate();
    return true;
