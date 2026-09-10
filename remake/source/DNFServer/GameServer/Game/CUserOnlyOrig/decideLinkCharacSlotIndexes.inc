void CUser::decideLinkCharacSlotIndexes(char idx, int maxIdx)
{
    // ORIG 0x08652b06+0x??：`call getCharacLinkType; movsbl %al,%eax; mov %eax,-0xc(%ebp)`
    // ⇒ 4B int 槽（char 返回值符号扩展物化），比较恒 cmpl
    const int linkType = static_cast<int>(getCharacLinkType());
    // ORIG 0x08652b2a/0x08652b30：两比较各有独立落岛 `jge +0x83(nop;jmp+0x87)`、
    // `js +0x86(nop)`——单 label 合并被 4.4 threading 折叠（实测只剩 -2 jmp）。
    // 技巧23/24 双 goto 钉两岛，末 else call 保 `jmp END`（技巧7 尾置 return）。
    if (!(idx < maxIdx))
        goto SKIP_LINK_A;
    if (!(-1 < idx))
        goto SKIP_LINK_B;
    {
        if (linkType == 2) {
            setLinkCharacSlotIndex(idx + 1);
        }
        else if (linkType == 3) {
            setLinkCharacSlotIndex(idx - 1);
        }
        else {
            setLinkCharacSlotIndex(-1);
        }
    }
    return;
SKIP_LINK_A:;
SKIP_LINK_B:;
