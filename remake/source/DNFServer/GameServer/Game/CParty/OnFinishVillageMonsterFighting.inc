    // [G7-sub 2026-09-10 复核·PacketGuard/EH] ORIG 0x085b99ac 的 PacketGuard 站点：
    // C1 ×2（ack -0x5c @0x085b9b21；packet -0x74 @0x085b9c39），两者生命周期重叠
    // （外 try 085b9b2c..085b9c3d / 内 try 085b9c54..085b9d47）⇒ **不是复用同一对象**。
    // 我方 C1 也是 2 个（数量一致，未发明对象）。我方 p167 +1 `_Unwind_Resume` 的真因：
    // ORIG 内层 catch 0x085b9dbf 清 -0x74 后 `jmp` 外层 catch 入口 0x085b9df9，
    // 共享唯一 _Unwind_Resume（0x085b9e0f）；4.4 给内层单独发一条 ⇒ 属 N104
    // （EH 尾块相对序）族，非源码可解。本 .inc 段（members[4] 清零 + 两循环）
    // 与 ORIG 0x085b99bd..0x085b9a76 逐点一致（clear 分支 `je <body>` 阳性 then
    // +0x085b9a9e 已对齐）。⇒ 封顶，不动。
int CParty::OnFinishVillageMonsterFighting(CUser* user, bool flag)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager + vector operator[]/size +
    // EXTRA const_iterator 6 项（P96 迭代器形，改型回归实测）。@0x85b99ac
    CUser* members[4] = {0, 0, 0, 0};                   // -0x54
    for (int i = 0; i < 4; ++i)                         // -0x3c
    {
        // ORIG 0x085b99f4 `xor $0x1,%eax; test %al,%al; jne <nop 岛>` ⇒ 负极性早退
        //（P168a②/裸形：`if (!f()) continue;`），旧码正向 if 缺 xor 段。
        if (!checkValidUser(i))
            continue;
        members[i] = get_user(i);
    }
    if (user == 0)
    {
        for (int i = 0; i < 4; ++i)                     // -0x38
        {
            if (checkValidUser(i))
            {
                user = get_user(i);
                break;
            }
        }
    }
    // ORIG 0x085b9a7c `mov $0x3,%ebx` + 0x085b9e14 `mov %ebx,%eax` ⇒ 单一 ret 出口。
    int ret = 0;                                        // -0x3c
    if (user == 0)
    {
        ret = 3;
    }
    else
    {
    // ORIG 0x085b9a86/0a8a 先两次 `movb $0x0` 预清，再按 clear 分支各置 1
    //（`test %al,%al; je <else>` 阳性 then = isClear=true；-0x3e=notClear 为死写）。
    bool notClear = false;                              // -0x3e
    bool isClear = false;                               // -0x3d
    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
