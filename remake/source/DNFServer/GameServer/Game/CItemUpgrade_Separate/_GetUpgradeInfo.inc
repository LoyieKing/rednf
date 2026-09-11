// ORIG T 0x0811e2e2。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0811e2e2，setcc-fold + N7 族）：
// 本轮复核（未改）：gap=5（orig 121 / our 116）全部为跨版本形态差——
// ①ORIG 条件物化 `jg <主体>; mov $0,%eax; jmp; mov $1,%eax; test %al,%al; je`
//   （0x0811e303..0x0811e321，6 条）vs 4.4 `setle %al; test; je`（3 条）⇒ setcc-fold；
// ②get_rarity→%ebx / 虚调 GetItemType→%esi 与 4.4 的 %esi/%ebx 相反（N7，P230 判
//   4.4.6 后端敏感族）。p167 11=11 CLEAR（0 missing/0 extra），帧 push esi/ebx +
// sub $0x50 与 ORIG 全等；语义/调用集齐，无源码可翻。
bool CItemUpgrade_Separate::_GetUpgradeInfo(const Inven_Item* item, const CItem* citem,
                                            const GenuineDamageUpgradeTable& table,
                                            STUpgradeInfo* out, int level)
{
    // [2026-09-10 SweepCEquipItem2] 真 bug 修（条件极性反写）：ORIG 0x0811e301
    // `cmp %eax,%edx; jg 0811e31a`，且 Ghidra C 独立同证
    // `if (*(int *)(param_3 + 4) < (int)(uVar3 & 0xff)) { uVar4 = 0; } else { …主体… }`
    // ⇒ **sepLv > m_maxLevel 才 return false**；sepLv <= m_maxLevel 路径继续主体，
    // 并在主体前重发一次 GetUpgradeSeparate（0x0811e30e，结果丢弃，P48 无 CSE）。
    // 旧译文写成 `if (!(sepLv > m_maxLevel)) { 死调用; return false; }`，极性完全相反
    // （正常强化恒失败、超上限反而放行）。
    if (static_cast<int>(item->m_upgradeSep.GetUpgradeSeparate()) > table.m_maxLevel)
    {
        return false;
    }
    (void)item->m_upgradeSep.GetUpgradeSeparate();   // ORIG 0x0811e30e 死调用（结果丢弃）

    const unsigned char growth =
        static_cast<unsigned char>(
            reinterpret_cast<const CEquipItem*>(citem)->getGrowthGrade(level));
    // ORIG 0x0811e34d/0x0811e35f：rarity→%ebx、itemType→%esi 内联实参（无具名槽）。
    const int materialCnt = table.getGenuineUpgradeMaterialCnt(
        growth,
        static_cast<int>(static_cast<unsigned char>(
            item->m_upgradeSep.GetUpgradeSeparate())),
        static_cast<ENUM_EQUIPMENTTYPE>(citem->GetItemType()),
        static_cast<ENUM_RARITY>(citem->get_rarity()));
    if (materialCnt < 0)
    {
        return false;
    }

    std::map<int, GenuineDamageUpgradeInfo>::const_iterator it =
        table.m_infos.find(
            static_cast<int>(static_cast<unsigned char>(
                item->m_upgradeSep.GetUpgradeSeparate())) + 1);
    if (it == table.m_infos.end())
    {
        return false;
    }

    out->m_materialCnt = materialCnt;
    // ORIG 0x0811e138：pt() 后 flds 0xc(%eax) = pair+0xc = value+0x8 = m_levelRate；
    // fistpll 64 位截断后取低 dword（ORIG 0x0811e445 + mov -0x38/-0x34）。
    const long long goldWide =
        static_cast<long long>(9840.0f * it->second.m_levelRate);
    out->m_gold = static_cast<int>(goldWide);
    return true;
}