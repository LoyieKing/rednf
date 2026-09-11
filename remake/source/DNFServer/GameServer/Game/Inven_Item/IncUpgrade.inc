void Inven_Item::IncUpgrade()
{
    // ORIG 0x854b4be：读侧截断留在 eax、this 重载 edx（probe i4 ++ 形全等）。
    m_attrBits.m_attrLo++;
}