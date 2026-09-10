BoosterSelectInfo::BoosterSelectInfo()
{
    // [b28c-27] AE：ORIG 0x8513bb8 先写 m_field18 再 vector::clear（源码语句序
    // 必须同序；原序 clear→field18 使 landing pad 布局差 2 条）。
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field18 = '\0';
    m_nameVec.clear();
