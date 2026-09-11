// ORIG 0848ecce：m_growType/m_secondGrowType 清零 + 两个 vector 构造后各自 clear()
STEventCharacterInfo::STEventCharacterInfo() : m_growType(0), m_secondGrowType(0)
{
    m_skillList.clear();
    m_itemList.clear();
}