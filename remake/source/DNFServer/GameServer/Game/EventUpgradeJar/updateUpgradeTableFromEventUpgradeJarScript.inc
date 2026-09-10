bool updateUpgradeTableFromEventUpgradeJarScript(upgrade_table_t* table, EventUpgradeJarInfo* info)
{
    // [b28d-solo-EventUpgradeJar] P133：ORIG 0x08968d60/0x08968d70 两条独立守卫
    // 各发 `cmpl $0; jne <下一检查>` + fall-through 早退块（合并 `||` 反产 je 跳早退）。
    if (table == 0)
        return false;
    if (info == 0)
        return false;

    for (unsigned int i = 0; i < info->m_npcScripts.size(); ++i)
    {
        if (i < table->m_infos.size())
        {
            // b28d-lane25：P167 差集实证 ORIG 对 m_infos[i] 发两次 operator[]
            // （P48 无 CSE，禁引用缓存）。
            table->m_infos[i].m_recipeItem = 0x28be7b;
            table->m_infos[i].m_meterialCount = (int)info->m_npcScripts[i].m_meterialCount;
        }
    }

    // 对 upgrade_table_t 主体表（m_body）的 100 项应用折扣：
    // 绝对偏移 (i+0x28)*4+4 处为 int 计数值，乘 m_discountRate 后四舍五入回写。
    // （ORIG： *(int*)(param_1 + (i+0x28)*4+4) = round(旧值 * discount)）
    // [b28d-solo-EventUpgradeJar] 用 dword 下标视图：ORIG 0x08968e0b 每轮
    // `mov i,%ecx/edx; add $0x28,%edx; mov 0x4(%eax,%edx,4),%eax` 两次重算
    // （P157 无缓存、P48 双访问）；m_i32 相对 m_body(+0xc) 的等效下标 = i+0x26。
    // N 封顶（P30/N81 族）：ORIG 基址 table+4/索引 i+0x28；我方视图基址只能
    // m_bodyView(+0xc) ⇒ `lea 0x26(%eax),%edx; mov 0xc(%eax,%edx,4)` —— 同一地址
    // 0xa4+4i，仅 4.1/4.4 对「前导成员偏移」的 add/disp 拆分常数不同
    // （探针 /tmp/eu_probe7.cpp fA 复证）。指令数 83=83 全等、调用点 p167 全清。
    // 乘法书写序按 P243：ORIG 先 fildl 旧值（-0x1c）后 flds 折扣率 ⇒ 旧值在左。
    for (unsigned int i = 0; i < 100; ++i)
    {
        // [round1-L19 2026-09-09] 基址 table+4/索引 i+0x28 形已实测**反劣**
        // （83/83 gap=0 → 83/87 gap=-4、ndiff 12→20：4.4 折成
        // `add $0x29; shl $2; lea`，不产出 ORIG 的 `add $0x28;disp 4` 两步形）
        // ⇒ 维持 m_bodyView 视图（P30/N81/N4 封顶，仅前导偏移拆分常数不同）。
        UPG_TBL_I32(table, (int)i + 0x26) =
            (int)((float)UPG_TBL_I32(table, (int)i + 0x26) * info->m_discountRate);
    }

    return true;
