// [b28d-solo-LinkClosureB_Misc] AE：P46 同值 case 合并为 fallthrough 标签组
//（4.4 不合并同体叶块）+ P286 内层 switch 形 + 照抄原码遗留重复 else-if。
int CWebEvent::GetItemType(int eventKind, int itemNo)
{
    int type = 0;
    if (eventKind == 2)
    {
        switch (itemNo)
        {
        // [b28d-solo-LinkClosureB_Misc] P46：ORIG 仅 4 个共享叶块（序 1/2/3/4）
        // ⇒ 同值 case 必须写成 fallthrough 标签组（4.4 不合并同体叶块）。
        case 0xae68:
        case 0x9ed0:
        case 0xbe0c:
        case 0xcd81:
            type = 1; break;
        case 0xa060:
        case 0xaff8:
        case 0xbf9c:
        case 0xcf11:
            type = 2; break;
        case 0x659d:
            type = 3; break;
        case 999:    // 0x3e7
            type = 4; break;
        }
    }
    else if (eventKind == 3)
    {
        // P286：ORIG 为 switch 形（形参载入 %eax + 寄存器比较），非 if 链。
        switch (itemNo)
        {
        case 0x659e: type = 1; break;
        case 8:      type = 2; break;
        }
    }
    else if (eventKind == 3)
    {
        // ORIG 原码遗留的重复分支（Ghidra 亦显示两次 param_1 != 3）。
        switch (itemNo)
        {
        case 1:      type = 1; break;
        }
    }
    return type;
}