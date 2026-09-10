CCharacter::CCharacter()
    : m_baseInfo0(), m_baseInfoGrow(), m_giveSkillList(), m_secondGrowTypes()
{
    // ORIG 0x834828c：各成员构造完成后调 clear()。
    clear();
