// ORIG 0x080f4dac。按 m_materialKind 填充 upgrade_info_t：
//   kind==1（fortune）：info.m_meterialCount=1，m_recipeItem 在 m_materialIndices 中查
//     materialIdx 命中则取对应索引值；kind==2（newNPC）：m_meterialCount=打折后材料数、
//     m_recipeItem=0x28be7b、m_discountBase=round(discountRate*原值)。末写 m_materialIdx=upgrade。
void Event_Upgrade_Jar::SettingRecipeNUpConunt(int materialIdx, upgrade_info_t& info, unsigned char upgrade)
{
    // [b28d-solo-EventUpgradeJar] N 封顶（verdict=NEAR，助记符序列全等、指令数
    // 95=95）：残余① float base 槽 -0x10 vs ORIG -0x28（4.1 浮点局部落帧最深、
    // 4.4 落浅槽；N82/N95 族，pad/声明序探针反劣）；② kind==1 循环
    // `cmp %eax,%ebx` vs `cmp %ebx,%eax` 操作数序（N18b 4.4 恒折）。
    // [b28d-solo-EventUpgradeJar] P212/N98：ORIG 槽序 i=-0xc（浅）/base=-0x28（深），
    // 全头部声明「先声明深」——base 先声明、i 后声明即复现（块内声明 4.4 反向）。
    // [P6AuditOldN-S1 2026-09-11 负结论 P339③] 试声明序反转（i 先 / base 后）复现
    //   ORIG 槽序（i=-0xc 浅 / base=-0x28 深）：base 归位 -0x28 但 i 被挤到 -0x10
    //   ⇒ triage blocks **3→8**（差异面扩大，N18b `cmp %eax,%ebx` 仍在）⇒ 回滚，
    //   维持原形（base 先声明、i=-0xc 对齐；仅 base 槽差属 N82/N95 浮点槽族）。
    float base;
    int i;

    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4dac：kind==0 落 leave 前 nop 岛
    // （0x080f4edb），kind==1 尾 `jmp TAIL`、kind==2 fall-through 到 TAIL，TAIL 写
    // m_materialIdx 后 `jmp END` 跳过 nop 岛 ⇒ 负极性早退 goto + 中间 TAIL 标签
    // （P223 正例；探针 /tmp/eu_probe1.cpp fB）。
    if (m_materialKind == 0)
        goto END;

    if (m_materialKind == 1)
    {
        // ORIG：+0x34(m_discountBase)=0、+0x30(m_meterialCount)=1、
        // +0x2c(m_recipeItem)=m_materialIndices 中命中 materialIdx 的值。
        info.m_discountBase = 0;
        info.m_meterialCount = 1;
        // ORIG 循环比较为有符号 setg（P196：unsigned 生 seta）。
        for (i = 0; i < (int)m_materialIndices.size(); ++i)
        {
            // P231/P248：ORIG 0x080f4df9 每轮循环体首条 `mov 0xc(%ebp),%ebx`。
            register int midx = materialIdx;
            if (midx == (int)m_materialIndices[i])
            {
                info.m_recipeItem = m_materialIndices[i];
                break;
            }
        }
        goto TAIL;
    }
    if (m_materialKind == 2)
    {
        // b28d-lane25：P167 差集实证 ORIG 0x080f4d?? 调 _getDiscountRate_NewNPC()
        // 取折扣率（非直读 m_discountRate 成员）。
        // [b28d-solo-EventUpgradeJar] P252：ORIG 0x080f4e73 先读 m_discountBase 到
        // -0x20、fildl+fstps 存具名 float 局部（-0x28），**之后**才 call 折扣率 getter，
        // 再 fmuls 该槽 ⇒ 原码有具名 float 局部且求值序在 call 之前。
        base = (float)info.m_discountBase;
        info.m_discountBase = (int)(_getDiscountRate_NewNPC() * base);
        info.m_meterialCount = _getMeterialCount_NewNPC(upgrade);
        info.m_recipeItem = 0x28be7b;
    }
TAIL:
    m_materialIdx = upgrade;
END:;
}