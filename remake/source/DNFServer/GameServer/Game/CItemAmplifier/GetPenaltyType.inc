//   返回 3 或 upgrade_info +0x24 惩罚类型字段（importUpgradeTable 的
//   infoInt(info,0x24) 脚本导入值；ORIG 反汇编 mov 0x24(%eax),%eax 实证，
//   非 m_discountBase(+0x34)）。
// ============================================================================
int upgrade_table_t::GetPenaltyType(unsigned char upgrade, ENUM_RARITY rarity,
                                    const upgrade_info_t* info) const
{
    // ORIG 两条件各自重读表项（无 th 局部），upgrade 物化 byte 后 movzbl；
    // 表 = +0x404 基 int 数组按 rarity 直接下标（两步 add/disp 形）。
    // 分支形为「反条件 return 3 前置」（pen2 实验：cmp;jl → then 后置）。
    if (m_penalty[rarity] != -1 && (int)upgrade >= m_penalty[rarity])
        return 3;
    return info->m_prob20[1];   // +0x24：ORIG `mov 0x24(%eax),%eax` 单条访存
}