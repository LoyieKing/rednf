void WarRoom::CalcGuildFP()
// [P8S6 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    // [b28d-solo-WarRoom-5] ORIG 0x086bbea6：**158 orig / 156 our（gap +2，全部为
    //   P223/N121 岛族）**。本轮第③修后：原 164（多 13 条）⇒ 156。
    //   本轮修两处（探针 /tmp/wr4/a.cpp）：
    //   ① 数组槽序翻正：ORIG local_4c[12]@-0x48..-0x3d（深）+ local_40[6]@-0x3c..-0x25；
    //      4.4 给**带初始化列表的数组**优先深槽 ⇒ 交换声明序（local_40 先、local_4c 后）
    //      = a2 命中 ORIG（a1=旧形）。
    //   ② 商只算一次：ORIG 086bc043 `mov %eax,-0x14(%ebp)` 存 local_18、086bc075
    //      循环内 `mov -0x14(%ebp),%edx` 复用（Ghidra 把复用显示成第二次除法=假象）
    //      ⇒ 源码 `m_slotFp[local_10] = local_18;`，消去重复 idiv（-5 条）。
    //   ③ [本轮] 12 字节区改 `WrBlock6 local_4c[2]` 双视角（定义见上）：ORIG 的宽清零
    //      `movl $0`+`movw $0`（086bbed0/086bbfaa）与 `-0x42(%ebp,%eax,1)` 单条 scaled
    //      索引（086bbf43/086bbf38 等 3 处）**同时复现**（P235 双视角 + P311 成员数组
    //      视图）；此前 12×movb + 3 处 `add $6` 全部消去（-13 条）。
    //      配套：第一处（086bbf48 `jmp +0xa5`）与第二处（086bc01d/086bc020 双 jmp、
    //      086bbfe5 守卫）用 P223 goto 尾标签串复现 ⇒ **8 个岛中 6 个归位**。
    //   残余（P223 边界 N，附本轮 5 次实测矩阵）：第三处 ORIG `je +0x1d8(nop)` +
    //      086bc07c `jmp +0x1d9` 为「1 直落 + 1 岛」混合形、第四处 ORIG 为 3 jmp；
    //      4.4 岛数恒 = 条件数或 0 —— 双 goto 形（R1）产 2/2、正向 if 形（R3）产 0/2、
    //      continue 形（R2）同 2/2，均给不出 ORIG 的 1/3 分布（同 P223 边界 RemoveFPItem
    //      0x0850a314 判例，探针 l2_probe9 四式全败）。seq/槽/帧全等、p167 CLEAR 2/2
    //      ⇒ N 封顶。
    int local_40[6] = {0};
    WrBlock6 local_4c[2];
    int local_28;
    int local_24;
    int local_20;
    int local_1c;
    int local_18;
    int local_14;
    int local_10;
    local_4c[1].m_z.m_l = 0;
    local_4c[1].m_z.m_s = 0;
    for (local_28 = 0; local_28 <= 5; ++local_28)
    {
        if (m_slots[local_28] != 0)
        {
            m_slotFp[local_28] =
                (m_slotsUser[local_28])->GetFinishPointTotal();
            local_40[local_28] =
                (m_slotsUser[local_28])->get_charac_guildkey();
            if (local_40[local_28] != 0)
                goto L_28_JOIN;
            local_4c[1].m_b[local_28] = 1;
L_28_JOIN:
            ;
        }
    }
    for (local_24 = 0; local_24 <= 5; ++local_24)
    {
        // [b28d-solo-WarRoom-5] ORIG 086bbf75 je→+0x1ea(岛1)、086bbf85 jne→+0x1ed
        //   (岛2)、086bc08e jmp→+0x1f1 ⇒ 两守卫两岛 + body 尾 goto（P223 多岛形）。
        if (m_slots[local_24] == 0)
            continue;
        if (local_4c[1].m_b[local_24] != 0)
            continue;
        {
            local_20 = m_slotFp[local_24];
            local_1c = 1;
            local_4c[1].m_b[local_24] = 1;
            local_4c[0].m_z.m_l = 0;
            local_4c[0].m_z.m_s = 0;
            local_4c[0].m_b[local_24] = 1;
            for (local_14 = local_24 + 1; local_14 <= 5; ++local_14)
            {
                // [b28d-solo-WarRoom-5] ORIG 086bbfd9/086bbfe5/086bbff7 三守卫各带岛
                //   （P223 多岛定界）：守卫1 落独立岛 +0x179（nop;jmp JOIN），守卫2/3
                //   落 +0x17c/+0x17d；改写为负条件 goto + 标签串复现。
                if (m_slots[local_14] == 0)
                    goto L_14_ISLAND1;
                if (local_4c[1].m_b[local_14] != 0)
                    goto L_14_JOIN;
                if (local_40[local_24] == local_40[local_14])
                {
                    local_20 += m_slotFp[local_14];
                    local_1c += 1;
                    local_4c[1].m_b[local_14] = 1;
                    local_4c[0].m_b[local_14] = 1;
                }
                goto L_14_JOIN;
L_14_ISLAND1:
                goto L_14_JOIN;
L_14_JOIN:
                ;
            }
            // [S3-W8 2026-09-11] **P285 定案（本件由 DIFF 升 AE）**：ORIG
            //   086bc036 `jle 086bc096 <+0x1f0>` 落 **nop 岛**（+0x1f0）后直落
            //   +0x1f1 addl ⇒ if 的 else 岛是尾部第三岛；原正向 `if (local_1c > 1){}`
            //   被 4.4 折叠成 `jle → addl`（无岛），并使 continue#2 的岛直落 addl
            //   ⇒ 尾部只有 2 jmp + 2 nop（163 条）vs ORIG 3 jmp + 3 nop（165 条）。
            //   改**负条件 goto 尾空标签**后：
            //     +0x1e8 jmp（主路径）、+0x1ea nop 岛（continue#1）→+0x1eb jmp、
            //     +0x1ed nop 岛（continue#2）→+0x1ee jmp、+0x1f0 nop 岛（本 if 的
            //     else，jle 目标）直落 +0x1f1 ⇒ 三岛三 jmp 全等，**AE**。
            if (local_1c <= 1)
                goto L_24_TAIL;
            {
                local_18 = local_20 / local_1c;
                for (local_10 = 0; local_10 <= 5; ++local_10)
                {
                    // [b28d-solo-WarRoom-5] ORIG 086bc05e je→+0x1d8(nop 岛)、
                    //   086bc06a je→+0x1d9(incr)、086bc07c jmp→+0x1d9 ⇒ goto 尾标签串
                    //   （P223 定案形，与 SendGuildFP 同族）。
                    if (m_slots[local_10] == 0)
                        goto L_10_NOP;
                    if (local_4c[0].m_b[local_10] != 0)
                    {
                        m_slotFp[local_10] = local_18;
                        goto L_10_END;
                    }
                L_10_NOP:
                    ;
                L_10_END:
                    ;
                }
            }
        }
        continue;
        // 尾部第三岛落点（P285）：空标签即 ORIG 086bc096 nop 岛。
L_24_TAIL:
        ;
    }
}