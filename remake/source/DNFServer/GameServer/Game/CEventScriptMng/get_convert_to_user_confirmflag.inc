int CEventScriptMng::get_convert_to_user_confirmflag(int key)
{
    // P46：稀疏 switch 叶块序=case 书写序（0,1,2 与 ORIG 0x810b0cc 块序
    // 一致）；4.4 决策树根取中值 0x192（与 ORIG 首比较同）。
    // ORIG 有 v 槽（-0x4）：初值 -1 + 各 case 槽写 + 尾 mov -0x4,%eax 回读。
    int v = -1;
    switch (key)
    {
    case 0x191:
        v = 0;
        break;
    case 0x192:
        v = 1;
        break;
    case 0x19c:
        v = 2;
        break;
    }
    return v;
