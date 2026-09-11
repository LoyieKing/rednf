void WarRoom::SendGuildFP()
{
    // [SweepWarRoom] NEAR | ORIG 0x086bc0b0 | 本轮三循环岛形修复后 271=271（gap 0），
    //   TU 判词由 DIFF 升 NEAR（identical_ae 113/242→113/242、near 1→2、diff 11→10）。
    //   ①第一循环：slot==0 独立 goto nop 岛 + body 尾 goto（ORIG 086bc110 je→086bc150
    //     nop、086bc13b je→086bc151、086bc14e jmp→086bc151）；
    //   ②第二循环：slot==0 goto nop 岛（ORIG 086bc1a6 je→086bc235）+ 两分支体尾 jmp；
    //   ③第三循环：三守卫三岛 goto 串（ORIG 086bc31c je→086bc369、086bc328 jne→
    //     086bc36c、086bc347 je→086bc36d；P223 多岛形）。
    //   [b28d-solo-WarRoom-4] 三标量槽序翻正 ⇒ **271=271 AE（离开 NEAR）**：
    //   ORIG i1@-0x20 / minFp@-0x1c / count@-0x18，原重建 i1@-0x1c / minFp@-0x18 /
    //   count@-0x20（块作用域致 4.4 把函数级 count 先占深槽）。判据⑩（全头部声明）
    //   必须按 **ORIG 槽深序 = 声明序**（i1, minFp, count）落地；三者全部提至函数头，
    //   块内仅赋值，且原 `int minFp = 0x7fffffff;` / `int count = 0;` 的声明位置
    //   改为独立赋值语句（保持 ORIG 指令序 086bc179 / 086bc2ea）。其余槽
    //   （-0x44 vector / -0x48 i2 / -0x4c it / -0x30 packet / -0x14 bestFp / -0x10 i3）
    //   两侧本就逐值相同。
    PacketGuard packet;
    packet.put_header(0, 0x60);
    packet.put_byte(GetWaiterCount());
    // [判据⑩ 槽序反转律 · b28d-solo-WarRoom-4] ORIG 槽：i1@-0x20(minFp -0x1c / count
    //   -0x18)；4.4 受块作用域影响把函数级 count 先占深槽 ⇒ 全头部声明按 ORIG 槽深序。
    int i1;
    int minFp;
    int count;
    for (i1 = 0; i1 <= 5; ++i1)
    {
        if (m_slots[i1] == 0)
            goto FP_NOP;
        // ORIG 086bc13b/086bc14e/086bc150：cond 假 je→086bc151(incr)、body 尾
        //   jmp→086bc151、slot==0 je→086bc150(nop 岛)；goto 尾标签串复现。
        if ((m_slotsUser[i1])->GetFinishPointTotal() <
            G_CDataManager()->m_warAreaCounter.m_kickPoint)
        {
            m_slotData[i1] = 9;
        }
        goto FP_END;
    FP_NOP:
        ;
    FP_END:
        ;
    }
    if (GetWaiterCount() > 4)
    {
        minFp = 0x7fffffff;
        std::vector<int> v;
        for (int i = 0; i <= 5; ++i)
        {
            if (m_slots[i] == 0)
                goto V_NOP;
            // ORIG 086bc1a6 je→086bc235(nop 岛)；两分支体尾 jmp→086bc236(incr)。
            if ((m_slotsUser[i])->GetFinishPointTotal() < minFp)
            {
                // b28d-lane25：P167 差集实证 ORIG（0x086bc1bc/0x086bc1db/0x086bc212）
                // 在比较与两分支内各自重新调用 GetFinishPointTotal（共 3 次），不缓存。
                minFp = (m_slotsUser[i])->GetFinishPointTotal();
                v.clear();
                v.push_back(i);
            }
            else if ((m_slotsUser[i])->GetFinishPointTotal() == minFp)
            {
                v.push_back(i);
            }
            goto V_END;
        V_NOP:
            ;
        V_END:
            ;
        }
        for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it)
        {
            m_slotData[*it] = 10;
        }
    }
    char local_3a[6] = {0};
    count = 0;
    while (count < GetWaiterCount())
    {
        int bestSlot = -1;
        int bestFp = -1;
        for (int i = 0; i <= 5; ++i)
        {
            // ORIG 086bc31c/086bc328/086bc347：三守卫三岛（je→086bc369 nop、
            //   jne→086bc36c nop、je→086bc36d incr），P223 多岛形复现。
            if (m_slots[i] == 0)
                goto B_NOP1;
            if (local_3a[i] != 0)
                goto B_NOP2;
            if (bestFp <= (m_slotsUser[i])->GetFinishPointTotal())
            {
                bestFp = (m_slotsUser[i])->GetFinishPointTotal();
                bestSlot = i;
            }
            goto B_END;
        B_NOP1:
            ;
        B_NOP2:
            ;
        B_END:
            ;
        }
        if (bestSlot == -1)
        {
            // [b28d-solo-WarRoom P97/P244] ORIG 0x086bc3xx：vararg 从具名局部槽
            //   -0x14(%ebp) 装载（`mov -0x14(%ebp),%eax; mov %eax,0x14(%esp)`），
            //   即实参来源是 bestSlot（此处恒 -1），非字面量 0xffffffff。
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x430,
                                  "maxPlayer (%d)", bestSlot);
        }
        local_3a[bestSlot] = 1;
        packet.put_byte(bestSlot);
        packet.put_byte(m_slotData[bestSlot]);
        ++count;
    }
    packet.finalize(true);
    SendToRoom(packet);
}