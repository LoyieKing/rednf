void CUser::setCharacSlotCountInfo(unsigned char effectCnt, unsigned char limit)
{
    // setCharacSlotCountInfo.md @0x0868b480：零参回退默认值分支。
    if (effectCnt == 0 || limit == 0)
    {
        // ORIG 0x0868b480+0x??：call 后 `mov %al,-0x9(%ebp)`（1B 槽=uchar 型截断直存）、
        // 两处实参 `movzbl -0x9(%ebp),%eax` ⇒ def 声明 uchar 非 int
        const unsigned char def = static_cast<unsigned char>(getCharacSlotCountDefaultInfo());
        setSlotEffectCount(def);
        setCharacSlotLimit(def);
    }
    else
    {
        setSlotEffectCount(effectCnt);
        setCharacSlotLimit(limit);
    }
