int WongWork::CWebEvent::_GetATIAvatarAbility(int itemIdx)
{
    // ORIG T 0x086c1a2e (size 0x8c)：离散映射 0x9ed0→1、0xae68→6、0xbe0c→4，
    // 其余（含 0xa060/0xaff8/0xbf9c/0xcd81/0xcf11 槽）→0。ret 栈变量形态按
    // ORIG 字节序（各 case 写 -0x4(%ebp)，尾部 mov -0x4(%ebp),%eax；无 default，
    // 0xcd81 独立 body、其余 0 值 case fall-through 共享 body——见 ORIG 布局）。
    int ret = 0;
    switch (itemIdx)
    {
        case 0x9ed0:
            ret = 1;
            break;
        case 0xae68:
            ret = 6;
            break;
        case 0xbe0c:
            ret = 4;
            break;
        case 0xcd81:
            ret = 0;
            break;
        case 0xa060:
        case 0xaff8:
        case 0xbf9c:
        case 0xcf11:
            ret = 0;
            break;
    }
    return ret;
}