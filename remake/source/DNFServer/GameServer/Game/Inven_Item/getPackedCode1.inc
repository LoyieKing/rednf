int Inven_Item::getPackedCode1() const
{
    // ORIG 0x850d102：全程 or %eax,mem 形需 code 为 unsigned int（probe p2 全等）。
    unsigned int code = 0;
    if (m_field0 != 0)
    {
        code |= 1;
    }
    code |= (m_field1 & 0x7) << 1;
    code |= m_addInfo << 4;
    code |= m_addInfo2 & 0xfff00000;
    return code;
}