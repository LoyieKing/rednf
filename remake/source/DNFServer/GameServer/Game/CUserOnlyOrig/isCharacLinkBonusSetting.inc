unsigned char CUser::isCharacLinkBonusSetting(char slot)   // CUser.h 已落 uchar（Main b28：4.4 int 返 uchar 恒两段，uchar 单段与 ORIG 尾全等）
{
    // ORIG 0x084ecd52（getLinkCharacNo 同族）：`movsbl -0xc,%ebx` 跨 get_charac_count
    // 存活 + `cmp %eax,%ebx; jge`/`cmpb $0,-0xc; jns` → mov$1/$0 物化 + `test%al;
    // je body`；尾 `movzbl 0x124f(%eax),%eax; ret` 直返无 setne ⇒ 原返回 int
    // （CUser.h 已 Main 落 bool→int，技巧33/P9）。
    // getLinkCharacNo 同族实证形（b28 fresh AE）：sext 左操作数跨 call 升格 %ebx。
    if ((static_cast<int>(slot) >= get_charac_count()) || (slot < 0))
        return 0;
    return m_characList[static_cast<int>(slot)].m_linkBonusSetting;
}