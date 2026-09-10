int CBoosterTest::_selectJewelTypeIndex(unsigned int count,
                                        unsigned short const* sockets) const
{
    int counts[5] = {0, 0, 0, 0, 0};
    // [b28d-solo-GmCmds_A] ORIG +0x029..+0x045 对 cumul[5] 也逐条清零
    // （-0x4c..-0x3c 五条 movl $0）⇒ 数组带零初始化列表。
    int cumul[5] = {0, 0, 0, 0, 0};
    int total = 0;
    // [b28d-solo-GmCmds_A] ORIG 清零序 counts→cumul→total(-0x50)→running(-0x24)
    // →i(-0x1c) ⇒ running 声明在首个循环之前。
    int running = 0;

    for (unsigned int i = 0; i < count; ++i)
    {
        if (sockets[i] != 0)
        {
            // [b28d-solo-GmCmds_A] P184：ORIG +0x097 保存返回值到 -0x18 并
            // `cmpl $0xffffffff,-0x18`（then 为空块，4.1 仍发射）⇒ 照抄死比较。
            int ret = getAvatarEmblemTypeIdxForCompountEmblem(sockets[i], counts,
                                                              total);
            // [round5-GmCmdsA] 配方㉓（Main 2026-09-10，A14 实证）：ORIG 0x084af44c
            // `cmpl $0xffffffff,-0x18; jmp +1; nop` 是**无分支消费者的死比较**，
            // 4.4 前端对 `if(ret==-1){}`/`ret==-1;`/while-break 等 17 形恒 DCE
            // 或产 sete/test ⇒ 旧判 N99/P184。改**空 switch** 即产纯比较后直落。
            switch (ret == -1)
            {
            }
        }
    }
    if (total == 0)
    {
        return -1;
    }

    for (int i = 0; i <= 4; ++i)
    {
        // [b28d-solo-2] 乘法书写序：`counts[i] * (10000/total)` 使 4.4 空
        // push %ebx（+1 条、帧 0x74 vs ORIG 0x78）；交换为
        // `(10000/total) * counts[i]` 后逐条复现 ORIG（探针 p13 A1）。
        int weight = (10000 / total) * counts[i];
        cumul[i] = weight + running;
        running += weight;
    }

    int picked = get_rand_int(running);
    for (int i = 0; i <= 4; ++i)
    {
        if (picked < cumul[i])
        {
            return i;
        }
    }
    return -1;
