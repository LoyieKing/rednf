// ORIG _selectJewelTypeIndex 0x084af3b2：按凹槽数据加权随机选宝石类型槽。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af3b2 orig/ours=94/87
//   残差：全帧槽号整体平移 4（ORIG -0x38.. vs 我方 -0x34..）+ push ebx 差异
//   （4.1 不按 lifetime 复用栈槽，N82/P112）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=12 [N82/N93+P184] p167 CLEAR(2/2)；
// push %ebx（4.4 需 callee-saved）+ 槽序差 + ORIG 空 then 死比较 `cmpl $-1,-0x18`
// 被 4.4 DCE（P184 照抄不可复现）。
// [b28d-solo-2] verdict=DIFF：真缺口 = 权重乘法书写序。`counts[i]*(10000/total)`
// 令 4.4 空 push %ebx（+push/pop 2 条、帧 0x74 vs ORIG 0x78）；交换为
// `(10000/total)*counts[i]` 后 push/pop 消失、槽位与 ORIG 全等（探针 p13 A1 逐条同形）。
// 残余仅 ORIG 空 then 死块 3 条（`cmpl $-1,-0x18; jmp; nop`，4.1 空 if 发射，
// 4.4 DCE；while+break 反产 sete/test 形）⇒ P184 封顶。
// [b28e-solo-2] 复核（fresh .o 94/91，jcc 目标逐位核对：除该 3 条外 84af415..084af501
// 与 19bd..1aa2 逐条同目标）：该 3 条为 ORIG 084af44c `83 7d e8 ff` +
// 084af450 `eb 01` + 084af452 `90`，比较结果**无消费者**（jmp 无条件跳过 nop）⇒
// 4.1 空 then `if (ret==-1){}` 发射产物。4.4 前端整块 DCE：探针 /tmp/pg/p2.cpp
// a1..a9（空 if/else 空/switch/while-break/do-while/goto 尾标签/嵌套/for-break）+
// /tmp/pg/p4.cpp b1..b8（双 goto 同标签/else goto/双标签串/自赋值/(void)）共 17 形
// 全部无 cmp/jmp/nop 输出 ⇒ 无源码解，P184 封顶。
// [round5-A20 本轮新修·配方㉓] 空 `switch (ret == -1) { }` 复现 ORIG 0x084af44c 的
//   死比较 `cmpl $0xffffffff,-0x18(%ebp)`（旧 `if(ret==-1){}` 被 4.4 前端整块 DCE，
//   17 形探针全败）⇒ 92/93（gap -1，改前 91/93 gap -2）。残余仅 ORIG 0x084af450
//   `jmp +1; nop` 空 then 中继岛（P184/P223 边界：该 jmp 为 4.1 空块占位产物，
//   4.4 无对应发射点，`switch(ret){case -1:break;}` 形实测更劣 91）⇒ 封顶。
// [L1FreeTop12.S2 2026-09-11] N-JMP 再试：`switch(ret==-1){default:break;}` 实测同 92
//   （4.4 仍 DCE 该无条件 jmp+nop）⇒ 维持封顶。§6.5 复核：本件 ORIG 无串引用、无 PRETTY。
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
}