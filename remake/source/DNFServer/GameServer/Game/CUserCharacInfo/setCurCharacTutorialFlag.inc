// [b28d-solo-CUserCharacInfo AE] 四旋钮归零（108=108）：
// ①N6 翻案：`1u << flag` → `1 << flag`（int 字面量）保 ORIG `mov $1,%ebx;
//   mov %ebx,%esi; shl %cl,%esi`（`1u` 被 4.4 折成 `shr %cl; and $1`；
//   探针 /tmp/s156_probe/p12.cpp f1 vs f4）；
// ②P169 尾标签守卫（`if(byteIndex<=0xf) goto PASS; return false; PASS:`）复现
//   ORIG `js <false>; cmpl $0xf; jle <body>; mov $0; jmp`（P222 包裹恒产 jg 岛）；
// ③or 操作数序改 field 先（ORIG 先读 field 再算 1<<bit，寄存器 %ebx/%edi 归位）；
// ④路由 `-march=i486 -mtune=generic`（Main 2026-09-09 已批）+ `index / 8` 复现
//   ORIG 6 条符号修正链（探针 /tmp/s156_probe/p13i486.o）。
// [P8 2026-09-11 新口径复核] n1_v（objdump -dr + reloc 归一）verdict=AE，108/109；
//   旧判词 NEAR 系 `game_func_report.py --no-compile`（objdump 无 -r）的假残差。
bool CUserCharacInfo::setCurCharacTutorialFlag(unsigned int flag)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        if (flag < 0x20)
        {
            // [b28d-L14 实证] ORIG 0x08647466 无 mask 槽：表达式内联（P0）。
            // [b28d-solo-CUserCharacInfo N6 翻案] `1 << flag`（int 字面量）保 ORIG 的
            // `mov $1,%ebx; mov %ebx,%esi; shl %cl,%esi`；`1u <<` 被 4.4 折成
            // `shr %cl; and $1`（探针 /tmp/s156_probe/p12.cpp f1 vs f4）。
            if ((m_selected->m_field51 & (1 << flag)) != 0)
                return false;
            m_selected->m_field51 |= 1 << flag;
            return true;
        }
        if (flag < 0x80)
        {
            int index = (int)(flag - 0x20);
            // [b28d-solo-CUserCharacInfo N97 复核] `index / 8` 实测 4.4 产
            // `lea 0x7(%eax); test; cmovs`（默认旗标，cmovs 形 ORIG 无）；
            // [b28d-solo-CUserCharacInfo 路由已批] 本 TU 走 `-march=i486 -mtune=generic`
            // （dnf_game_cxx.sh i486 组，Main 2026-09-09 落）后 4.4.7 对 `index / 8`
            // 产 ORIG 精确 6 条（mov/mov/sar/shr/lea/sar，探针 /tmp/s156_probe/p13i486.o）。
            int byteIndex = index / 8;
            // [b28d-solo-CUserCharacInfo P0/P222] ORIG 0x086474ad 有 int bit 槽 -0x1c
            // （先算 index%8 再边界检查）且两守卫走 P222 正向包裹（js 尾部共享 false 块）。
            int bit = index % 8;
            if (byteIndex >= 0)
            {
                if (byteIndex <= 0xf)
                    goto PASS;
            }
            return false;
        // [b28d-solo-CUserCharacInfo P169] 复现 ORIG 0x086474ad 段
        // `js <false>; cmpl $0xf; jle <body>; mov $0; jmp; <body>:`（P222 包裹恒产
        // `jg <skip>` 岛）。
        PASS:
            if ((((signed char)m_selected->m_tradeRestrictData[byteIndex]) >> bit & 1) != 0)
                return false;
            m_selected->m_tradeRestrictData[byteIndex] =
                (unsigned char)(m_selected->m_tradeRestrictData[byteIndex] | (1 << bit));
            return true;
        }
    }
    return false;
}