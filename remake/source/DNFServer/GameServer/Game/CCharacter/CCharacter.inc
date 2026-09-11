// ORIG CCharacter::CCharacter @ 0x834828c：
//   _Additioal_info[7]（this+0 起，步长 0x52）
//   vector[7]（+0x240，步长 0xc）
//   STSecondGrowTypeInfo[7]（+0x294，步长 0xc0）→ clear()。
CCharacter::CCharacter()
    : m_baseInfo0(), m_baseInfoGrow(), m_giveSkillList(), m_secondGrowTypes()
{
    // ORIG 0x834828c：各成员构造完成后调 clear()。
    clear();
}