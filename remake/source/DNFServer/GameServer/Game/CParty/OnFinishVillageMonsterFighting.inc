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
