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
            if ((CUR->m_tutorialFlags & (1 << flag)) != 0)
                return false;
            CUR->m_tutorialFlags |= 1 << flag;
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
            if ((((signed char)CUR->m_tutorialFlags2[byteIndex]) >> bit & 1) != 0)
                return false;
            CUR->m_tutorialFlags2[byteIndex] =
                (unsigned char)(CUR->m_tutorialFlags2[byteIndex] | (1 << bit));
            return true;
        }
    }
    return false;
