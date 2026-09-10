void RidableObject::clear()
{
    // ORIG 0x830e47c：前 5 个字段写 -1，+0x14（m_field18）写 0xc8（200）。
    m_x = -1;
    m_y = -1;
    m_field0c = -1;
    m_field10 = -1;
    m_field14 = -1;
    m_field18 = 0xc8;
