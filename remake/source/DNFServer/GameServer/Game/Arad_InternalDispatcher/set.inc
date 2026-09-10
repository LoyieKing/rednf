void arad::SigSaveRewardEventItem::set(int a, unsigned int b, unsigned int c,
                                       unsigned long d, short e, unsigned int f)
{
    // ORIG 0x08195d18 栈槽：0xc→+0x00、0x10→+0x04、0x14→+0x08、
    //   0x18(d 低32位)→+0x10、0x1c(e 低16位)→+0x14、0x20(f)→+0x0c。
    m_field00 = a;
    m_field04 = b;
    m_field08 = c;
    m_field10 = (unsigned int)d;
    m_field14 = (short)e;
    m_field0c = f;
