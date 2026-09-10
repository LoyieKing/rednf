STAICharacterMapArrangeData::STAICharacterMapArrangeData(
    const STAICharacterMapArrangeData& other)
    : m_type(other.m_type),
      m_field04(other.m_field04),
      m_field08(other.m_field08),
      m_field0c(other.m_field0c),
      m_camp(other.m_camp),
      m_roleType(other.m_roleType),
      m_posX(other.m_posX),
      m_posY(other.m_posY),
      m_championFeatures(other.m_championFeatures),   // copy 于 +0x28 前
      m_npcIdx(other.m_npcIdx),
      m_collisionGroup(other.m_collisionGroup)        // 尾部向量拷贝
{
