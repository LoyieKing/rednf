int CUserCharacInfo::getCurCharacEscaladeTutorialFlag(unsigned int flag)
{
    if (m_selected)
    {
        if (flag < 0x20)
        {
            // [b28d-L14 实证] ORIG 0x0864751e 无 mask 槽：`1<<flag` 以表达式内联
            // （mov $1,%ebx; mov %ebx,%esi; shl %cl,%esi），P0 禁具名局部缓存。
            // [b28d-solo-CUserCharacInfo N6 翻案] `1 << flag` 保 shl（探针 p12 f4）。
            if ((CUR->m_tutorialFlags & (1 << flag)) != 0)
                return 1;
        }
        else if (flag < 0x80)
        {
            int index = (int)(flag - 0x20);
            // [b28d-solo-CUserCharacInfo 路由已批] `index / 8` + i486 产 ORIG 6 条
            // （同 setCurCharacTutorialFlag；探针 /tmp/s156_probe/p13i486.o）。
            int byteIndex = index / 8;
            // [b28d-solo-CUserCharacInfo P0/P222] ORIG 0x0864757a 有独立 bit 槽 -0xc
            // （先算 index%8 再边界检查）且两守卫走 P222 正向包裹（js 尾部共享 false 块）。
            int bitIndex = index % 8;
            if (byteIndex >= 0)
            {
                if (byteIndex <= 0xf)
                    goto PASS;
            }
            return 0;
        // [b28d-solo-CUserCharacInfo P169] 复现 ORIG 0x0864757a 段
        // `js <false>; cmpl $0xf; jle <body>; mov $0; jmp; <body>:`。
        PASS:
            return (int)((signed char)CUR->m_tutorialFlags2[byteIndex]) >> bitIndex & 1;
        }
    }
    return 0;
