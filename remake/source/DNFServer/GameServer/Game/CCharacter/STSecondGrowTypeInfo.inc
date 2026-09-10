STSecondGrowTypeInfo::STSecondGrowTypeInfo()
    // ORIG W 0x08373934：两组数组循环构造后直接 call clear()；
    // m_fieldbc 不在 C1 写（仅 clear 的 memset 2B 覆盖）。
    : m_addInfo(), m_skills()
{
    clear();   // ORIG 0x8373a1e：成员构造后调 clear()（m_fieldbc 低 2 字节置 0）
